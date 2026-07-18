#include <maps/sectors.h>

inherit "/lib/core/object.c";

// mapping in the form ([ file_name : location_data ])
mapping locations;
// mapping in the form ([ x_y_z : file_name ])
mapping positions;
// mapping in the form ([ x_y_z : 1 ]) for coords whose location is a
// maze — kept parallel to positions so consumers can tag / skip
// entries without a full location load.
mapping maze_positions;
// mapping in the form ([ sector_type : count ]) — how many locations
// in this sector carry each cartography component. Populated by
// add_location() and decremented by remove_location(); consumers read
// via query_sector_type() (dominant type) or query_type_counts() (raw
// distribution). See include/maps/sectors.h for the taxonomy.
mapping type_counts;
// array of loaded locations
static object * loaded_locations;
string file_name;

void create() {
  locations = ([ ]);
  positions = ([ ]);
  maze_positions = ([ ]);
  type_counts = ([ ]);
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
  mapping previous;
  string * old_types, * new_types;
  int i;

  if (!maze_positions) maze_positions = ([ ]);
  if (!type_counts) type_counts = ([ ]);

  key = "" + x + "_" + y + "_" + z;
  previous = locations[location_file_name];
  locations[location_file_name] = map_copy(location_data);
  positions[key] = location_file_name;

  // location_data may carry a "maze" flag lifted from the location's
  // component set — record it so pathfinding can filter without
  // touching the location object.
  if (location_data && location_data["maze"])
    maze_positions[key] = 1;
  else
    map_delete(maze_positions, key);

  // Update per-type counters. A re-add for the same file is treated as
  // an update: decrement the previous contribution before adding the
  // new one so tallies stay accurate under recompiles / re-registration.
  old_types = (previous && previous["types"]) ? previous["types"] : ({ });
  for (i = 0; i < sizeof(old_types); i++)
    if (type_counts[old_types[i]] > 0)
      type_counts[old_types[i]]--;

  new_types = (location_data && location_data["types"]) ?
              location_data["types"] : ({ });
  for (i = 0; i < sizeof(new_types); i++)
    type_counts[new_types[i]] = type_counts[new_types[i]] + 1;

  save_me();
}

void remove_location(string location_file_name)
{
  string * pos_keys;
  mapping previous;
  string * old_types;
  int i;

  if (!maze_positions) maze_positions = ([ ]);
  if (!type_counts) type_counts = ([ ]);

  // Roll back this file's contribution to the sector-type tally BEFORE
  // dropping its entry from `locations`, since that entry is where the
  // type list lives.
  previous = locations[location_file_name];
  old_types = (previous && previous["types"]) ? previous["types"] : ({ });
  for (i = 0; i < sizeof(old_types); i++)
    if (type_counts[old_types[i]] > 0)
      type_counts[old_types[i]]--;

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

mapping query_type_counts()
{
  if (!type_counts) type_counts = ([ ]);
  return type_counts;
}

// The sector's dominant type: whichever cartography component is
// attached to the most locations here. Iterates SECTOR_CONTRIB_COMPONENTS
// in priority order with a strict `>`, so ties go to the earlier entry
// (city > road > coast > forest > underground). Returns SECTOR_TYPE_NONE
// if no cartography component has been tallied yet.
string query_sector_type()
{
  string * order;
  string best;
  int best_count, i, c;

  if (!type_counts) type_counts = ([ ]);

  order = SECTOR_CONTRIB_COMPONENTS;
  best = SECTOR_TYPE_NONE;
  best_count = 0;

  for (i = 0; i < sizeof(order); i++)
  {
    c = type_counts[order[i]];
    if (c > best_count)
    {
      best_count = c;
      best = order[i];
    }
  }

  return best;
}
