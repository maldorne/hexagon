inherit "/lib/core/object.c";

// mapping in the form ([ file_name : location_data ])
mapping locations;
// mapping in the form ([ x_y_z : file_name ])
mapping positions;
// array of loaded locations
static object * loaded_locations;
string file_name;

void create() {
  locations = ([ ]);
  positions = ([ ]);
  loaded_locations = ({ });
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
  locations[location_file_name] = map_copy(location_data);
  positions["" + x + "_" + y + "_" + z] = location_file_name;
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
