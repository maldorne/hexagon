
#include <maps/maps.h>
#include <maps/sector.h>
#include <room/location.h>

mapping loaded_sectors;

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

  if (file_size(path) != -2)
    mkdir(path);

  if (loaded_sectors[path])
    return loaded_sectors[path];

  sector = clone_object(MAP_SECTOR_STORAGE_OBJECT);

  // will try to load the .o if it exists
  if (!sector->restore_from_file_name(path + "sector.o"))
  {
    sector->set_file_name(path + "sector.o");
  }

  loaded_sectors[path] = sector;

  return sector;
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

  // every sector could store 10x10x10 locations
  sector_x = x / 10 - (x < 0);
  sector_y = y / 10 - (y < 0);
  sector_z = z / 10 - (z < 0);

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

    contrib = SECTOR_CONTRIB_COMPONENTS;
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
          if (member_array(type, way_types) != -1)
            ways[dirs[i]] = type;
        }
      }

      if (map_sizeof(ways))
        location_data["ways"] = ways;
    }

    sector_storage->add_location(location->query_file_name(),
                                 x, y, z, location_data);
  }
  sector_storage->add_loaded_location(location);

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
    log_file("maps_collision", msg);
    stderr("⚠️  maps: " + msg);

    remove_file(path + file_name);
    write_file(path + file_name, location->query_file_name());
  }

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
    sector->remove_location(location_file_name);

  return 1;
}
