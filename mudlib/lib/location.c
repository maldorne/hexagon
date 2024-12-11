
// location.c

inherit obj      "/lib/core/object.c";
inherit light    "/lib/core/basic/light.c";
inherit property "/lib/core/basic/property.c";

inherit contents "/lib/room/contents.c";
inherit exits    "/lib/room/exits.c";
inherit zone     "/lib/room/zone.c";

#include <basic/light.h>
#include <room/location.h>
#include <areas/area.h>
#include <maps/maps.h>
#include <translations/exits.h>

static object * components;
mapping component_info;

string _original_room_file_name;
string _original_long;
string _original_short;
mapping _exit_map;

string file_name;
string map_name;

int * coordinates;

// function prototypes
string query_file_name();
void set_file_name(string filename);
void save_me();
void add_exits_from_exit_map(mapping m);
// void init_original_info();

nomask int query_location() { return 1; }
nomask int query_room() { return 1; }
// allow adding and removing objects from the inventory (is a room)
int add_weight(int n) { return 1; }
int test_add(object ob, int flag) { return 1; }
int test_remove(object ob, int flag) { return 1; }

void create() 
{
  components = ({ });
  component_info = ([ ]);

  _original_room_file_name = "";
  _original_long = "";
  _original_short = "";
  _exit_map = ([ ]);

  file_name = "";
  map_name = "default";

  coordinates = nil;

  light::create();
  property::create();
  contents::create();
  exits::create();
  zone::create();
  // the last one
  obj::create();

  // init_original_info();

  // don't save/restore the location if it's the location.c object itself
  // if (base_name(this_object()) != "/lib/location")
  // {
  //   if (file_size(query_file_name()) >= 0)
  //     restore_object(query_file_name());
  //   else
  //     save_me();
  // }
  set_light(BASE_ROOM_LIGHT_VALUE);

  // allow goto command to work
  add_property("location", 1);
}

void init()
{
  contents::init();
  exits::init();
}

int id(string str)
{
  return 0;
}

string query_original_room_file_name() { return _original_room_file_name; }

void set_original_room_file_name(string str) { _original_room_file_name = str; }
string query_original_short() { return _original_short; }
void set_original_short(string str) { _original_short = str; }
string query_original_long() { return _original_long; }
void set_original_long(string str) { _original_long = str; }

int * query_coordinates() { return coordinates; }
void set_coordinates(int x, int y, int z) { coordinates = ({ x, y, z }); }

string query_map_name() { return map_name; }
void set_map_name(string name) { map_name = name; }

void add_component(string component_type, mixed properties)
{
  component_info[component_type] = ({ properties });
}

void remove_component(string component_type)
{
  map_delete(component_info, component_type);
}

// void init_original_info()
// {
//   if (this_object()->query_short())
//     _original_short = this_object()->query_short();
//   if (this_object()->query_long())
//     _original_long = this_object()->query_long();
// }

string short(varargs int dark)
{
  string ret;
  string * component_types;
  int i;

  component_types = keys(component_info);

  for (i = 0; i < sizeof(component_types); i++)
    ret += component_info[component_types[i]][0]->short(dark);

  if (!ret || !strlen(ret))
    ret = _original_short;

  return ret;
}

string long(string str, int dark)
{
  string ret;
  string * component_types;
  int i;

  component_types = keys(component_info);

  for (i = 0; i < sizeof(component_types); i++)
    ret += component_info[component_types[i]][0]->long(str, dark);

  if (!ret || !strlen(ret))
    // ret = _original_long;
    ret = wrap(_original_long, 
                (this_user() ? this_user()->query_cols() : 80), 1); 
                
  exit_string = query_dirs_string();
  ret += exit_string + "\n" + query_contents("");

  return ret;
}

string query_file_name() { return file_name; }
void set_file_name(string name)
{
  file_name = name;
  save_me();
}

// restore the location from a file
// returns 1 if the file exists and the location was restored, 0 otherwise
int restore_from_file_name(string name)
{
  if (file_size(name) >= 0)
  {
    restore_object(name);

    add_exits_from_exit_map(_exit_map);

    return 1;
  }

  return 0;
}

