/*
 * Location cleaner handler.
 *
 * The single owner of every map object's lifetime — rooms and locations
 * alike — hence "cleaner": it is the one place that reclaims them.
 *
 * Owns the residency of every map object in the mud (rooms and locations
 * alike). It preemptively loads the graph neighbourhood around each player
 * so nearby content is live before the player arrives, keeps that
 * neighbourhood resident while a player is close, and (through the evict
 * sweep, added separately) reclaims memory when no player is near.
 *
 * Objects register themselves on create and deregister on destruct, so the
 * handler is the single reaper for the whole mud. State is bucketed per
 * game; the prewarm queue is drained by one global worker a few objects
 * per tick, so nothing is synchronous to the player and no single tick
 * overruns its budget.
 */

#include <room/location-cleaner.h>
#include <room/location.h>
#include <room/room.h>
#include <sector/sector.h>
#include <cartography.h>

// Per-game registry of live map objects: ([ game : ([ ob : reg_time ]) ]).
mapping buckets;
// Reverse index (ob -> game at registration) so deregister is exact even
// if the object's game would resolve differently once its environment is
// gone (e.g. during dest_me).
mapping ob_game;

// L1 coarse cache: ([ "<game>:<x>_<y>_<z>" : expiry_time ]). A move into a
// still-warm location returns immediately without walking the neighbourhood.
mapping region_warmed;
// L2 fine cache: ([ file : expiry_time ]). A fresh file is not re-loaded or
// re-enqueued while its stamp holds.
mapping warmed;

// Prewarm queue: ({ ({ file, steps, centre }), ... }), drained from `head` (FIFO
// without O(n) shifts). `queued` dedupes files already in flight.
mixed * queue;
int head;
mapping queued;

// call_out handle of the worker, 0 when idle.
int worker_handle;

// running total of objects reclaimed by the evict sweep (for introspection).
int evicted_total;

void create()
{
  buckets       = ([ ]);
  ob_game       = ([ ]);
  region_warmed = ([ ]);
  warmed        = ([ ]);
  queue         = ({ });
  head          = 0;
  queued        = ([ ]);
  worker_handle = 0;
  evicted_total = 0;

  // start the periodic evict sweep
  call_out("_evict_sweep", CLEANER_SWEEP_INTERVAL);
}

// ---------------------------------------------------------------------------
// Registration — called from room.c / location.c create and dest_me.
// ---------------------------------------------------------------------------

void register_object(object ob)
{
  string g;

  if (!ob)
    return;

  g = game_name(ob);
  if (!buckets[g])
    buckets[g] = ([ ]);
  buckets[g][ob] = time();
  ob_game[ob] = g;
}

void deregister_object(object ob)
{
  string g;

  if (!ob)
    return;

  g = ob_game[ob];
  if (g != nil && buckets[g])
  {
    map_delete(buckets[g], ob);
    if (!map_sizeof(buckets[g]))
      map_delete(buckets, g);
  }
  map_delete(ob_game, ob);
}

// ---------------------------------------------------------------------------
// Prewarm — walk the graph around a player and load it, chunked across ticks.
// ---------------------------------------------------------------------------

// A file is "fresh" while its load stamp holds; fresh files are neither
// re-enqueued nor reloaded.
private int _fresh(string file)
{
  return warmed[file] != nil && warmed[file] > time();
}

// Resolve a destination file to its live object, loading on demand — this
// is the whole point of prewarm. `.o` locations always go through the
// location handler (they are clones, not blueprints); `.c` rooms load
// directly.
private object _ensure_loaded(string file)
{
  object ob;
  string err;

  if (!file || !strlen(file))
    return nil;

  // Whatever one file does, the rest of the queue still has to be warmed: a
  // location that throws while restoring (or while bringing its people back)
  // must not take the worker down with it, or everything behind it in the
  // queue stays cold until the mud is restarted.
  if (file[strlen(file) - 2 ..] == ".o")
  {
    err = catch(ob = load_object(LOCATION_HANDLER)->load_location(file));

    if (err)
    {
      stderr("🧹 cleaner prewarm: " + file + " failed to load: " + err + "\n");
      return nil;
    }

    return ob;
  }

  ob = find_object(file);
  if (ob)
    return ob;

  err = catch(ob = load_object(file));

  if (err)
  {
    stderr("🧹 cleaner prewarm: " + file + " failed to load: " + err + "\n");
    return nil;
  }

  return ob;
}

// Whether coordinate `c` lies within CLEANER_RADIUS of `centre` on every axis.
private int _in_window(int * centre, int * c)
{
  int i, d;

  for (i = 0; i < 3; i++)
  {
    d = c[i] - centre[i];
    if (d > CLEANER_RADIUS || d < -CLEANER_RADIUS)
      return 0;
  }

  return 1;
}

