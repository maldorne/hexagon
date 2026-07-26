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
 *
 * Design note: dev/location-cleaner.md.
 */

#include <room/location-cleaner.h>
#include <room/location.h>

// Per-game registry of live map objects: ([ game : ([ ob : reg_time ]) ]).
mapping buckets;
// Reverse index (ob -> game at registration) so deregister is exact even
// if the object's game would resolve differently once its environment is
// gone (e.g. during dest_me).
mapping ob_game;

// L1 coarse cache: ([ "<game>:<x>_<y>_<z>" : expiry_time ]). A move into a
// still-warm cell returns immediately without walking the neighbourhood.
mapping region_warmed;
// L2 fine cache: ([ file : expiry_time ]). A fresh file is not re-loaded or
// re-enqueued while its stamp holds.
mapping warmed;

// Prewarm queue: ({ ({ file, depth }), ... }), drained from `head` (FIFO
// without O(n) shifts). `queued` dedupes files already in flight.
mixed * queue;
int head;
mapping queued;

// call_out handle of the worker, 0 when idle.
int worker_handle;

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

  if (!file || !strlen(file))
    return nil;

  if (file[strlen(file) - 2 ..] == ".o")
    return load_object(LOCATION_HANDLER)->load_location(file);

  ob = find_object(file);
  if (ob)
    return ob;

  catch(ob = load_object(file));
  return ob;
}

// Enqueue a loaded object's exit destinations at `depth`, skipping files
// already in flight or still fresh.
private void _seed(object ob, int depth)
{
  string * dest_dir;
  int i;

  if (depth > CLEANER_RADIUS)
    return;

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

    queued[dest] = 1;
    queue += ({ ({ dest, depth }) });
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
    int depth;
    object ob;

    file  = queue[head][0];
    depth = queue[head][1];
    head++;
    map_delete(queued, file);

    ob = _ensure_loaded(file);
    if (!ob)
      continue;

    warmed[file] = time() + CLEANER_FILE_TTL;

    if (depth < CLEANER_RADIUS)
      _seed(ob, depth + 1);
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

  // L1 coarse gate: nothing to do if the player's current cell is warm.
  coords = env->query_coordinates();
  if (coords && sizeof(coords) >= 3)
  {
    string key;

    key = game_name(env) + ":" + coords[0] + "_" + coords[1] + "_" + coords[2];
    if (region_warmed[key] != nil && region_warmed[key] > time())
      return;
    region_warmed[key] = time() + CLEANER_REGION_TTL;
  }

  _seed(env, 1);
  _schedule();
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
