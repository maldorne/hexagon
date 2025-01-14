
// location.c

inherit obj      "/lib/core/object.c";
inherit light    "/lib/core/basic/light.c";
inherit property "/lib/core/basic/property.c";

inherit contents "/lib/room/contents.c";
inherit exits    "/lib/room/exits.c";
inherit zone     "/lib/room/zone.c";
inherit sign     "/lib/room/sign.c";

#include <basic/light.h>
#include <room/location.h>
#include <areas/area.h>
#include <areas/common.h>
#include <maps/maps.h>
#include <translations/exits.h>

static object * components;
mapping component_info;

string _original_room_file_name;
string _original_long;
string _original_short;
mapping _exit_map;

string file_name;  // .o file of the location
string area_name;  // path where we will find the area.o and our own file
string map_name;   // "default", or "underdark", or "mars", use something different 
                   // from "default" to build a new map

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
  area_name = "";
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

void add_component(string component_type, mapping properties)
{
  component_info[component_type] = properties;
}

void remove_component(string component_type)
{
  map_delete(component_info, component_type);
}

void init_components(mapping info)
{
  string * component_paths;
  int i;

  component_paths = keys(info);

  for (i = 0; i < sizeof(component_paths); i++)
  {
    object component;
    component = clone_object("/lib/location/components/" + component_paths[i] + ".c");

    if (!component)
      continue;

    // initialize variable values
    component->init_auto_load_attributes(info[component_paths[i]]);
    component->initialize(this_object());
    components += ({ component });
  }
}

// void init_original_info()
// {
//   if (this_object()->query_short())
//     _original_short = this_object()->query_short();
//   if (this_object()->query_long())
//     _original_long = this_object()->query_long();
// }

string query_components_string()
{
  int i;
  string * types;

  if (!sizeof(components))
    return "";

  types = ({ });

  for (i = 0; i < sizeof(components); i++)
    types += ({ components[i]->query_type() });

  return "%^BOLD%^CYAN%^Components: " + implode(types, ", ") + ".%^RESET%^";
}

string short(varargs int dark)
{
  string ret;
  int i;

  ret = "";

  for (i = 0; i < sizeof(components); i++)
    ret += components[i]->short(dark);

  if (!ret || !strlen(ret))
    ret = _original_short;

  return ret;
}

string long(string str, int dark)
{
  string ret, aux;
  int i;

  ret = "";

  for (i = 0; i < sizeof(components); i++)
    ret += components[i]->long(str, dark);

  if (!ret || !strlen(ret))
    // ret = _original_long;
    ret = wrap(_original_long, 
                (this_user() ? this_user()->query_cols() : 80), 1); 

  aux = query_components_string();

  if (this_player()->query_coder() && strlen(aux))
    ret += aux + "\n";

  exit_string = query_dirs_string();
  ret += exit_string + "\n" + query_contents("");

  return ret;
}

string query_area_name() { return area_name; }
string query_file_name() { return file_name; }
void set_file_name(string name)
{
  string * pieces;
  pieces = explode(name, "/");

  if (sizeof(pieces) >= 2)
    area_name = "/" + implode(pieces[0..sizeof(pieces) - 2], "/") + "/";

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

    init_components(component_info);

    return 1;
  }

  return 0;
}

int guess_coordinates()
{
  int x, y, z;
  int i;
  string * exit_info;
  int found;

  found = FALSE;

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
          found = TRUE;
          break;
        case DIR_SOUTH:
          set_coordinates(dest_coords[0], 
                          dest_coords[1] + 1, 
                          dest_coords[2]);
          found = TRUE;
          break;
        case DIR_EAST:
          set_coordinates(dest_coords[0] - 1, 
                          dest_coords[1], 
                          dest_coords[2]);
          found = TRUE;
          break;
        case DIR_WEST:
          set_coordinates(dest_coords[0] + 1, 
                          dest_coords[1], 
                          dest_coords[2]);
          found = TRUE;
          break;
        case DIR_NORTHWEST:
          set_coordinates(dest_coords[0] + 1, 
                          dest_coords[1] - 1, 
                          dest_coords[2]);
          found = TRUE;
          break;
        case DIR_NORTHEAST:
          set_coordinates(dest_coords[0] - 1, 
                          dest_coords[1] - 1, 
                          dest_coords[2]);
          found = TRUE;
          break;
        case DIR_SOUTHWEST:
          set_coordinates(dest_coords[0] + 1, 
                          dest_coords[1] + 1, 
                          dest_coords[2]);
          found = TRUE;
          break;
        case DIR_SOUTHEAST:
          set_coordinates(dest_coords[0] - 1, 
                          dest_coords[1] + 1, 
                          dest_coords[2]);
          found = TRUE;
          break;          
        case DIR_UP:
          set_coordinates(dest_coords[0], 
                          dest_coords[1], 
                          dest_coords[2] - 1);
          found = TRUE;
          break;
        case DIR_DOWN:
          set_coordinates(dest_coords[0], 
                          dest_coords[1], 
                          dest_coords[2] + 1);
          found = TRUE;
          break;
      }
    }
  }

  save_me();

  return found;
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
        (file_size(dest_location_file_name) > 0))
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

object query_area()
{
  return LOCATION_HANDLER->query_area_from_location_file_name(query_file_name());
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

void dest_me()
{
  object * arr;
  int i;

  // similar to room.c
  arr = all_inventory(this_object());
  arr -= ({ nil });

  for (i = 0; i < sizeof(arr); i++)
  {
    // important, so we do not destroy the player avatar
    if (interactive(arr[i]))
        arr[i]->move(ROOM_VOID);
    else
        arr[i]->dest_me();
  }

  destruct(this_object());
}

mixed stats()
{
  int i;
  mapping comp_stats;
  
  comp_stats = ([ ]);

  for (i = 0; i < sizeof(components); i++)
    comp_stats["Component " + components[i]->query_type()] = components[i]->stats();

  return obj::stats() +
         light::stats() +
         property::stats() + 
         zone::stats() + 
         exits::stats() +
          ({
      ({ "Map Name", map_name, }),
      ({ "Coordinates", coordinates, }),
      ({ "Components (nosave)", keys(component_info), }),
      ({ "Components stats (extra info)", comp_stats, }),
          });

      // light::stats() +
      // desc::stats() +
      // guard::stats() +
      // navigation::stats() +
      // diplomacy::stats();
}
