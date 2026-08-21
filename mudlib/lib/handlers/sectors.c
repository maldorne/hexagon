
#include <sector/sector.h>
#include <room/location.h>
#include <room/room.h>

/*
 * On-disk map index. Each occupied 10x10x10 sector is a directory under
 * /save/games/<game>/maps/<map>/<sx>/<sy>/<sz>/ holding two things:
 *   - sector.o      the index: positions ("x_y_z" -> location file), the
 *                   terrain-type tallies and the per-coordinate road/path ways.
 *   - <x>_<y>_<z>.o one pointer file per occupied coordinate, naming the
 *                   location that sits there.
 *
 * Invariant: every positions entry has a matching pointer file for the same
 * location, and a location is indexed at exactly one coordinate. add_location
 * writes both sides; remove_location_from_map and sector::remove_position drop
 * both; the per-location indexed-coord marker (purged when coordinates change)
 * and `sectors purge` enforce the "exactly one" clause.
 */

mapping loaded_sectors;

// prototypes
int remove_location_from_map(string location_file_name, string map_name,
                             int x, int y, int z);

void create() {
  loaded_sectors = ([ ]);
  // ::create();
}

mapping query_loaded_sectors() {
  return loaded_sectors;
}

// will create an sector storage in the destination directory
// (and create such directory if it doesn't exist)
object create_sector(string path)
{
  object sector;

  // mkdir efun (lib/core/efuns/file.c) creates every missing level along
  // the path, so a virgin sector deep in maps/<map>/<x>/<y>/<z>/ works.
  mkdir(path);

  if (loaded_sectors[path])
    return loaded_sectors[path];

  sector = clone_object(SECTOR_STORAGE_OBJECT);

  // will try to load the .o if it exists
  if (!sector->restore_from_file_name(path + "sector.o"))
  {
    sector->set_file_name(path + "sector.o");
  }

  loaded_sectors[path] = sector;

  return sector;
}

// The sector object owning world coord (x, y, z) in game/map, or nil if that
// part of the map was never indexed. The fine pathfinder walks the node/edge
// graph across sector boundaries and needs to fetch an arbitrary coordinate's
// sector on demand. Unlike create_sector it does not materialise a virgin
// sector on disk: an already-loaded one is returned from cache, an existing
// one is loaded, and a never-written coordinate returns nil (so probing a
// dead-end edge leaves no empty directories behind).
object query_sector_for_coord(string game_slug, string map_name,
                              int x, int y, int z)
{
  int sector_x, sector_y, sector_z;
  string path;

  sector_x = x / 10 - (x < 0);
  sector_y = y / 10 - (y < 0);
  sector_z = z / 10 - (z < 0);

  path = "/save/games/" + game_slug + "/maps/" + map_name + "/" +
         sector_x + "/" + sector_y + "/" + sector_z + "/";

  if (!loaded_sectors[path] && file_size(path + "sector.o") < 0)
    return nil;

  return create_sector(path);
}

// Is this location already indexed at its own coordinate? A pure O(1) check --
// the location's coordinate, the sector that owns it (cached), one node lookup --
// with no scanning. Callers use it to skip a redundant add_location (each of
// which rewrites the sector .o) when nothing would change: the location is
// already the node at that coordinate. Returns 0 when the location has no
// coordinate, its sector was never written, or a different file holds the node
// (moved / never indexed / coordinate clash) -- all cases where add_location
// must run.
int is_location_indexed(object location)
{
  int x, y, z;
  object sector;
  mapping node;

  if (!location || location->query_coordinates() == nil)
    return 0;

  x = location->query_coordinates()[0];
  y = location->query_coordinates()[1];
  z = location->query_coordinates()[2];

  sector = query_sector_for_coord(
             game_from_path(location->query_file_name()),
             location->query_map_name(), x, y, z);
  if (!sector)
    return 0;

  node = sector->query_nodes()["" + x + "_" + y + "_" + z];
  return (node && node["file"] == location->query_file_name()) ? 1 : 0;
}

