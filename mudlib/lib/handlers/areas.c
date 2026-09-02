
#include <areas/area.h>

mapping loaded_areas;
// Each game's areas, as found on disk. Static: it is a reading of the tree, not
// state of our own, and a reboot reads it again.
static mapping area_paths;

// ({ ({ area, uuid, hour }), ... }) collected each game hour and released a few
// per tick (one call_out chain) so a crowd ordered off at the same hour staggers
// out -- and, since each is woken as it is released, the load of materialising
// unloaded NPCs spreads out too, instead of hitting on one beat.
mixed * pending_schedule;

// Cross-area position index. Most NPCs stay in their roster area, found through
// that area's own census; a few roam (an NPC works in one area, sleeps in
// another). While such a roamer rests in a foreign location, this index lets
// that location, on load, find it without scanning every area's census.
//   npc_positions: foreign location_file -> ([ uuid : roster_area_path ])
//   npc_at:        uuid -> the location_file it is indexed at (one entry per uuid)
// Runtime only: rebuilt as NPCs roam. After a reboot a roamer is recovered by its
// roster area's scheduler (which loads its census position), not from this index.
mapping npc_positions;
mapping npc_at;

private void _delete_npc_folder(string dir);

void create() {
  area_paths = ([ ]);
  loaded_areas = ([ ]);
  pending_schedule = ({ });
  npc_positions = ([ ]);
  npc_at = ([ ]);
  // ::create();
}

// Find a live NPC by uuid across every loaded area (not just one), so a roamer
// that has wandered into another area is still recognised as already in the
// world -- the global dedup a per-area lookup cannot give. Returns nil if it is
// not currently materialized anywhere.
object find_live_npc(string uuid)
{
  object * areas;
  int i;

  if (!uuid || !strlen(uuid))
    return nil;
  areas = map_values(loaded_areas);
  for (i = 0; i < sizeof(areas); i++)
  {
    object npc;
    if (!areas[i])
      continue;
    npc = areas[i]->live_npc(uuid);
    if (npc)
      return npc;
  }
  return nil;
}

// Record that a roamer rostered in `roster` is now resting at `loc` (a location
// in another area), or clear it with loc = nil. Keeps a single entry per uuid, so
// a stale bucket is dropped when the NPC moves on.
void set_foreign_position(string uuid, string roster, string loc)
{
  string old;

  if (!uuid || !strlen(uuid))
    return;

  old = npc_at[uuid];
  if (old && npc_positions[old])
  {
    map_delete(npc_positions[old], uuid);
    if (!map_sizeof(npc_positions[old]))
      map_delete(npc_positions, old);
  }

  if (loc && strlen(loc))
  {
    if (!npc_positions[loc])
      npc_positions[loc] = ([ ]);
    npc_positions[loc][uuid] = roster;
    npc_at[uuid] = loc;
  }
  else
    map_delete(npc_at, uuid);
}

// The roamers indexed at `location_file`: ([ uuid : roster_area_path ]). Empty
// when no roamer is resting there.
mapping foreign_positions_at(string location_file)
{
  return npc_positions[location_file] ? npc_positions[location_file] : ([ ]);
}

mapping query_loaded_areas() {
  return loaded_areas;
}

// Every area of a game, loaded or not. There is no register of them: an area
// exists because there is an area.o in its directory, so the tree under the
// game's locations is the list, and walking it is the only answer that cannot
// go stale. Cached, because the walk is the same until somebody converts a new
// area -- create_area adds to the cache when that happens.
string * query_area_paths(string game)
{
  string * queue, * found;
  mixed * entries;
  string dir;
  int i;

  if (!game || !strlen(game))
    return ({ });

  if (area_paths[game])
    return area_paths[game];

  queue = ({ "/save/games/" + game + "/locations/areas/" });
  found = ({ });

  while (sizeof(queue))
  {
    dir = queue[0];
    queue = queue[1 ..];

    if (file_size(dir + "area.o") >= 0)
      found += ({ dir });

    entries = get_dir(dir + "*", -1);
    for (i = 0; i < sizeof(entries); i++)
      if (entries[i][1] == -2)   // size -2 marks a directory
        queue += ({ dir + entries[i][0] + "/" });
  }

  area_paths[game] = found;
  return found;
}

