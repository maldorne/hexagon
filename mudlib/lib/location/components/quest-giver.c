// Quest-giver component: a location that hands quests out or is where they end.
//
// For a game made of locations. Everything a quest giver knows and answers
// comes from /lib/quests/giver.c, the same code a room or a monster inherits
// directly -- the way a shop location component and a shop room share the code
// under /lib/ventures/. This file only adds what a component needs on top:
// being attached to a location and saved with it.
//
// Declared in the location's components, attributes and all:
//
//   "quest-giver" : ([ "quest_offered"  : ({ "<game>:kill-the-wasps" }),
//                      "quest_completed": ({ "<game>:kill-the-wasps" }),
//                      "quest_races"    : ({ "human" }) ])

#include <room/location.h>

inherit component "/lib/location/component.c";
inherit giver     "/lib/quests/giver.c";

void create()
{
  component::create();
  set_type(LOCATION_COMPONENT_QUEST_GIVER);
}

string query_info()
{
  return implode(query_offered_quests() + query_completed_quests(), ", ");
}

// What it deals in travels with the location, so it stays a quest giver across
// a reboot without the area having to be converted again.
mapping query_auto_load_attributes()
{
  return component::query_auto_load_attributes() +
    ([ "quest_offered"  : query_offered_quests(),
       "quest_completed": query_completed_quests(),
       "quest_races"    : query_dealt_races() ]);
}

void init_auto_load_attributes(mapping args)
{
  component::init_auto_load_attributes(args);

  if (!args)
    return;

  if (!undefinedp(args["quest_offered"]))
    set_offered_quests(args["quest_offered"]);
  if (!undefinedp(args["quest_completed"]))
    set_completed_quests(args["quest_completed"]);
  if (!undefinedp(args["quest_races"]))
    deals_with_races(args["quest_races"]);
}

mixed * stats()
{
  return component::stats() +
         ({ ({ "Quests offered", implode(query_offered_quests(), ", "), }),
            ({ "Quests completed", implode(query_completed_quests(), ", "), }),
            ({ "Races dealt with", implode(query_dealt_races(), ", "), }), });
}