// Set (or clear, with SECTOR_TYPE_NONE) the manual type of the sector
// containing world coord (x, y, z) in the given game/map. Creates the
// sector.o if it does not exist yet, so a programmer can paint a type on
// a virgin part of the map. Returns the sector object.
// (sector_x, sector_y, sector_z) are sector indices, not world coordinates:
// the caller has already divided world coords into sectors (or is naming a
// sector directly). Creates the sector.o if it does not exist yet, so a
// virgin part of the map can be painted.
object set_sector_manual_type(string game, string map_name,
                              int sector_x, int sector_y, int sector_z,
                              string type)
{
  string path;
  object sector;

  path = "/save/games/" + game + "/maps/" + map_name + "/" +
         sector_x + "/" + sector_y + "/" + sector_z + "/";

  sector = create_sector(path);
  if (sector)
    sector->set_manual_type(type);

  return sector;
}

// Cartesian delta of a one-step move in a canonical exit direction, matching
// the world coordinate convention, which is fixed for every game and map:
// +x is east, +y is north, +z is up, and a diagonal composes its two axes.
// A renderer may flip an axis on the way out (an ASCII map usually draws y
// growing downward on screen); the stored coordinates never change.
// Returns nil for non-directional exits (enter, out, ...), which have no place
// on the coordinate grid.
private int * _dir_delta(string dir)
{
  switch (dir)
  {
    case SECTOR_DIR_NORTH:     return ({  0,  1,  0 });
    case SECTOR_DIR_SOUTH:     return ({  0, -1,  0 });
    case SECTOR_DIR_EAST:      return ({  1,  0,  0 });
    case SECTOR_DIR_WEST:      return ({ -1,  0,  0 });
    case SECTOR_DIR_NORTHEAST: return ({  1,  1,  0 });
    case SECTOR_DIR_NORTHWEST: return ({ -1,  1,  0 });
    case SECTOR_DIR_SOUTHEAST: return ({  1, -1,  0 });
    case SECTOR_DIR_SOUTHWEST: return ({ -1, -1,  0 });
    case "up":                 return ({  0,  0,  1 });
    case "down":               return ({  0,  0, -1 });
  }
  return nil;
}

// Public wrapper over _dir_delta: the unit coordinate step for a canonical
// direction ("north" -> ({0,1,0})), or nil for a non-directional exit. Callers
// that need the neighbouring coordinate in a direction (the builder-ring plot
// command) use this instead of duplicating the table.
int * query_dir_delta(string canonical_dir)
{
  return _dir_delta(canonical_dir);
}