// The coordinate an exit of `ob` leads to, worked out from the exit's compass
// direction so the destination does not have to be loaded to know where it
// is. nil when `ob` has no coordinate or the exit is not a compass direction.
private int * _dest_coordinate(object ob, string dir)
{
  int * from, * delta;

  from = ob->query_coordinates();
  if (!from || sizeof(from) < 3)
    return nil;

  delta = load_object(SECTORS_HANDLER)->query_dir_delta(
            ROOM_HAND->canonical_dir(dir));
  if (!delta)
    return nil;

  return ({ from[0] + delta[0], from[1] + delta[1], from[2] + delta[2] });
}

// Whether the walk may go on through the exit `dir` of `ob`, `steps` exits away
// from the player. With a centre, the coordinate window decides; a place that
// cannot say where it is (a room with no coordinates, an exit that is not a
// compass direction) falls back to counting exits.
private int _within_reach(object ob, string dir, int steps, int * centre)
{
  int * to;

  if (steps > CLEANER_MAX_STEPS)
    return 0;

  if (!centre)
    return steps <= CLEANER_RADIUS;

  to = _dest_coordinate(ob, dir);
  if (!to)
    return steps <= CLEANER_RADIUS;

  return _in_window(centre, to);
}

// Enqueue a loaded object's exit destinations `steps` exits from the player,
// skipping files already in flight or still fresh and anything outside the
// window around `centre`.
private void _seed(object ob, int steps, int * centre)
{
  string * dest_dir;
  int i;

  dest_dir = ob->query_dest_dir();
  if (!dest_dir)
    return;

  // query_dest_dir returns ({ dir, dest, dir, dest, ... }) pairs
  for (i = 0; i < sizeof(dest_dir); i += 2)
  {
    string dest;

    dest = dest_dir[i + 1];
    if (!dest || !strlen(dest))
      continue;
    if (queued[dest] || _fresh(dest))
      continue;
    if (!_within_reach(ob, dest_dir[i], steps, centre))
      continue;

    queued[dest] = 1;
    queue += ({ ({ dest, steps, centre }) });
  }
}

private void _schedule()
{
  if (worker_handle)
    return;
  worker_handle = call_out("_prewarm_step", 0);
}

// Worker: load up to CHUNK files, enqueueing the next ring from each, then
// reschedule until the queue drains. Not static: the driver invokes it by
// name through call_out.
void _prewarm_step()
{
  int i;

  worker_handle = 0;

  for (i = 0; i < CLEANER_CHUNK && head < sizeof(queue); i++)
  {
    string file;
    int steps;
    int * centre;
    object ob;

    file   = queue[head][0];
    steps  = queue[head][1];
    centre = queue[head][2];
    head++;
    map_delete(queued, file);

    ob = _ensure_loaded(file);

    // A file that could not be loaded is stamped like a loaded one, so a
    // broken location is retried when its stamp ages out instead of on every
    // walk through the neighbourhood.
    if (!ob)
    {
      warmed[file] = time() + CLEANER_FILE_TTL;
      continue;
    }

    warmed[file] = time() + CLEANER_FILE_TTL;

    _seed(ob, steps + 1, centre);
  }

  if (head < sizeof(queue))
  {
    worker_handle = call_out("_prewarm_step", 0);
  }
  else
  {
    // drained — release the FIFO backing storage
    queue = ({ });
    head  = 0;
  }
}

// Entry point for the player-enter hook. Seeds the neighbourhood walk from
// the player's environment, subject to the L1 cache.
void player_moved(object player)
{
  object env;
  int * coords;

  if (!player || !interactive(player))
    return;

  env = environment(player);
  if (!env)
    return;

  // L1 coarse gate: nothing to do if the player's current location is warm.
  coords = env->query_coordinates();
  if (coords && sizeof(coords) >= 3)
  {
    string key;

    key = game_name(env) + ":" + coords[0] + "_" + coords[1] + "_" + coords[2];
    if (region_warmed[key] != nil && region_warmed[key] > time())
      return;
    region_warmed[key] = time() + CLEANER_REGION_TTL;
  }

  _seed(env, 1, (coords && sizeof(coords) >= 3) ? coords : nil);
  _schedule();
}

// ---------------------------------------------------------------------------
// Evict — reclaim objects out of every player's range, past the grace period.
// ---------------------------------------------------------------------------

// The loaded object an exit leads to, without loading anything: retention
// only decides what to keep of what is already in memory.
private object _loaded_destination(string dest)
{
  if (!dest || !strlen(dest))
    return nil;

  if (dest[strlen(dest) - 2 ..] == ".o")
    return load_object(LOCATION_HANDLER)->query_loaded_location(dest);

  return find_object(dest);
}

