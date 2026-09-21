// quest-giver.c -- what a creature that hands work out knows and answers.
//
// Shared the way the shop code under /lib/ventures/ is shared by a shop room
// and a shop location component: a monster in a game made of rooms inherits
// this directly, and the npc component /lib/npc/components/quest-giver.c
// inherits it too, for npcs in a game made of locations. One implementation,
// two carriers; handler("quests") is what knows which of the two it is
// talking to.
//
// The giver decides nothing and owns no verbs: it says which quests it deals in
// and, if it is picky, which races it deals with. The quests command is what
// finds the givers standing in a room and asks them, and every answer about
// whether somebody may take or hand in a quest comes from handler("quests").
//
//   offers_quests("<game>:kill-the-wasps");
//   takes_quests("<game>:kill-the-wasps");   // who it is handed back to
//   deals_with_races(({ "human" }));         // by race file, not by its name
//
// Offering and taking back are declared apart because they are not always the
// same creature: one hands the work out and another is the one to be found.

#include <living/quests.h>
#include <language.h>

private string * offered;
private string * taken;
// race ids -- the basename of the race file -- this one will talk to. Empty
// means anybody.
private string * races;

// The lists start empty at first use: this is a mixin inherited under a label,
// so its create() is not the one the object runs.
void offers_quests(string id)
{
  if (!offered)
    offered = ({ });

  offered += ({ id });
}

string * query_offered_quests() { return offered ? offered : ({ }); }
void set_offered_quests(string * list) { offered = list; }

void takes_quests(string id)
{
  if (!taken)
    taken = ({ });

  taken += ({ id });
}

string * query_taken_quests() { return taken ? taken : ({ }); }
void set_taken_quests(string * list) { taken = list; }

void deals_with_races(string * list) { races = list; }
string * query_dealt_races() { return races ? races : ({ }); }

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

// Whether this giver talks to somebody at all.
int deals_with(object who)
{
  if (!sizeof(query_dealt_races()))
    return 1;

  return member_array(race_id_of(who), query_dealt_races()) != -1;
}

// The quests this giver would offer somebody right now, and the ones it takes
// back: the lists are ours, the decisions are the handler's. The handler is the
// one of the game of whoever is asking, since those are the quests at stake.
string * quests_for(object who)
{
  if (!deals_with(who))
    return ({ });

  return handler(QUESTS_HANDLER, who)->takeable(who, query_offered_quests());
}

string * quests_to_hand_in(object who)
{
  return handler(QUESTS_HANDLER, who)->handable(who, query_taken_quests());
}

// ---------------------------------------------------------------------------
// What a player sees before asking
// ---------------------------------------------------------------------------
//
// Somebody has to be able to tell, by looking, that there is work here. The
// room listing (/lib/core/basic/contents.c) and the map
// (/lib/handlers/cartography.c) both mark a giver by asking these three: a
// yellow [!] when it has something to offer the one looking, a yellow [?] when
// they can hand something in.

int query_quest_object() { return 1; }

// Has something for this one to take.
int check_player(object who)
{
  if (!who)
    return 0;

  return sizeof(quests_for(who)) > 0;
}

// This one can hand something in here.
int check_player_finished(object who)
{
  if (!who)
    return 0;

  return sizeof(quests_to_hand_in(who)) > 0;
}