string add_location(object location)
{
  int x, y, z;
  int sector_x, sector_y, sector_z;
  object sector_storage;
  string path, file_name, content;

  if (!location)
    return nil;

  if (location->query_coordinates() == nil)
    return nil;

  x = location->query_coordinates()[0];
  y = location->query_coordinates()[1];
  z = location->query_coordinates()[2];

  // every sector could store 10x10x10 locations. The -(n < 0) term is what
  // keeps negative coordinates in the sector a reader expects: integer division
  // truncates toward zero, so -1 / 10 is 0, and without the correction the
  // whole strip from -9 to 9 would collapse into sector 0.
  sector_x = x / 10 - (x < 0);
  sector_y = y / 10 - (y < 0);
  sector_z = z / 10 - (z < 0);

  // Ghost prevention: if this location is already indexed at a different
  // coordinate (its coords were re-guessed on a reload, or it was moved),
  // drop that stale entry before writing the new one. Without this a
  // reload that shifts coordinates leaves the old sector pointing at a
  // location that no longer sits there. See location.c::_map_indexed_coord.
  {
    int * prev;
    prev = location->query_map_indexed_coord();
    if (prev && sizeof(prev) == 3 &&
        (prev[0] != x || prev[1] != y || prev[2] != z))
      remove_location_from_map(location->query_file_name(),
                               location->query_map_name(),
                               prev[0], prev[1], prev[2]);
  }

  path = "/save/games/" + game_from_path(location->query_file_name()) +
         "/maps/" + location->query_map_name() + "/" + 
         sector_x + "/" + sector_y + "/" + sector_z + "/";
  file_name = "" + x + "_" + y + "_" + z + ".o";

  sector_storage = create_sector(path);

  // Lift a "maze" flag and the set of cartography-contributing
  // components from the location's component set so the sector can
  // index each coord for pathfinding and classify itself for the
  // world map renderer, without callers having to reload the location
  // afterwards. See include/maps/sector.h for the type taxonomy.
  {
    mapping location_data;
    string * contrib, * my_types;
    int i;

    location_data = ([ ]);
    if (location->query_component_by_type(LOCATION_COMPONENT_MAZE))
      location_data["maze"] = 1;

    contrib = SECTOR_MAP_COMPONENTS;
    my_types = ({ });
    for (i = 0; i < sizeof(contrib); i++)
      if (location->query_component_by_type(contrib[i]))
        my_types += ({ contrib[i] });
    if (sizeof(my_types))
      location_data["types"] = my_types;

    // Lift the location's path / road exits (direction -> way_type) so
    // the sector can answer border-crossing and pathfinding queries
    // without reloading the location. Only the cartographic exit types
    // (SECTOR_WAY_TYPES) are recorded; doors, stairs, etc are ignored.
    {
      mapping exit_map, ways;
      string * dirs, * way_types;

      exit_map = location->query_exit_map();
      ways = ([ ]);
      way_types = SECTOR_WAY_TYPES;

      if (mappingp(exit_map))
      {
        dirs = map_indices(exit_map);
        for (i = 0; i < sizeof(dirs); i++)
        {
          string type;
          // exit_map[dir] = ({ dest, type, material, ... })
          if (!arrayp(exit_map[dirs[i]]) || sizeof(exit_map[dirs[i]]) < 2)
            continue;
          type = exit_map[dirs[i]][1];
          // store the direction in canonical English so sector.o stays
          // language-agnostic like the rest of /save (see
          // room_handler::canonical_dir).
          if (member_array(type, way_types) != -1)
            ways[ROOM_HAND->canonical_dir(dirs[i])] = type;
        }
      }

      if (map_sizeof(ways))
        location_data["ways"] = ways;
    }

    // Lift the movement graph: every traversable exit (any direction, not just
    // road/path) as direction -> the neighbouring coordinate it leads to (one
    // step in that direction). This full edge set (location_data["edges"]) is
    // what the fine pathfinder walks by direct lookup. The subset of those
    // edges whose destination lands in a neighbouring sector is also recorded
    // as location_data["boundary"] -- the sector's A* "ports", read by the
    // coarse pathfinder (sector to sector) and to stitch a path across a border.
    {
      mapping exit_map, edge_map;
      string * dirs, * boundary;

      exit_map = location->query_exit_map();
      edge_map = ([ ]);
      boundary = ({ });

      if (mappingp(exit_map))
      {
        dirs = map_indices(exit_map);
        for (i = 0; i < sizeof(dirs); i++)
        {
          int * d;
          int tx, ty, tz;
          string cdir, to;

          cdir = ROOM_HAND->canonical_dir(dirs[i]);
          d = _dir_delta(cdir);
          if (!d)
            continue;
          tx = x + d[0];
          ty = y + d[1];
          tz = z + d[2];
          to = "" + tx + "_" + ty + "_" + tz;

          edge_map[cdir] = to;

          if (tx / 10 - (tx < 0) != sector_x ||
              ty / 10 - (ty < 0) != sector_y ||
              tz / 10 - (tz < 0) != sector_z)
            boundary += ({ to });
        }
      }

      if (map_sizeof(edge_map))
        location_data["edges"] = edge_map;
      if (sizeof(boundary))
        location_data["boundary"] = boundary;
    }

    sector_storage->add_location(location->query_file_name(),
                                 x, y, z, location_data);
  }

  // keep the per-coordinate pointer file in sync with the location's
  // own file_name. Three cases:
  //   - file missing      → create it (first time this coord is used)
  //   - content matches   → no-op
  //   - content differs   → rewrite
  content = read_file(path + file_name);

  if (!content)
    write_file(path + file_name, location->query_file_name());
  else if (content != location->query_file_name())
  {
    // Two distinct locations claim the same (x, y, z). This is a
    // data bug on the room side — the sector can only remember one
    // pointer per coord, so the loser is dropped from coord lookup
    // (still loadable directly, but the map won't find it). Emit a
    // persistent trace so authors can find and fix the source.
    string msg;
    msg = "[" + ctime(time(), 4) + "] map=" + location->query_map_name() +
          " coord=(" + x + "," + y + "," + z + ") collision: " +
          content + " ↔ " + location->query_file_name() + "\n";
    log_file("sectors_collision", msg);
    stderr("⚠️  maps: " + msg);

    remove_file(path + file_name);
    write_file(path + file_name, location->query_file_name());
  }

  // remember where we just indexed it, so a later coordinate change can
  // find and purge this entry (see the ghost-prevention block above).
  location->set_map_indexed_coord(({ x, y, z }));

  return path + file_name;
}

