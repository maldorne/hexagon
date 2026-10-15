// component.c -- base every NPC component inherits.
//
// A component of an NPC (a schedule, a guard post, a toll) is a component as
// /lib/core/ecs/component.c defines it, plus the object half: it inherits
// /lib/core/object.c, not container.c, because it holds no inventory and never
// moves. Its entity is the NPC (query_entity), whose component_info rides in
// the npc.o.

inherit obj "/lib/core/object.c";
inherit ecs "/lib/core/ecs/component.c";

void create()
{
  obj::create();
  ecs::create();
}

mapping query_auto_load_attributes()
{
  return ecs::query_auto_load_attributes();
}

void init_auto_load_attributes(mapping args)
{
  ecs::init_auto_load_attributes(args);
}

mixed * stats()
{
  return ecs::stats();
}