// Walk the same window the prewarm fills, over what is already in memory, and
// mark every object met as retained. Anything inside the window that is not
// loaded is queued for the worker: the invariant is kept here, every sweep,
// and not only when the player moves -- a location that went away while the
// player stood still is brought back within one sweep.
private void _retain_around(object env, mapping retain)
{
  mixed * pending;
  int * centre;
  int i, steps;

  centre = env->query_coordinates();
  if (centre && sizeof(centre) < 3)
    centre = nil;

  retain[env] = 1;
  pending = ({ ({ env, 1 }) });

  while (sizeof(pending))
  {
    object here;
    string * dest_dir;

    here  = pending[0][0];
    steps = pending[0][1];
    pending = pending[1..];

    dest_dir = here->query_dest_dir();
    if (!dest_dir)
      continue;

    for (i = 0; i < sizeof(dest_dir); i += 2)
    {
      object there;
      string dest;

      dest = dest_dir[i + 1];
      if (!dest || !strlen(dest))
        continue;
      if (!_within_reach(here, dest_dir[i], steps, centre))
        continue;

      there = _loaded_destination(dest);

      if (!there)
      {
        if (!queued[dest])
        {
          queued[dest] = 1;
          queue += ({ ({ dest, steps, centre }) });
        }
        continue;
      }

      if (retain[there])
        continue;

      retain[there] = 1;
      pending += ({ ({ there, steps + 1 }) });
    }
  }
}

// The set of objects within CLEANER_RADIUS of any online player.
private mapping _retain_set()
{
  object * ps;
  mapping retain;
  int i;

  ps = players();
  retain = ([ ]);

  for (i = 0; i < sizeof(ps); i++)
  {
    object env;

    if (!ps[i])
      continue;
    env = environment(ps[i]);
    if (!env)
      continue;

    _retain_around(env, retain);
  }

  // whatever the walk found missing is loaded by the worker, a chunk a tick
  if (head < sizeof(queue))
    _schedule();

  return retain;
}

// Periodic sweep. Refresh the last-activity stamp of every registered object
// still in range; evict (via its own clean_up) anything out of range that has
// been idle past the grace period. Reschedules itself.
void _evict_sweep()
{
  mapping retain;
  string * games;
  object * victims;
  int now, i, g;

  now = time();
  retain = _retain_set();
  victims = ({ });

  games = map_indices(buckets);
  for (g = 0; g < sizeof(games); g++)
  {
    object * obs;

    obs = map_indices(buckets[games[g]]);
    for (i = 0; i < sizeof(obs); i++)
    {
      object ob;

      ob = obs[i];
      if (!ob)
        continue;

      if (retain[ob])
      {
        // still in range — refresh its last-activity stamp
        buckets[games[g]][ob] = now;
        continue;
      }

      if (now - buckets[games[g]][ob] > CLEANER_GRACE)
        victims += ({ ob });
    }
  }

  // evict after iterating: clean_up -> dest_me -> deregister_object mutates
  // the buckets, which is unsafe to do mid-iteration
  for (i = 0; i < sizeof(victims); i++)
    if (victims[i] && !victims[i]->clean_up())
      evicted_total++;

  call_out("_evict_sweep", CLEANER_SWEEP_INTERVAL);
}

// ---------------------------------------------------------------------------
// Introspection — for the admin inspection command and debugging.
// ---------------------------------------------------------------------------

// Compact one-shot snapshot of the handler's state.
mapping query_stats()
{
  mapping s;
  string * games;
  int i, total;

  s = ([ ]);
  games = map_indices(buckets);
  total = 0;
  for (i = 0; i < sizeof(games); i++)
    total += map_sizeof(buckets[games[i]]);

  s["games"]         = sizeof(games);
  s["registered"]    = total;
  s["queue_pending"] = sizeof(queue) - head;
  s["in_flight"]     = map_sizeof(queued);
  s["region_cached"] = map_sizeof(region_warmed);
  s["file_cached"]   = map_sizeof(warmed);
  s["worker_active"] = (worker_handle != 0);
  s["evicted_total"] = evicted_total;
  return s;
}

// Registered-object count per game.
mapping query_game_counts()
{
  mapping out;
  string * games;
  int i;

  out = ([ ]);
  games = map_indices(buckets);
  for (i = 0; i < sizeof(games); i++)
    out[games[i]] = map_sizeof(buckets[games[i]]);
  return out;
}

// Registered objects in one game bucket (copy — callers must not mutate).
object * query_registered(string game)
{
  if (!buckets[game])
    return ({ });
  return map_indices(buckets[game]);
}

int query_worker_active()   { return worker_handle != 0; }
int query_queue_pending()   { return sizeof(queue) - head; }
mapping query_region_warmed() { return map_copy(region_warmed); }
mapping query_warmed()        { return map_copy(warmed); }
