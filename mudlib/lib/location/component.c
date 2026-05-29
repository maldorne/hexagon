// Base class every location component inherits from.
//
// PERSISTENCE CONTRACT
// --------------------
// A component owns the persistence of its own state. The parent location
// stores a `component_info` mapping naming which components it has, but
// does NOT serialise the components' instance variables. On restore, the
// location re-clones the component fresh and calls
// `init_auto_load_attributes()` on it; everything else is up to the
// component.
//
// Components that hold state (shop inventory, NPC patrol schedule, quest
// progress, ...) extend `query_auto_load_attributes()` and
// `init_auto_load_attributes()` below to include that state. The data
// then travels with the location's `.o` file through the `component_info`
// channel.

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

// Hook registration. A component declares which of the location's
// public functions it wants to participate in, and at what priority
// bucket (see include/room/location_hooks.h). The location reads this
// at init_components time, builds an ordered chain per function, and
// dispatches calls through it.
//
// Override in subclasses. Example:
//   mapping query_hooks() {
//     return ([ "do_exit_command": HOOK_PRIORITY_TRANSFORM ]);
//   }
//
// Then implement the matching `hook_<func>(mixed * args)` function.
// The contract (transform args / return a piece) is documented per
// hook in location.c.
mapping query_hooks() { return ([ ]); }

// Dispatcher entry point used by the location. Resolves to this
// component's hook_<func> implementation. Kept here so the location
// has a single uniform call site regardless of which component it is
// invoking.
mixed call_hook(string func, mixed * args)
{
  return call_other(this_object(), "hook_" + func, args);
}

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
