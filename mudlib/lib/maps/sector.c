#include <maps/sector.h>
#include <translations/exits.h>

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
// distribution). See include/maps/sector.h for the taxonomy.
mapping type_counts;
// mapping in the form ([ x_y_z : ([ direction : way_type ]) ]) — the
// path / road exits leaving each coordinate in this sector. way_type is
// one of SECTOR_WAY_TYPES; direction is a full exit name ("north", ...).
// Consumers read the raw per-coordinate graph via query_way_exits()
// (pathfinding), or the sector-border summary via query_border_ways()
// (world map rendering). See include/maps/sector.h.
mapping way_exits;
// array of loaded locations
static object * loaded_locations;
string file_name;

void create() {
  locations = ([ ]);
  positions = ([ ]);
  maze_positions = ([ ]);
  type_counts = ([ ]);
  way_exits = ([ ]);
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
  if (!way_exits) way_exits = ([ ]);

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

  // location_data may carry a "ways" mapping ([ direction : way_type ])
  // holding this coordinate's path / road exits, lifted from the
  // location's exit graph. Store it keyed by coord (or clear a stale
  // entry when the location no longer has any).
  if (location_data && location_data["ways"] &&
      map_sizeof(location_data["ways"]))
    way_exits[key] = map_copy(location_data["ways"]);
  else
    map_delete(way_exits, key);

  // Update per-type counters. A re-add for the same file is treated as
  // an update: decrement the previous contribution before adding the
  // new one so tallies stay accurate under recompiles / re-registration.
  // Missing keys in `type_counts` read as nil, so coerce with an
  // undefinedp guard on both the check and the arithmetic.
  old_types = (previous && previous["types"]) ? previous["types"] : ({ });
  for (i = 0; i < sizeof(old_types); i++)
  {
    int prev_count;
    prev_count = undefinedp(type_counts[old_types[i]]) ?
                 0 : type_counts[old_types[i]];
    if (prev_count > 0)
      type_counts[old_types[i]] = prev_count - 1;
  }

  new_types = (location_data && location_data["types"]) ?
              location_data["types"] : ({ });
  for (i = 0; i < sizeof(new_types); i++)
  {
    int prev_count;
    prev_count = undefinedp(type_counts[new_types[i]]) ?
                 0 : type_counts[new_types[i]];
    type_counts[new_types[i]] = prev_count + 1;
  }

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
  if (!way_exits) way_exits = ([ ]);

  // Roll back this file's contribution to the sector-type tally BEFORE
  // dropping its entry from `locations`, since that entry is where the
  // type list lives. Guard the arithmetic against missing keys reading
  // as nil.
  previous = locations[location_file_name];
  old_types = (previous && previous["types"]) ? previous["types"] : ({ });
  for (i = 0; i < sizeof(old_types); i++)
  {
    int prev_count;
    prev_count = undefinedp(type_counts[old_types[i]]) ?
                 0 : type_counts[old_types[i]];
    if (prev_count > 0)
      type_counts[old_types[i]] = prev_count - 1;
  }

  map_delete(locations, location_file_name);

  // also strip the coords -> file_name reverse mapping
  pos_keys = map_indices(positions);
  for (i = 0; i < sizeof(pos_keys); i++)
  {
    if (positions[pos_keys[i]] == location_file_name)
    {
      map_delete(positions, pos_keys[i]);
      map_delete(maze_positions, pos_keys[i]);
      map_delete(way_exits, pos_keys[i]);
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
    c = undefinedp(type_counts[order[i]]) ? 0 : type_counts[order[i]];
    if (c > best_count)
    {
      best_count = c;
      best = order[i];
    }
  }

  return best;
}

// Raw per-coordinate path / road exit graph:
//   ([ "x_y_z" : ([ direction : way_type ]) ])
// Intended for a pathfinder that walks the graph inside a sector. The
// world map renderer wants the border summary below instead.
mapping query_way_exits()
{
  if (!way_exits) way_exits = ([ ]);
  return way_exits;
}

// Sector index for a single coordinate axis, matching the convention in
// maps.c: sector n = n / 10 - (n < 0), so -1 lands in sector -1.
private int _sector_index(int n)
{
  return n / 10 - (n < 0);
}

private void _record_border(mapping acc, string border, string type)
{
  if (!acc[border])
    acc[border] = ({ });
  if (member_array(type, acc[border]) == -1)
    acc[border] += ({ type });
}

// Which cardinal borders of this sector are crossed by a path or road,
// and of which type(s). Returns
//   ([ "n": ({ "road" }), "e": ({ "path", "road" }), ... ])
// including only borders that actually carry a crossing. A way crosses a
// border when its destination coordinate (source + one step in the exit
// direction) falls in the neighbouring sector along that axis. Diagonal
// and vertical exits are kept in way_exits but ignored here — the border
// glyphs are cardinal. When a border carries both types the renderer is
// free to pick the heavier road (SECTOR_WAY_TYPES lists road first).
mapping query_border_ways()
{
  mapping result;
  string * coord_keys;
  int i;

  if (!way_exits) way_exits = ([ ]);
  result = ([ ]);

  coord_keys = map_indices(way_exits);
  for (i = 0; i < sizeof(coord_keys); i++)
  {
    int x, y, z;
    mapping ways;
    string * dirs;
    int j;

    if (sscanf(coord_keys[i], "%d_%d_%d", x, y, z) != 3)
      continue;

    ways = way_exits[coord_keys[i]];
    dirs = map_indices(ways);

    for (j = 0; j < sizeof(dirs); j++)
    {
      string type;
      type = ways[dirs[j]];

      switch (dirs[j])
      {
        case DIR_NORTH:
          if (_sector_index(y + 1) != _sector_index(y))
            _record_border(result, SECTOR_BORDER_N, type);
          break;
        case DIR_SOUTH:
          if (_sector_index(y - 1) != _sector_index(y))
            _record_border(result, SECTOR_BORDER_S, type);
          break;
        case DIR_EAST:
          if (_sector_index(x + 1) != _sector_index(x))
            _record_border(result, SECTOR_BORDER_E, type);
          break;
        case DIR_WEST:
          if (_sector_index(x - 1) != _sector_index(x))
            _record_border(result, SECTOR_BORDER_W, type);
          break;
      }
    }
  }

  return result;
}