/**
 * Remove a location from the map index: drops its entry from the
 * sector.o, deletes the per-coordinate pointer file, and forgets the
 * sector if it ends up empty.
 *
 * Called by the location-cleanup pipeline (see
 * LOCATION_HANDLER->clean_apply). The location object itself is no
 * longer needed at this point; the caller passes its coordinates and
 * map name explicitly so this can run even after the location has been
 * destructed.
 */
int remove_location_from_map(string location_file_name, string map_name,
                             int x, int y, int z)
{
  int sector_x, sector_y, sector_z;
  string sector_path, coord_file;
  object sector;

  sector_x = x / 10 - (x < 0);
  sector_y = y / 10 - (y < 0);
  sector_z = z / 10 - (z < 0);

  sector_path = "/save/games/" + game_from_path(location_file_name) +
                "/maps/" + map_name + "/" +
                sector_x + "/" + sector_y + "/" + sector_z + "/";
  coord_file = sector_path + x + "_" + y + "_" + z + ".o";

  // remove per-coordinate pointer file
  if (file_size(coord_file) >= 0)
    remove_file(coord_file);

  // ensure the sector is loaded and tell it to drop the entry
  sector = create_sector(sector_path);
  if (sector)
  {
    sector->remove_location(location_file_name);

    // a sector with no locations left is not deleted: it keeps its file
    // and folder so a programmer can still paint it a type. Mark it
    // "empty" (unless one is already set) so it renders as blank until
    // then. See sector.c::query_sector_type.
    if (!map_sizeof(sector->query_positions()) &&
        !strlen(sector->query_manual_type()))
      sector->set_manual_type(SECTOR_TYPE_EMPTY);
  }

  return 1;
}

// Ask the location system for a location's real coordinates. Going through
// LOCATION_HANDLER is the sanctioned path — it restores the location with the
// right permissions; the sector layer must not read location files itself.
// Returns ({ x, y, z }), or ({ }) when the location no longer loads. Memoised
// so a location referenced from several sectors is resolved once.
private int * _location_coords(string file, mapping cache)
{
  object loc;
  int * coords;

  if (!undefinedp(cache[file]))
    return cache[file];

  loc = load_object(LOCATION_HANDLER)->load_location(file);
  coords = loc ? loc->query_coordinates() : nil;
  cache[file] = (coords && sizeof(coords) == 3) ? coords : ({ });

  return cache[file];
}

/**
 * Sweep every sector.o under a map and drop "ghost" position entries: a
 * coordinate that points at a location whose real coordinates sit elsewhere
 * (or at a location that no longer loads). These are left behind by reloads
 * that re-guessed coordinates before add_location's ghost-prevention existed.
 *
 * Only the stale coordinate is removed, so a location that legitimately keeps
 * another entry in the same sector is untouched (see sector.c::remove_position).
 * A sector emptied by the sweep and not manually painted is marked "empty".
 * Returns the number of stale entries removed.
 */
