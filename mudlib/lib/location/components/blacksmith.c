// Blacksmith component. Marks a location as a smithy: it has a forge, an anvil
// and the tools to work metal, and it is where the repair skill can be used.
//
// The forge is lit while somebody works it: somebody whose work is this place
// and is standing in it. Without a job declared here the forge is always lit,
// so a smithy that nobody staffs is still a smithy.
//
// The forge itself is a prop of the location ("forge"), so it can be looked at.
// This component adds it when missing and keeps its lit state in step with who
// is at work, whenever somebody comes in or goes out.

#include <room/location.h>
#include <room/prop.h>

#define FORGE_PROP_TYPE "forge"
#define FORGE_PROP_ID   "forge_1"

inherit component "/lib/location/component.c";

void create()
{
  component::create();
  set_type(LOCATION_COMPONENT_BLACKSMITH);
}

void init() {}
void dest_me() {}

void add_forge_prop();
void update_forge_props();

void initialize(object loc)
{
  component::initialize(loc);
  // not during the location's own component setup
  call_out("add_forge_prop", 0);
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

// Gives the smithy its forge prop, attaching the props component if the
// location has none yet.
void add_forge_prop()
{
  object loc, props;

  loc = query_my_location();
  if (!loc)
    return;

  props = loc->query_component_by_type(LOCATION_COMPONENT_PROPS);
  if (props && sizeof(props->query_instances_by_type(FORGE_PROP_TYPE)))
  {
    update_forge_props();
    return;
  }

  if (!props)
  {
    loc->add_component(LOCATION_COMPONENT_PROPS, ([ "props_instances": ({ }) ]));
    props = loc->query_component_by_type(LOCATION_COMPONENT_PROPS);
    if (!props)
      return;
  }

  props->add_prop_instance(FORGE_PROP_TYPE, FORGE_PROP_ID, ([ ]));
  props->refresh_actions();
  update_forge_props();
  loc->save_me();
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

// Somebody arriving or leaving may be the smith. The state is read once the
// move is over, when the leaver is no longer here.
void event_enter(object ob, varargs string msg, object from, mixed avoid)
{
  if (ob && living(ob))
    call_out("update_forge_props", 0);
}

void event_exit(object ob, varargs string msg, object dest, mixed avoid)
{
  if (ob && living(ob))
    call_out("update_forge_props", 0);
}
