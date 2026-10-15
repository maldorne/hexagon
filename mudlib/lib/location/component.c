// Base class every location component inherits from: a component as
// /lib/core/ecs/component.c defines it, plus the container half, so a component
// can hold things for its location (the props, a shop's vault). Its entity is
// the location (query_entity).
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
//   `query_entity()->query_file_name()` — never accept a path
//   from a setter. See `shop-inventory.c::query_save_file_name()` for
//   the pattern.

inherit container "/lib/core/basic/container.c";
inherit ecs       "/lib/core/ecs/component.c";

// do not allow by default adding and removing objects from the inventory
// if you need a particular component to allow this, override these functions
// (i.e. shop, vault, etc, will clone a vault object and will put it 
// temporarily in the inventory)
int test_add(object ob, int flag) { return 0; }
int test_remove(object ob, int flag) { return 0; }

void create()
{
  container::create();
  ecs::create();
}

mapping query_auto_load_attributes() 
{
  // the container's own attributes are not kept: a component's inventory is
  // rebuilt by the component itself
  return ecs::query_auto_load_attributes();
}

void init_auto_load_attributes(mapping args) 
{
  ecs::init_auto_load_attributes(args);
}

mixed * stats()
{
  // for the components stats, give just the essential info
  return ecs::stats();
}
