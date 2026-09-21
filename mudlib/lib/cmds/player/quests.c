/* ************************************************************
 *
 *   Quest system for CcMud, neverbot 03/09
 *
 *   Rewritten for Hexagon, neverbot 09/2026: one verb for everything, the
 *   givers standing in the room are found by the command instead of binding
 *   verbs of their own, and every decision is asked of handler("quests").
 *
 * *********************************************************** */

#include <mud/cmd.h>
#include <living/quests.h>
#include <language.h>

inherit CMD_BASE;

void setup()
{
  set_aliases(_LANG_CMD_QUESTS_ALIAS);
  set_usage(_LANG_CMD_QUESTS_SYNTAX);
  set_help(_LANG_CMD_QUESTS_HELP);
}

// Whoever in this room deals in quests at all.
private object * givers_here(object me)
{
  object * here, * out;
  int i;

  out = ({ });

  if (!environment(me))
    return out;

  here = all_inventory(environment(me));

  // anything that is not a giver answers nil to both
  for (i = 0; i < sizeof(here); i++)
    if (arrayp(here[i]->query_offered_quests()) ||
        arrayp(here[i]->query_taken_quests()))
      out += ({ here[i] });

  return out;
}

// What can be taken here, in the order the listing shows it.
private mixed * offers_here(object me)
{
  object * givers;
  mixed * out;
  string * ids;
  int i, j;

  out = ({ });
  givers = givers_here(me);

  for (i = 0; i < sizeof(givers); i++)
  {
    ids = givers[i]->quests_for(me);

    for (j = 0; j < sizeof(ids); j++)
      out += ({ ({ givers[i], ids[j] }) });
  }

  return out;
}

// What can be handed in here, in the order the listing shows it.
private mixed * hand_ins_here(object me)
{
  object * givers;
  mixed * out;
  string * ids;
  int i, j;

  out = ({ });
  givers = givers_here(me);

  for (i = 0; i < sizeof(givers); i++)
  {
    ids = givers[i]->quests_to_hand_in(me);

    for (j = 0; j < sizeof(ids); j++)
      out += ({ ({ givers[i], ids[j] }) });
  }

  return out;
}

// The quests being done, as a list, so a number in the listing means the same
// thing in every subcommand that works on them.
private string * my_quests(object me)
{
  return map_indices(me->query_active_quests(game_name(me)));
}

// One line per objective: what it asks for and where the count stands.
private string objectives_of(object quest, object me, string id)
{
  mapping * objectives;
  int * progress;
  string text;
  int j;

  objectives = quest->query_objectives();
  progress = me->query_progress(game_name(me), id);
  text = "";

  for (j = 0; j < sizeof(objectives); j++)
    text += _LANG_CMD_QUESTS_OBJECTIVE_LINE;

  return text;
}

private int list_quests(object me)
{
  object quests, quest, giver;
  mixed * offers, * ready;
  string * mine;
  string text;
  int i;

  quests = handler(QUESTS_HANDLER, me);
  mine = my_quests(me);
  offers = offers_here(me);
  ready = hand_ins_here(me);
  text = "";

  if (!sizeof(mine))
    text += _LANG_CMD_QUESTS_NONE;
  else
  {
    text += _LANG_CMD_QUESTS_YOURS;

    for (i = 0; i < sizeof(mine); i++)
    {
      quest = quests->query_quest(mine[i]);

      if (!quest)
        continue;

      text += _LANG_CMD_QUESTS_LINE;
      text += objectives_of(quest, me, mine[i]);
    }
  }

  // what the people standing here will do about quests, which is the only way
  // to learn a quest exists at all
  for (i = 0; i < sizeof(ready); i++)
  {
    giver = ready[i][0];
    quest = quests->query_quest(ready[i][1]);
    text += _LANG_CMD_QUESTS_CAN_HAND_IN;
  }

  for (i = 0; i < sizeof(offers); i++)
  {
    giver = offers[i][0];
    quest = quests->query_quest(offers[i][1]);
    text += _LANG_CMD_QUESTS_OFFERED;
  }

  tell_object(me, handler("frames")->frame(text, _LANG_CMD_QUESTS_TITLE,
                                           this_user()->query_cols()));
  return 1;
}

private int show_info(object me, int which)
{
  object quests, quest;
  string * mine;

  mine = my_quests(me);

  if (which < 1 || which > sizeof(mine))
  {
    notify_fail(_LANG_CMD_QUESTS_NO_SUCH);
    return 0;
  }

  quests = handler(QUESTS_HANDLER, me);
  quest = quests->query_quest(mine[which - 1]);

  if (!quest)
  {
    notify_fail(_LANG_CMD_QUESTS_NO_SUCH);
    return 0;
  }

  tell_object(me, _LANG_CMD_QUESTS_INFO +
                  objectives_of(quest, me, mine[which - 1]));
  return 1;
}

private int accept_quest(object me, int which)
{
  mixed * offers;

  offers = offers_here(me);

  if (!sizeof(offers))
  {
    notify_fail(_LANG_CMD_QUESTS_NOTHING_OFFERED);
    return 0;
  }

  // one offer needs no number
  if (!which && sizeof(offers) == 1)
    which = 1;

  if (which < 1 || which > sizeof(offers))
  {
    notify_fail(_LANG_CMD_QUESTS_WHICH_OFFER);
    return 0;
  }

  handler(QUESTS_HANDLER, me)->accept(me, offers[which - 1][1],
                                      offers[which - 1][0]->query_name());
  return 1;
}

private int hand_in_quest(object me, int which)
{
  mixed * ready;

  ready = hand_ins_here(me);

  if (!sizeof(ready))
  {
    notify_fail(_LANG_CMD_QUESTS_NOTHING_TO_HAND_IN);
    return 0;
  }

  if (!which && sizeof(ready) == 1)
    which = 1;

  if (which < 1 || which > sizeof(ready))
  {
    notify_fail(_LANG_CMD_QUESTS_WHICH_HAND_IN);
    return 0;
  }

  handler(QUESTS_HANDLER, me)->hand_in(me, ready[which - 1][1]);
  return 1;
}

private int abandon_quest(object me, int which)
{
  string * mine;

  mine = my_quests(me);

  if (which < 1 || which > sizeof(mine))
  {
    notify_fail(_LANG_CMD_QUESTS_NO_SUCH);
    return 0;
  }

  handler(QUESTS_HANDLER, me)->abandon(me, mine[which - 1]);
  return 1;
}

static int cmd(string str, object me, string verb)
{
  string option, rest;
  int which;

  if (!str || !strlen(str))
    return list_quests(me);

  rest = "";

  if (sscanf(str, "%s %s", option, rest) != 2)
    option = str;

  which = 0;
  sscanf(rest, "%d", which);

  if (member_array(option, _LANG_CMD_QUESTS_INFO_OPTIONS) != -1)
    return show_info(me, which);

  if (member_array(option, _LANG_CMD_QUESTS_ACCEPT_OPTIONS) != -1)
    return accept_quest(me, which);

  if (member_array(option, _LANG_CMD_QUESTS_HAND_IN_OPTIONS) != -1)
    return hand_in_quest(me, which);

  if (member_array(option, _LANG_CMD_QUESTS_ABANDON_OPTIONS) != -1)
    return abandon_quest(me, which);

  notify_fail(_LANG_CMD_QUESTS_SYNTAX + "\n");
  return 0;
}
