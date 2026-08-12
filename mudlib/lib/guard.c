// guard.c -- a persisted area NPC that also carries the guardian role.
//
// This is what the area clones (GUARD_NPC) for a citizenship's guards. It is a
// generic persisted NPC in every respect -- it takes its appearance, race and
// level from a bestiary template like any area NPC -- plus the guardian mixin,
// which adds the exit check. The area stamps its city_ob (the area's
// citizenship) and the direction it watches when it places it.

inherit npc      "/lib/npc.c";
inherit guardian "/lib/npc/guardian.c";

void create()
{
  npc::create();
}
