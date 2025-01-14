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
