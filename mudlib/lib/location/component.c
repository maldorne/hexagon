// basic component all location components will inherit from

// inherit obj "/lib/core/object.c";
inherit container "/lib/core/basic/container.c";

private string type;  // type of component (outside, shop, swamp, whatever)
static object location;

int query_location_component() { return 1; }

// do not allow by default adding and removing objects from the inventory
// if you need a particular component to allow this, override these functions
// (i.e. shop, vault, etc, will clone a vault object and will put it 
// temporarily in the inventory)
int test_add(object ob, int flag) { return 0; }
int test_remove(object ob, int flag) { return 0; }

void create()
{
  container::create();
  type = "base";
  location = nil;
}

object query_my_location() { return location; }

// will be called after creation + init_auto_load_attributes,
// providing de parent location object
void initialize(object loc)
{
  location = loc;
}

// if one of the components of a location returns 1 to an override_function,
// is indicating its result is the only one that matters
// i.e.: long() will take into account the description of every component,
// but if one of them returns 1 to override_function("long"), it will be only
// its long() what will be returned

int override_function(string func)
{
  // by default a component never overrides anything
  return 0;
}

string extra_look()
{
  // default implementation, return empty string
  return "";
}

// basic component functions
string query_type() { return type; }
void set_type(string t) { type = t; }

mapping query_auto_load_attributes() 
{
  // return container::query_auto_load_attributes() +
  return ([ "type" : type, ]);
}

void init_auto_load_attributes(mapping args) 
{
  if (!undefinedp(args["type"]))
    type = args["type"];

  // container::init_auto_load_attributes(args);
}

mixed * stats()
{
  // for the components stats, give just the essential info
  // return container::stats() +
  return ({
    ({ "Component Type", type, }),
         });
}