void guess_coordinates()
{
  int x, y, z;
  int i;
  string * exit_info;

  // if (query_coordinates() != nil)
  //   return;

  exit_info = query_dest_dir();

  for (i = 0; i < sizeof(exit_info); i += 2)
  {
    object dest;
    dest = query_dest_object(exit_info[i]);

    if (dest && dest->query_coordinates() != nil)
    {
      int * dest_coords;
      dest_coords = dest->query_coordinates();

      switch (exit_info[i])
      {
        case DIR_NORTH:
          set_coordinates(dest_coords[0], 
                          dest_coords[1] - 1, 
                          dest_coords[2]);
          break;
        case DIR_SOUTH:
          set_coordinates(dest_coords[0], 
                          dest_coords[1] + 1, 
                          dest_coords[2]);
          break;
        case DIR_EAST:
          set_coordinates(dest_coords[0] - 1, 
                          dest_coords[1], 
                          dest_coords[2]);
          break;
        case DIR_WEST:
          set_coordinates(dest_coords[0] + 1, 
                          dest_coords[1], 
                          dest_coords[2]);
          break;
        case DIR_NORTHWEST:
          set_coordinates(dest_coords[0] + 1, 
                          dest_coords[1] - 1, 
                          dest_coords[2]);
          break;
        case DIR_NORTHEAST:
          set_coordinates(dest_coords[0] - 1, 
                          dest_coords[1] - 1, 
                          dest_coords[2]);
          break;
        case DIR_SOUTHWEST:
          set_coordinates(dest_coords[0] + 1, 
                          dest_coords[1] + 1, 
                          dest_coords[2]);
          break;
        case DIR_SOUTHEAST:
          set_coordinates(dest_coords[0] - 1, 
                          dest_coords[1] + 1, 
                          dest_coords[2]);
          break;          
        case DIR_UP:
          set_coordinates(dest_coords[0], 
                          dest_coords[1], 
                          dest_coords[2] - 1);
          break;
        case DIR_DOWN:
          set_coordinates(dest_coords[0], 
                          dest_coords[1], 
                          dest_coords[2] + 1);
          break;
      }
    }
  }

  save_me();
}

// we save the full exit map
// and restore it when the location is loaded
void add_exits_from_exit_map(mapping m)
{ 
  int i;
  string * exit_names;
  string * exits_info;

  exit_names = keys(m);

  for (i = 0; i < sizeof(exit_names); i++)
  {
    exits_info = m[exit_names[i]];

    // first remove the current exit if it exists
    remove_exit(exit_names[i]);

    add_exit(exit_names[i], 
             exits_info[0], 
             exits_info[1], 
             exits_info[2]);
  }
}

// check every exit, if it goes to a room already converted to location, 
// change its destination to the saved location file
int update_exits_to_locations()
{
  string * keys;
  int i;
  int exits_changed;

  keys = keys(_exit_map);
  exits_changed = 0;

  for (i = 0; i < sizeof(keys); i++)
  {
    object dest;
    string dest_location_file_name;

    dest = load_object(_exit_map[keys[i]][0]);

    if (!dest)
      continue;

    dest_location_file_name = LOCATION_HANDLER->get_location_file_name_from_room(dest);

    // if file already exists, change the exit destination
    if ((_exit_map[keys[i]][0] != dest_location_file_name) &&
        (file_size(dest_location_file_name) >= 0))
    {
      _exit_map[keys[i]][0] = dest_location_file_name;
      exits_changed += 1;
    }
  }

  if (exits_changed > 0)
  {
    add_exits_from_exit_map(map_copy(_exit_map));
    save_me();
  }

  return exits_changed;
}

object * find_inv_match(string str)
{
  return (object *)all_inventory(this_object()) + 
         (object *)contents::query_hidden_objects();
         // m_values(items);
}

void save_me()
{
  stderr("🎃 location save_me: " + file_name + "\n");
  // save with the current exits
  _exit_map = query_exit_map();

  // save in the map system every location with coordinates
  if (query_coordinates() != nil)
    MAPS_HANDLER->add_location(this_object());

  // save it in the area system also
  AREA_HANDLER->add_location(this_object());

  if (file_name && strlen(file_name))
    save_object(file_name);
}

mixed stats()
{
  return ({
    ({ "Components", keys(component_info), }),
          }) +
      obj::stats() +
      light::stats() +
      property::stats() + 
      zone::stats() + 
      exits::stats();

      // light::stats() +
      // desc::stats() +
      // guard::stats() +
      // navigation::stats() +
      // diplomacy::stats();
}
