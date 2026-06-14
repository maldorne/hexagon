// Base class every location component inherits from.
//
// PERSISTENCE CONTRACT
// --------------------
// The parent location is the courier; the component is the owner.
// `location.c::component_info` is a mapping `component_type -> attrs`
// that travels inside the location's own `.o` file. The component
// instance itself (`static` in the location, freshly cloned on every
// restore) is never serialised directly.
//
// The flow is symmetric, pull-on-save / push-on-restore:
//
//   save_me()         -> for each live component, write
//                        `component->query_auto_load_attributes()` into
//                        `component_info[type]`, then `save_object`.
//   restore_from_*()  -> `init_components(component_info)` clones every
//                        listed component, calls
//                        `component->init_auto_load_attributes(attrs)`,
//                        then `component->initialize(this_object())`.
//
// Implications for component authors:
//
// * Declare every field that must survive a reload in
//   `query_auto_load_attributes()` and restore it in
//   `init_auto_load_attributes()` — pair them exactly. The location
//   refreshes the snapshot on every `save_me()`, so you never need to
//   call `save_me` yourself.
// * Namespace your keys with a short prefix
//   (`shop_permanent_goods`, `pub_menu_items`, ...) so multiple mixins
//   composed in one component can merge their mappings without
//   colliding. The chain in `shop.c` / `pub.c` is the canonical
//   example.
// * State that doesn't fit a flat mapping (large inventories, logs)
//   should live in side files whose paths derive from
//   `query_my_location()->query_file_name()` — never accept a path
//   from a setter. See `shop-inventory.c::query_save_file_name()` for
//   the pattern.

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
