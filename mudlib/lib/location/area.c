inherit "/lib/core/object.c";

#include <room/location.h>

// mapping in the form ([ file_name : location_data ])
mapping locations;
// mapping in the form ([ file_name : ({ "direction", destination }) ])
mapping connections;
// array of loaded locations
static object * loaded_locations;
string file_name;
string area_path;

// prototype functions
void add_loaded_location(object location);


void create() {
  locations = ([ ]);
  loaded_locations = ({ });
  connections = ([ ]);
  file_name = "";
  area_path = "";
  ::create();
}

void restore_me() {
  restore_object(file_name);
}

void save_me() {
  save_object(file_name);
}

string query_file_name() { return file_name; }
void set_file_name(string name)
{
  file_name = name;
  save_me();
}

mapping query_locations() { return locations; }

// The full save path of this area
// (e.g. /save/games/rl-aeternum/locations/areas/elfereth/rooms/), set
// by the area handler when the storage is created.
string query_area_path() { return area_path; }

/**
 * Short identifier of the area: everything after the "areas/" segment
 * of the storage path, with the trailing "/" stripped.
 *
 * For "/save/games/rl-aeternum/locations/areas/elfereth/rooms/" this
 * returns "elfereth/rooms"; nested area trees (e.g. an inner-city
 * sub-area) keep their inner structure in the name. Falls back to the
 * full path if "areas/" is not present.
 */
string query_area_name()
{
  string * parts;
  int i;
  parts = explode(area_path, "/") - ({ "" });
  for (i = 0; i < sizeof(parts); i++)
    if (parts[i] == "areas" && i + 1 < sizeof(parts))
      return implode(parts[i + 1..], "/");
  return area_path;
}

// Set by the area handler when the storage is created. The argument
// is the full save path (/save/games/.../areas/<name>/...); the public
// "area name" (the short identifier) is derived from it.
void set_area_path(string path)
{
  area_path = path;
  // save_me();
}

// restore the area from a file
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

object load_location(string location_file_name)
{
  object location;
  int i;

  // check if this location is in this area
  if (member_array(location_file_name, keys(locations)) == -1)
    return nil;

  // removed already dest'ed locations
  loaded_locations -= ({ nil });

  // look through loaded_locations to see if it's already loaded
  for (i = 0; i < sizeof(loaded_locations); i++)
  {
    if (loaded_locations[i]->query_file_name() == location_file_name)
    {
      // write("🎃 location already loaded: " + location_file_name + "\n");
      return loaded_locations[i];
    }
  }

  location = clone_object(BASE_LOCATION_OBJ);

  if (!location)
    return nil;

  location->restore_from_file_name(location_file_name);
  loaded_locations += ({ location });

  return location;
}

void add_location(string location_file_name, mapping location_data) 
{
  locations[location_file_name] = map_copy(location_data);
  save_me();
}

void remove_location(string location_file_name) 
{
  map_delete(locations, location_file_name);
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

mapping query_connections() { return connections; }
void add_connection(string location_file_name, string direction, string destination) 
{
  if (!connections[location_file_name])
    connections[location_file_name] = ({ direction, destination });

  save_me();
}
