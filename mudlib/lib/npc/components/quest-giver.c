// quest-giver.c -- an npc that hands work out, as a component.
//
// For npcs in a game made of locations. Everything a quest giver knows and
// answers comes from /lib/monster/quest-giver.c, the same code a monster in a
// game made of rooms inherits directly -- the way a shop location component and
// a shop room share the code under /lib/ventures/. This file only adds what a
// component needs on top: being attached to an npc and saved with it.
//
// Declared in the npc template's components, attributes and all:
//
//   "quest-giver" : ([ "quest_offered" : ({ "<game>:kill-the-wasps" }),
//                      "quest_taken"   : ({ "<game>:kill-the-wasps" }),
//                      "quest_races"   : ({ "human" }) ])

inherit component "/lib/npc/component.c";
inherit giver     "/lib/monster/quest-giver.c";

void create()
{
  component::create();
}

// What it deals in travels with the npc, so a quest giver stays one across a
// reboot without the template having to be read again.
mapping query_auto_load_attributes()
{
  return component::query_auto_load_attributes() +
    ([ "quest_offered" : query_offered_quests(),
       "quest_taken"   : query_taken_quests(),
       "quest_races"   : query_dealt_races() ]);
}

void init_auto_load_attributes(mapping args)
{
  component::init_auto_load_attributes(args);

  if (!args)
    return;

  if (!undefinedp(args["quest_offered"]))
    set_offered_quests(args["quest_offered"]);
  if (!undefinedp(args["quest_taken"]))
    set_taken_quests(args["quest_taken"]);
  if (!undefinedp(args["quest_races"]))
    deals_with_races(args["quest_races"]);
}
