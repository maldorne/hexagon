inherit "/lib/core/object.c";

// mapping in the form ([ file_name : location_data ])
mapping locations;
// mapping in the form ([ x_y_z : file_name ])
mapping positions;
// mapping in the form ([ x_y_z : 1 ]) for coords whose location is a
// maze — kept parallel to positions so consumers can tag / skip
// entries without a full location load.
mapping maze_positions;
// array of loaded locations
static object * loaded_locations;
string file_name;

void create() {
  locations = ([ ]);
  positions = ([ ]);
  maze_positions = ([ ]);
  loaded_locations = ({ });
  ::create();
}

void restore_me() {
  restore_object(file_name);
}

void save_me() {
  save_object(file_name);
}

mapping query_positions() { return positions; }
mapping query_locations() { return locations; }
mapping query_maze_positions()
{
  if (!maze_positions) maze_positions = ([ ]);
  return maze_positions;
}

int is_maze_at(int x, int y, int z)
{
  if (!maze_positions) return 0;
  return maze_positions["" + x + "_" + y + "_" + z] ? 1 : 0;
}

string query_file_name() { return file_name; }
void set_file_name(string name)
{
  file_name = name;
  save_me();
}

// restore the sector from a file
// returns 1 if the file exists and the location was restored, 0 otherwise
int restore_from_file_name(string name)
{
  if (file_size(name) >= 0)
  {
    restore_object(name);
    return 1;
  }

  return 0;
}

void add_location(string location_file_name, int x, int y, int z, mapping location_data)
{
  string key;

  if (!maze_positions) maze_positions = ([ ]);

  key = "" + x + "_" + y + "_" + z;
  locations[location_file_name] = map_copy(location_data);
  positions[key] = location_file_name;

  // location_data may carry a "maze" flag lifted from the location's
  // component set — record it so pathfinding can filter without
  // touching the location object.
  if (location_data && location_data["maze"])
    maze_positions[key] = 1;
  else
    map_delete(maze_positions, key);

  save_me();
}

void remove_location(string location_file_name)
{
  string * pos_keys;
  int i;

  if (!maze_positions) maze_positions = ([ ]);

  map_delete(locations, location_file_name);

  // also strip the coords -> file_name reverse mapping
  pos_keys = map_indices(positions);
  for (i = 0; i < sizeof(pos_keys); i++)
  {
    if (positions[pos_keys[i]] == location_file_name)
    {
      map_delete(positions, pos_keys[i]);
      map_delete(maze_positions, pos_keys[i]);
    }
  }

  save_me();
}

void add_loaded_location(object location) 
{
  int i;

  loaded_locations -= ({ nil });

  for (i = 0; i < sizeof(loaded_locations); i++)
  {
    // why are we having two objects for the same location?
    // don't know, don't care, the last one should be the good one
    if (loaded_locations[i]->query_file_name() == location->query_file_name())
    {
      loaded_locations[i] = location;
      return;
    }
  }

  loaded_locations += ({ location });
}

object * query_loaded_locations() 
{
  loaded_locations -= ({ nil });
  return loaded_locations;
}
