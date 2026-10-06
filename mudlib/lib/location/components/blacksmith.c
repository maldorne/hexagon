// Blacksmith component. Marks a location as a smithy: it has a forge, an anvil
// and the tools to work metal, and it is where the repair skill can be used.
//
// The forge is lit while somebody works it: somebody whose work is this place
// and is standing in it. Without a job declared here the forge is always lit,
// so a smithy that nobody staffs is still a smithy.
//
// The forge, the anvil and the bellows are props of the location, added by this
// component when missing. It keeps the forge's lit state in step with who is at
// work, whenever somebody comes in or goes out.

#include <room/location.h>
#include <room/prop.h>

#define FORGE_PROP_TYPE    "forge"
#define SMITHY_PROP_TYPES  ({ FORGE_PROP_TYPE, "anvil", "bellows" })

inherit component "/lib/location/component.c";

void create()
{
  component::create();
  set_type(LOCATION_COMPONENT_BLACKSMITH);
}

void init() {}
void dest_me() {}

void add_smithy_props();
void update_forge_props();

// Once every component of the location exists, the props one can be reached.
void components_ready()
{
  add_smithy_props();
}

// Whether the forge is burning: the same condition a shop opens under, somebody
// who works here is in it (or nobody is meant to). See
// /lib/ventures/conditions/attended.c.
int query_forge_lit()
{
  object loc;

  loc = query_my_location();
  if (!loc)
    return 0;

  return call_other("/lib/ventures/conditions/attended", "check_open",
                    this_object(), loc, nil, ([ ])) == 1;
}

string query_info()
{
  return query_forge_lit() ? "forge lit" : "forge cold";
}

// Gives the smithy the props every smithy has (a forge, an anvil and a pair of
// bellows), attaching the props component if the location has none yet. The
// rest of the furnishing belongs to each location.
void add_smithy_props()
{
  object loc, props;
  int i, added;

  loc = query_my_location();
  if (!loc)
    return;

  props = loc->query_component_by_type(LOCATION_COMPONENT_PROPS);
  if (!props)
  {
    loc->add_component(LOCATION_COMPONENT_PROPS, ([ "props_instances": ({ }) ]));
    props = loc->query_component_by_type(LOCATION_COMPONENT_PROPS);
    if (!props)
      return;
  }

  for (i = 0; i < sizeof(SMITHY_PROP_TYPES); i++)
  {
    if (sizeof(props->query_instances_by_type(SMITHY_PROP_TYPES[i])))
      continue;
    props->add_prop_instance(SMITHY_PROP_TYPES[i], SMITHY_PROP_TYPES[i] + "_1", ([ ]));
    added = 1;
  }

  update_forge_props();

  // saved once the location has finished loading
  if (added)
  {
    props->refresh_actions();
    call_out("save_location", 0);
  }
}

void save_location()
{
  if (query_my_location())
    query_my_location()->save_me();
}

// Sets the lit state of every forge prop from who is at work.
void update_forge_props()
{
  object loc, props;
  mapping * forges;
  int lit, i;

  loc = query_my_location();
  if (!loc)
    return;

  props = loc->query_component_by_type(LOCATION_COMPONENT_PROPS);
  if (!props)
    return;

  forges = props->query_instances_by_type(FORGE_PROP_TYPE);
  lit = query_forge_lit();

  for (i = 0; i < sizeof(forges); i++)
    props->set_state(forges[i][PROP_FIELD_ID], "lit", lit);
}

// Somebody arriving or leaving may be the smith. Somebody arriving is already
// here; somebody leaving still is, so that state is read once the move is over.
void event_enter(object ob, varargs string msg, object from, mixed avoid)
{
  if (ob && living(ob))
    update_forge_props();
}

void event_exit(object ob, varargs string msg, object dest, mixed avoid)
{
  if (ob && living(ob))
    call_out("update_forge_props", 0);
}