int purge_drift(string game, string map_name)
{
  string base;
  string * xs;
  mapping coord_cache;
  int removed, xi, yi, zi;

  base = "/save/games/" + game + "/maps/" + map_name;
  coord_cache = ([ ]);
  removed = 0;

  xs = get_files(base + "/*");
  for (xi = 0; xi < sizeof(xs); xi++)
  {
    string * ys;

    if (file_size(xs[xi]) != -2)
      continue;

    ys = get_files(xs[xi] + "/*");
    for (yi = 0; yi < sizeof(ys); yi++)
    {
      string * zs;

      if (file_size(ys[yi]) != -2)
        continue;

      zs = get_files(ys[yi] + "/*");
      for (zi = 0; zi < sizeof(zs); zi++)
      {
        string sector_path;
        object sector;
        mapping positions;
        string * keys;
        int ki;

        if (file_size(zs[zi]) != -2)
          continue;

        sector_path = zs[zi] + "/";
        if (file_size(sector_path + "sector.o") < 0)
          continue;

        sector = create_sector(sector_path);
        positions = sector->query_positions();
        keys = map_indices(positions);

        for (ki = 0; ki < sizeof(keys); ki++)
        {
          string file, key;
          int kx, ky, kz;
          int * rc;

          key = keys[ki];
          file = positions[key];
          if (sscanf(key, "%d_%d_%d", kx, ky, kz) != 3)
            continue;

          rc = _location_coords(file, coord_cache);

          if (sizeof(rc) != 3 || rc[0] != kx || rc[1] != ky || rc[2] != kz)
          {
            remove_file(sector_path + key + ".o");
            sector->remove_position(key);
            removed++;
          }
        }

        // orphan pointer files: <k>.o on disk with no index entry (never
        // added, or left behind by an older writer). Nothing reads them from
        // the aggregate index, but direct coordinate access would; drop them.
        {
          string * disk_files;
          mapping live;
          int fi;

          live = sector->query_positions();
          disk_files = get_files(sector_path + "*_*_*.o");
          for (fi = 0; fi < sizeof(disk_files); fi++)
          {
            string base;
            base = disk_files[fi];
            if (strlen(base) < 2 || base[strlen(base) - 2 ..] != ".o")
              continue;
            base = base[strlen(sector_path) .. strlen(base) - 3];
            if (undefinedp(live[base]))
            {
              remove_file(disk_files[fi]);
              removed++;
            }
          }
        }

        if (!map_sizeof(sector->query_positions()) &&
            !strlen(sector->query_manual_type()))
          sector->set_manual_type(SECTOR_TYPE_EMPTY);
      }
    }
  }

  return removed;
}

/**
 * Read-only audit of a single sector — the counterpart of purge_drift scoped
 * to one sector, for the `sectors verify` command. Flags stale "ghost"
 * positions (a coordinate whose location has moved elsewhere or no longer
 * loads), pointer files that disagree with the index, and orphan pointer
 * files on disk with no index entry. Changes nothing.
 *
 * Returns nil when the sector has no sector.o, otherwise:
 *   ([ "total"   : int,                                positions in the index
 *      "stale"   : ({ ({ coord_key, file, reason }), ... }),
 *      "orphans" : ({ coord_key, ... }) ])             pointer files, no entry
 * reason is one of "moved", "unloadable", "no_pointer", "pointer_mismatch".
 */
mapping verify_sector(string game, string map_name, int sx, int sy, int sz)
{
  string sector_path;
  object sector;
  mapping positions, coord_cache;
  string * keys, * disk_files, * orphans;
  mixed * stale;
  int i;

  sector_path = "/save/games/" + game + "/maps/" + map_name + "/" +
                sx + "/" + sy + "/" + sz + "/";
  if (file_size(sector_path + "sector.o") < 0)
    return nil;

  sector = create_sector(sector_path);
  positions = sector->query_positions();
  keys = map_indices(positions);
  coord_cache = ([ ]);
  stale = ({ });

  for (i = 0; i < sizeof(keys); i++)
  {
    string file, key, reason;
    int kx, ky, kz;
    int * rc;

    key = keys[i];
    file = positions[key];
    reason = "";

    if (sscanf(key, "%d_%d_%d", kx, ky, kz) != 3)
      continue;

    rc = _location_coords(file, coord_cache);
    if (sizeof(rc) != 3)
      reason = "unloadable";
    else if (rc[0] != kx || rc[1] != ky || rc[2] != kz)
      reason = "moved";
    else if (file_size(sector_path + key + ".o") < 0)
      reason = "no_pointer";
    else if (read_file(sector_path + key + ".o") != file)
      reason = "pointer_mismatch";

    if (strlen(reason))
      stale += ({ ({ key, file, reason }) });
  }

  // orphan pointer files: <k>.o on disk with no matching positions entry
  orphans = ({ });
  disk_files = get_files(sector_path + "*_*_*.o");
  for (i = 0; i < sizeof(disk_files); i++)
  {
    string base;
    base = disk_files[i];
    if (strlen(base) < 2 || base[strlen(base) - 2 ..] != ".o")
      continue;
    base = base[strlen(sector_path) .. strlen(base) - 3];
    if (undefinedp(positions[base]))
      orphans += ({ base });
  }

  return ([ "total": sizeof(keys), "stale": stale, "orphans": orphans ]);
}
