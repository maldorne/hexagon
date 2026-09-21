// quest-giver.c -- an NPC that hands work out, as a component.
//
// The same thing /lib/monster/quest-giver.c is for a plain monster, for an NPC
// built out of components: a game made of locations declares it in the template's
// "components" and the quests it deals in as component attributes. Everything it
// is asked goes to handler("quests"); the component decides nothing.
//
// The NPC object proxies what the outside world asks (query_quest_object,
// check_player, check_player_finished, the quest lists) down to here -- see
// /lib/npc.c -- so the quests command and the map treat a component NPC and a
// monster exactly alike.

#include <living/quests.h>

inherit component "/lib/npc/component.c";

private string * offered;
private string * taken;
// race ids -- the basename of the race file -- this one will talk to. Empty
// means anybody.
private string * races;

void create()
{
  component::create();

  offered = ({ });
  taken = ({ });
  races = ({ });
}

void offers_quests(string id) { offered += ({ id }); }
void takes_quests(string id) { taken += ({ id }); }
void deals_with_races(string * list) { races = list; }

string * query_offered_quests() { return offered; }
string * query_taken_quests() { return taken; }
string * query_dealt_races() { return races; }

// The race of a living as a stable id: the name of the file its race object
// lives in, which is not translated.
private string race_id_of(object who)
{
  string path;
  string * parts;

  path = who->query_race_ob();

  if (!stringp(path) || !strlen(path))
    return "";

  parts = explode(path, "/");

  if (!sizeof(parts))
    return "";

  path = parts[sizeof(parts) - 1];

  // the path may or may not carry the extension
  if (strlen(path) > 2 && path[strlen(path) - 2 ..] == ".c")
    path = path[0 .. strlen(path) - 3];

  return path;
}

// Whether the NPC this belongs to talks to somebody at all.
int deals_with(object who)
{
  if (!sizeof(races))
    return 1;

  return member_array(race_id_of(who), races) != -1;
}

// The handler is resolved against whoever is asking: an npc of a game built on
// locations is a clone of /lib/npc and belongs to no game by its path, while
// the player is in the game whose quests are at stake.
string * quests_for(object who)
{
  if (!deals_with(who))
    return ({ });

  return handler(QUESTS_HANDLER, who)->takeable(who, offered);
}

string * quests_to_hand_in(object who)
{
  return handler(QUESTS_HANDLER, who)->handable(who, taken);
}

// Whoever asks got this component back rather than the npc, so it answers with
// its owner's name: to a player, the one offering the work is the npc.
string query_name() { return query_owner() ? query_owner()->query_name() : ""; }
string query_cap_name()
{
  return query_owner() ? query_owner()->query_cap_name() : "";
}

int query_quest_object() { return 1; }

int check_player(object who)
{
  if (!who)
    return 0;

  return sizeof(quests_for(who)) > 0;
}

int check_player_finished(object who)
{
  if (!who)
    return 0;

  return sizeof(quests_to_hand_in(who)) > 0;
}

// What it deals in travels with the NPC, so a quest giver stays one across a
// reboot without the template having to be read again.
mapping query_auto_load_attributes()
{
  return component::query_auto_load_attributes() +
    ([ "quest_offered" : offered,
       "quest_taken"   : taken,
       "quest_races"   : races ]);
}

void init_auto_load_attributes(mapping args)
{
  component::init_auto_load_attributes(args);

  if (!undefinedp(args["quest_offered"]))
    offered = args["quest_offered"];
  if (!undefinedp(args["quest_taken"]))
    taken = args["quest_taken"];
  if (!undefinedp(args["quest_races"]))
    races = args["quest_races"];
}
