
// location.c

inherit obj      "/lib/core/object.c";
inherit property "/lib/core/basic/property.c";

inherit contents "/lib/room/contents";
inherit exits    "/lib/room/exits";
inherit zone     "/lib/room/zone";

static object * components;
mapping component_info;

string _original_long;
string _original_short;

string file_name;

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

  file_name = "";

  property::create();
  contents::create();
  exits::create();
  zone::create();
  // the last one
  obj::create();

  // init_original_info();

  // don't save/restore the location if it's the location.c object itself
  if (base_name(this_object()) != "/lib/location")
  {
    if (file_size(query_file_name()) >= 0)
      restore_object(query_file_name());
    else
      save_me();
  }

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

void save_me()
{
  if (file_name && strlen(file_name))
  {
    save_object(file_name);
  }

  // string path;

  // don't save the location if it's the location.c object itself
  // if (base_name(this_object()) == "/lib/location")
  //   return;

  // path = query_file_name();

  // // don't know where this location came from, 
  // // but i'm not going to save it
  // if (!strlen(path))
  //   return;

  // save_object(path);
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