// will create an area storage in the destination directory
// (and create such directory if it doesn't exist)
object create_area(string path)
{
  object area;
  string game;

  // normalise to a single trailing slash so create_area(".../rooms") and
  // create_area(".../rooms/") key the same cached area (and never write a
  // stray "roomsarea.o" from a slashless path).
  if (strlen(path) && path[strlen(path) - 1] != '/')
    path += "/";

  if (file_size(path) != -2)
    mkdir(path);

  if (loaded_areas[path])
    return loaded_areas[path];

  area = clone_object(AREA_STORAGE_OBJECT);
  area->set_area_path(path);

  // will try to load the .o if it exists
  if (!area->restore_from_file_name(path + "area.o"))
  {
    // this will also save the area file
    area->set_file_name(path + "area.o");
  }

  loaded_areas[path] = area;

  // a brand-new area belongs to the list of what exists, without re-walking it
  game = game_from_path(path);
  if (strlen(game) && area_paths[game] &&
      member_array(path, area_paths[game]) == -1)
    area_paths[game] += ({ path });

  return area;
}

// Look up an already-known area without creating anything. Returns the
// cached area if it is loaded, otherwise restores it from its area.o when
// that file exists on disk (and caches it), or nil when there is no such
// area. Unlike create_area this never mkdirs the path nor writes a fresh
// area.o -- use it for read-only lookups (death cleanup, queries) where a
// missing area must not be brought into existence.
object query_area(string path)
{
  object area;

  if (strlen(path) && path[strlen(path) - 1] != '/')
    path += "/";

  if (loaded_areas[path])
    return loaded_areas[path];

  if (file_size(path + "area.o") < 0)
    return nil;

  area = clone_object(AREA_STORAGE_OBJECT);
  area->set_area_path(path);
  area->restore_from_file_name(path + "area.o");
  loaded_areas[path] = area;

  return area;
}

// Cron calls this once per game hour (see the crontab, after the weather line so
// the hour is already advanced). For every loaded area it reads that area's game
// hour and collects the census uuids with something scheduled this hour, then
// releases them staggered. Each is woken as released, so unloaded scheduled NPCs
// are materialised at their census position and act just like loaded ones.
void update_areas()
{
  string * games, * paths, * uuids;
  object area;
  int g, p, hour, j;

  // Every area of every game, not only the ones somebody happens to be standing
  // in: a town whose people stop going to work because no player is watching is
  // a town that only exists while it is looked at. An area object is small and
  // query_area caches it, so the whole world costs one restore each and then
  // nothing.
  games = (string *)handler("games")->query_games();

  for (g = 0; g < sizeof(games); g++)
  {
    paths = query_area_paths(games[g]);

    for (p = 0; p < sizeof(paths); p++)
    {
      area = query_area(paths[p]);
      if (!area)
        continue;

      hour = (int)area->query_game_hour();
      uuids = area->hour_actor_uuids(hour);
      for (j = 0; j < sizeof(uuids); j++)
        pending_schedule += ({ ({ area, uuids[j], hour }) });
    }
  }

  if (sizeof(pending_schedule) && find_call_out("_dispatch_schedule") == -1)
    call_out("_dispatch_schedule", 0);
}

// Release a few scheduled actions per tick and re-arm until the queue drains, so
// departures (and the loads that waking unloaded NPCs triggers) trickle out over
// ~seconds rather than all on one beat.
void _dispatch_schedule()
{
  int i;

  // a handful per tick keeps the stagger visible without dragging on
  for (i = 0; i < 3 && sizeof(pending_schedule); i++)
  {
    mixed * item;
    item = pending_schedule[0];
    pending_schedule = pending_schedule[1..];
    if (item[0])
      item[0]->wake_and_schedule(item[1], item[2]);
  }

  if (sizeof(pending_schedule))
    call_out("_dispatch_schedule", 1 + random(2));
}

