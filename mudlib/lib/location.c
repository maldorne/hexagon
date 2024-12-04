
// location.c

inherit obj      "/lib/core/object.c";
inherit property "/lib/core/basic/property.c";

inherit contents "/lib/room/contents";
inherit exits    "/lib/room/exits";
inherit zone     "/lib/room/zone";

#include <areas/area.h>
#include <maps/maps.h>

static object * components;
mapping component_info;

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
// void init_original_info();

int query_location() { return 1; }
int query_room() { return 1; }
// allow adding and removing objects from the inventory (is a room)
int add_weight(int n) { return 1; }
int test_add(object ob, int flag) { return 1; }
int test_remove(object ob, int flag) { return 1; }

void create() 
{
  components = ({ });
  component_info = ([ ]);

  _original_long = "";
  _original_short = "";
  _exit_map = ([ ]);

  file_name = "";
  map_name = "default";

  coordinates = nil;

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
    ret = _original_long;

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
    return 1;
  }

  return 0;
}

void guess_coordinates()
{
  int x, y, z;
  int i;
  string * exit_info;

  if (query_coordinates() != nil)
    return;

  exit_info = query_dest_dir();

  for (i = 0; i < sizeof(exit_info); i += 2)
  {
    object dest;
    dest = load_object(exit_info[i + 1]);

    if (dest && dest->query_coordinates() != nil)
    {
      int * dest_coords;
      dest_coords = dest->query_coordinates();

      switch (exit_info[i])
      {
        case "north":
          set_coordinates(dest_coords[0], 
                          dest_coords[1] - 1, 
                          dest_coords[2]);
          break;
        case "south":
          set_coordinates(dest_coords[0], 
                          dest_coords[1] + 1, 
                          dest_coords[2]);
          break;
        case "east":
          set_coordinates(dest_coords[0] - 1, 
                          dest_coords[1], 
                          dest_coords[2]);
          break;
        case "west":
          set_coordinates(dest_coords[0] + 1, 
                          dest_coords[1], 
                          dest_coords[2]);
          break;
        case "northwest":
          set_coordinates(dest_coords[0] + 1, 
                          dest_coords[1] - 1, 
                          dest_coords[2]);
          break;
        case "northeast":
          set_coordinates(dest_coords[0] - 1, 
                          dest_coords[1] - 1, 
                          dest_coords[2]);
          break;
        case "southwest":
          set_coordinates(dest_coords[0] + 1, 
                          dest_coords[1] + 1, 
                          dest_coords[2]);
          break;
        case "southeast":
          set_coordinates(dest_coords[0] - 1, 
                          dest_coords[1] + 1, 
                          dest_coords[2]);
          break;          
        case "up":
          set_coordinates(dest_coords[0], 
                          dest_coords[1], 
                          dest_coords[2] - 1);
          break;
        case "down":
          set_coordinates(dest_coords[0], 
                          dest_coords[1], 
                          dest_coords[2] + 1);
          break;
      }
    }
  }
}

void save_me()
{
  stderr("🎃 location save_me: " + file_name + "\n");
  // save with the current exits
  _exit_map = query_exit_map();

  // try to update our coordinates (if we don't have them,
  // we will check adjacent locations)
  guess_coordinates();

  // save in the map system every location with coordinates
  if (query_coordinates() != nil)
    MAP_HANDLER->add_location(this_object());

  // save it in the area system also
  AREA_HANDLER->add_location(this_object());

  if (file_name && strlen(file_name))
    save_object(file_name);
}

mixed stats()
{
  return ({
    ({ "Component types", keys(component_info), }),
          }) +
      obj::stats() +
      property::stats() + 
      zone::stats() + 
      exits::stats();

      // light::stats() +
      // desc::stats() +
      // guard::stats() +
      // navigation::stats() +
      // diplomacy::stats();
}
