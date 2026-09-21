// quest-giver.c -- a creature that hands work out, as a mixin.
//
// Inherited by the file of any monster meant to offer quests, which is how a
// game made of rooms gets a source: a plain monster has no component host, since
// /lib/monster.c does not inherit /lib/npc.c. Games built on locations use the
// npc component instead; both end up calling the same handler.
//
// The giver decides nothing. It says which quests it deals in and, if it is
// picky, which races it deals with; every answer about whether somebody may take
// or hand in a quest comes from handler("quests").
//
//   offers_quests("<game>:kill-the-wasps");
//   deals_with_races(({ "human" }));   // by race file, not by its name

#include <living/quests.h>
#include <language.h>

private string * offered;
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

// The quests this giver would offer somebody right now.
string * quests_for(object who)
{
  object quests;
  string * out;
  int i;

  out = ({ });

  if (!deals_with(who))
    return out;

  quests = handler(QUESTS_HANDLER, this_object());

  for (i = 0; i < sizeof(query_offered_quests()); i++)
    if (quests->check_can_take(who, query_offered_quests()[i]) == QUEST_OK)
      out += ({ query_offered_quests()[i] });

  return out;
}

// The quests somebody is carrying that this giver takes back.
string * quests_to_hand_in(object who)
{
  object quests;
  string * out;
  int i;

  out = ({ });
  quests = handler(QUESTS_HANDLER, this_object());

  for (i = 0; i < sizeof(query_offered_quests()); i++)
    if (quests->check_can_hand_in(who, query_offered_quests()[i]) == QUEST_OK)
      out += ({ query_offered_quests()[i] });

  return out;
}

void giver_init()
{
  add_action("do_ask_work", _LANG_GIVER_ASK_VERBS);
  add_action("do_accept_work", _LANG_GIVER_ACCEPT_VERBS);
  add_action("do_hand_in_work", _LANG_GIVER_HAND_IN_VERBS);
}

// What this one has to say about work, which is also how somebody learns there
// is any.
int do_ask_work(string str)
{
  object who, quests, quest;
  string * available, * ready;
  int i;

  who = this_player();
  quests = handler(QUESTS_HANDLER, this_object());

  if (!deals_with(who))
  {
    tell_object(who, _LANG_GIVER_NOT_YOUR_KIND);
    return 1;
  }

  ready = quests_to_hand_in(who);

  for (i = 0; i < sizeof(ready); i++)
  {
    quest = quests->query_quest(ready[i]);
    tell_object(who, _LANG_GIVER_COME_BACK_DONE);
  }

  available = quests_for(who);

  if (!sizeof(available) && !sizeof(ready))
  {
    tell_object(who, _LANG_GIVER_NOTHING_TO_OFFER);
    return 1;
  }

  for (i = 0; i < sizeof(available); i++)
  {
    quest = quests->query_quest(available[i]);
    tell_object(who, _LANG_GIVER_OFFER);
  }

  return 1;
}

int do_accept_work(string str)
{
  object who, quests;
  string * available;

  who = this_player();
  quests = handler(QUESTS_HANDLER, this_object());
  available = quests_for(who);

  if (!sizeof(available))
  {
    notify_fail(_LANG_GIVER_NOTHING_TO_ACCEPT);
    return 0;
  }

  quests->accept(who, available[0], this_object()->query_name());

  return 1;
}

int do_hand_in_work(string str)
{
  object who, quests;
  string * ready;

  who = this_player();
  quests = handler(QUESTS_HANDLER, this_object());
  ready = quests_to_hand_in(who);

  if (!sizeof(ready))
  {
    notify_fail(_LANG_GIVER_NOTHING_TO_HAND_IN);
    return 0;
  }

  quests->hand_in(who, ready[0]);

  return 1;
}
