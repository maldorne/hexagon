// component.c -- base every NPC component inherits.
//
// Analogous to /lib/location/component.c, but for NPCs and deliberately
// lighter: it inherits /lib/core/object.c, not container.c, because an NPC
// component holds no inventory and never moves (container = object + contents;
// we want just the object half). A component is a small owned object carrying
// one slice of an NPC's behaviour -- a schedule, a shop, a patrol, a dialogue.
//
// PERSISTENCE (same contract as location components): the NPC is the courier,
// the component the owner. The NPC's `component_info` mapping (type -> attrs)
// rides in the npc.o savefile; the live instance is `static` and re-cloned on
// restore. Declare every persisted field in query_auto_load_attributes and
// restore it in init_auto_load_attributes -- pair them exactly. The NPC
// refreshes the snapshot on every save (sync_component_info), so a component
// never calls save itself.

inherit obj "/lib/core/object.c";

private string type;   // component type (= blueprint filename under components/)
static object owner;    // the NPC this component belongs to

int query_npc_component() { return 1; }

void create()
{
  obj::create();
  type = "base";
  owner = nil;
}

// Called after creation + init_auto_load_attributes, with the owning NPC.
// Override to wire up subscriptions (e.g. a schedule subscribing to the hourly
// calendar tick), remembering to undo them if the component can be removed.
void initialize(object npc)
{
  owner = npc;
}

object query_owner() { return owner; }

string query_type() { return type; }
void set_type(string t) { type = t; }

// Persisted state. The base only carries its type; subclasses extend both
// halves in lockstep, namespacing their keys (schedule_entries, shop_goods, ...)
// so composed mixins never collide.
mapping query_auto_load_attributes()
{
  return ([ "type" : type ]);
}

void init_auto_load_attributes(mapping args)
{
  if (args && !undefinedp(args["type"]))
    type = args["type"];
}

mixed * stats()
{
  return ({ ({ "Component Type", type }) });
}