// Delete an area whose locations are all gone: remove its area.o file,
// forget it, and drop the now-empty directory, so a wiped area leaves no
// dead area.o behind. Returns 1 if it was removed, 0 otherwise.
int remove_area_if_empty(object area)
{
  string area_file, path, game;

  if (!area || map_sizeof(area->query_locations()))
    return 0;

  area_file = area->query_file_name();
  path = area->query_area_path();

  if (area_file && file_size(area_file) >= 0)
    remove_file(area_file);

  if (path)
  {
    map_delete(loaded_areas, path);

    // and out of the list of what exists, which create_area adds to
    game = game_from_path(path);
    if (strlen(game) && area_paths[game])
      area_paths[game] -= ({ path });
  }
  destruct(area);

  if (path && file_size(path) == -2 && !sizeof(get_dir(path + "*")))
    rmdir(path);

  return 1;
}

// Delete an NPC's save folder and everything in it: every file, then the folder
// itself. Takes the folder path (the NPC object is not loaded here). Callers
// pass a folder under the npcs save tree -- npc_save_dir() for a known uuid, or
// the exact folder found by a scan.
private void _delete_npc_folder(string dir)
{
  string * files;
  int i;

  if (file_size(dir) != -2)
    return;

  files = (string *)get_dir(dir + "*");
  for (i = 0; i < sizeof(files); i++)
    catch(remove_file(dir + files[i]));
  catch(rmdir(dir));
}

// Delete the save folders of every NPC in this area's census. Called when the
// area is being removed (a clean) so its NPCs leave no orphan folder behind.
// Scoped to the area -- O(area's NPCs), unlike a game-wide verify. Returns the
// number of folders removed.
int prune_area_npc_saves(object area)
{
  string game;
  string * uuids;
  int i;

  if (!area)
    return 0;
  game = game_from_path(area->query_area_path());
  if (!game)
    return 0;

  uuids = map_indices(area->query_npc_census());
  for (i = 0; i < sizeof(uuids); i++)
    _delete_npc_folder(npc_save_dir(game, uuids[i]));

  return sizeof(uuids);
}

// --- npcs verify: a chunked, background audit of NPC save folders -----------
//
// Every persisted NPC lives in /save/games/<game>/npcs/<letter>/<uuid>/, but
// only those whose uuid is in some area's census are still real. The rest are
// orphans left when a census entry was dropped without the NPC dying (a
// reconversion, a census rebuild, a removed vacancy). This can span thousands
// of areas and folders (several imported muds), so it runs across ticks via
// call_out rather than one synchronous pass -- first collecting census uuids,
// then scanning the save folders -- and reports to the initiator when done.
//
// Areas loaded only to read their census are dropped again afterwards, so a
// verify never leaves hundreds of areas cached in loaded_areas.

#define NPC_VERIFY_COLLECT_CHUNK 20   // area dirs visited per tick
#define NPC_VERIFY_SCAN_CHUNK   100   // uuid folders checked per tick

// call_out target: one tick of the verify state machine (see verify_npc_saves).
// Public because the call_out dispatcher reaches it through call_other.
void _npc_verify_step(mapping st)
{
  int n;

  // Phase 1: walk the area tree (BFS, so the walk itself is chunked, not just
  // the loads), collecting every census uuid into st["referenced"].
  if (st["phase"] == "collect")
  {
    n = 0;
    while (n < NPC_VERIFY_COLLECT_CHUNK && sizeof(st["queue"]))
    {
      string dir;
      mixed * entries;
      int i;

      dir = st["queue"][0];
      st["queue"] = st["queue"][1 ..];
      n++;

      if (file_size(dir + "area.o") >= 0)
      {
        int fresh;
        object area;

        // don't keep an area cached just because verify touched it
        fresh = !loaded_areas[dir];
        area = query_area(dir);
        if (area)
        {
          string * uuids;
          uuids = map_indices(area->query_npc_census());
          for (i = 0; i < sizeof(uuids); i++)
            st["referenced"][uuids[i]] = 1;
          if (fresh)
          {
            map_delete(loaded_areas, area->query_area_path());
            destruct(area);
          }
        }
      }

      entries = get_dir(dir + "*", -1);
      for (i = 0; i < sizeof(entries); i++)
        if (entries[i][1] == -2)   // size -2 marks a directory
          st["queue"] += ({ dir + entries[i][0] + "/" });
    }

    if (!sizeof(st["queue"]))
    {
      st["letters"] = get_dir(st["npcbase"] + "*");
      st["phase"] = "scan";
    }
    call_out("_npc_verify_step", 0, st);
    return;
  }

  // Phase 2: scan the npc save folders shard by shard, classifying each uuid
  // folder against the collected census and (on apply) deleting the orphans.
  n = 0;
  while (n < NPC_VERIFY_SCAN_CHUNK)
  {
    string uuid, udir;
    string * files;

    if (!sizeof(st["uuids"]))
    {
      if (!sizeof(st["letters"]))
        break;
      st["letter"] = st["letters"][0];
      st["letters"] = st["letters"][1 ..];
      // templates live under npcs/templates/, not a uuid shard
      if (st["letter"] == "templates" ||
          file_size(st["npcbase"] + st["letter"] + "/") != -2)
        continue;
      st["uuids"] = get_dir(st["npcbase"] + st["letter"] + "/*");
      continue;
    }

    uuid = st["uuids"][0];
    st["uuids"] = st["uuids"][1 ..];
    n++;

    udir = st["npcbase"] + st["letter"] + "/" + uuid + "/";
    if (file_size(udir) != -2 || st["referenced"][uuid])
      continue;

    files = (string *)get_dir(udir + "*");
    if (sizeof(files))
      st["orphans"] = st["orphans"] + 1;
    else
      st["empty"] = st["empty"] + 1;

    // delete the folder exactly where it was found, not a recomputed shard
    // path, so a misplaced folder is still removed
    if (st["apply"])
      _delete_npc_folder(udir);
  }

  if (sizeof(st["uuids"]) || sizeof(st["letters"]))
  {
    call_out("_npc_verify_step", 0, st);
    return;
  }

  // done -- report to whoever asked
  if (st["initiator"])
  {
    int total;
    string msg;

    total = st["orphans"] + st["empty"];
    msg = "NPC save verify for '" + st["game"] + "':\n" +
          "  orphan npcs folders (a save with no census entry): " +
          st["orphans"] + "\n" +
          "  empty npcs folders (save already deleted): " + st["empty"] + "\n";
    if (st["apply"])
      msg += "  -> deleted " + total + " folder" + (total == 1 ? "" : "s") +
             ".\n";
    else if (total)
      msg += "  run 'npcs verify apply' to delete them.\n";
    else
      msg += "  nothing to clean.\n";
    tell_object(st["initiator"], msg);
  }
}

// Start a chunked, background verify of `game`'s NPC save folders (see
// _npc_verify_step). Reports to `initiator` when it finishes; `apply` deletes
// the orphans it finds.
void verify_npc_saves(string game, int apply, object initiator)
{
  mapping st;

  st = ([
    "game":       game,
    "apply":      apply,
    "initiator":  initiator,
    "phase":      "collect",
    "queue":      ({ "/save/games/" + game + "/locations/areas/" }),
    "referenced": ([ ]),
    "npcbase":    "/save/games/" + game + "/npcs/",
    "letters":    ({ }),
    "letter":     "",
    "uuids":      ({ }),
    "orphans":    0,
    "empty":      0,
  ]);

  call_out("_npc_verify_step", 0, st);
}

string add_location(object location)
{
  string file_name, dir_name;
  object area_storage;

  file_name = location->query_file_name();
  dir_name = "/" + implode(shift_right(explode(file_name, "/")), "/") + "/";

  // create the area storage if it doesn't exist, will return an already
  // created area if it is stored in the loaded_areas mapping
  area_storage = create_area(dir_name);

  // add the location to the area storage
  area_storage->add_location(location->query_file_name(), ([ ]));
  area_storage->add_loaded_location(location);

  // write a file in path + file_name with the location data
  remove_file(file_name);
  write_file(file_name, location->query_file_name());

  return dir_name + file_name;
}
