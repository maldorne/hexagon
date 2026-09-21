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

// What one numbered line of the listing stands for.
#define ENTRY_KIND     0   // ENTRY_OFFER or ENTRY_MINE
#define ENTRY_ID       1   // the quest id
#define ENTRY_CREATURE 2   // who offers it, or who takes it back here; nil if nobody

#define ENTRY_OFFER 1
#define ENTRY_MINE  2

void setup()
{
  set_aliases(_LANG_CMD_QUESTS_ALIAS);
  set_usage(_LANG_CMD_QUESTS_SYNTAX);
  set_help(_LANG_CMD_QUESTS_HELP);
}

// The creatures standing here that deal in quests. The creature is what this
// command works with and names; the handler says which object carries its quest
// code -- the creature itself, or the component an npc carries -- and that is
// only asked, never shown.
private object * givers_here(object me)
{
  object * here, * out;
  object quests;
  int i;

  out = ({ });

  if (!environment(me))
    return out;

  quests = handler(QUESTS_HANDLER, me);
  here = all_inventory(environment(me));

  for (i = 0; i < sizeof(here); i++)
    if (quests->giver_of(here[i]))
      out += ({ here[i] });

  return out;
}

// Who standing here takes this quest back, if anybody.
private object taker_here(object me, string id, object * givers)
{
  object quests;
  int i;

  quests = handler(QUESTS_HANDLER, me);

  for (i = 0; i < sizeof(givers); i++)
    if (member_array(id, quests->giver_of(givers[i])->quests_to_hand_in(me)) != -1)
      return givers[i];

  return nil;
}

/*
 * Everything the listing numbers, in the order it shows it: first what the
 * givers here offer, grouped by giver, then the quests being done. The numbers
 * are worked out at the moment and shared by both lists, so the same number means
 * the same line in the listing and in every subcommand.
 */
private mixed * entries(object me)
{
  object quests;
  object * givers;
  mixed * out;
  string * ids;
  int i, j;

  out = ({ });
  quests = handler(QUESTS_HANDLER, me);
  givers = givers_here(me);

  for (i = 0; i < sizeof(givers); i++)
  {
    ids = quests->giver_of(givers[i])->quests_for(me);

    for (j = 0; j < sizeof(ids); j++)
      out += ({ ({ ENTRY_OFFER, ids[j], givers[i] }) });
  }

  ids = map_indices(me->query_active_quests(game_name(me)));

  for (i = 0; i < sizeof(ids); i++)
    out += ({ ({ ENTRY_MINE, ids[i], taker_here(me, ids[i], givers) }) });

  return out;
}

// Where a quest being done stands, to go beside its title: nothing for a quest
// whose only task is to reach somebody, the count for a single objective, how
// many objectives are met when there are several, and finished once all are.
private string progress_note(object me, object quest, string id)
{
  mapping * objectives;
  int * progress;
  int j, met, done, needed, total;

  objectives = quest->query_objectives();
  total = sizeof(objectives);

  if (!total)
    return "";

  if (handler(QUESTS_HANDLER, me)->is_complete(me, id))
    return _LANG_CMD_QUESTS_FINISHED;

  progress = me->query_progress(game_name(me), id);

  if (total == 1)
  {
    done = progress[0];
    needed = objectives[0][OBJ_COUNT];
    return _LANG_CMD_QUESTS_PROGRESS_ONE;
  }

  met = 0;
  for (j = 0; j < total; j++)
    if (progress[j] >= objectives[j][OBJ_COUNT])
      met++;

  return _LANG_CMD_QUESTS_PROGRESS_MANY;
}

private int list_quests(object me)
{
  object quests, quest, creature, last_giver;
  mixed * lines;
  string * hints;
  string text;
  int index, offers, mine, ready;

  quests = handler(QUESTS_HANDLER, me);
  lines = entries(me);
  text = "";
  last_giver = nil;
  offers = mine = ready = 0;

  for (index = 1; index <= sizeof(lines); index++)
  {
    quest = quests->query_quest(lines[index - 1][ENTRY_ID]);
    creature = lines[index - 1][ENTRY_CREATURE];

    if (!quest)
      continue;

    if (lines[index - 1][ENTRY_KIND] == ENTRY_OFFER)
    {
      // one heading per giver, over the quests it offers
      if (creature != last_giver)
      {
        text += _LANG_CMD_QUESTS_OFFERS_FROM;
        last_giver = creature;
      }

      text += _LANG_CMD_QUESTS_ENTRY + "\n";
      offers++;
      continue;
    }

    if (!mine)
      text += (strlen(text) ? "\n" : "") + _LANG_CMD_QUESTS_YOURS;

    text += _LANG_CMD_QUESTS_ENTRY +
            progress_note(me, quest, lines[index - 1][ENTRY_ID]);

    // who standing here takes it back
    if (creature)
    {
      text += _LANG_CMD_QUESTS_HAND_IN_HERE;
      ready++;
    }

    text += "\n";
    mine++;
  }

  if (!offers && !mine)
    text += _LANG_CMD_QUESTS_NONE;

  // what can be typed about what is on the screen, and nothing else
  if (offers || mine)
  {
    hints = ({ _LANG_CMD_QUESTS_HINT_INFO });
    if (offers)
      hints += ({ _LANG_CMD_QUESTS_HINT_ACCEPT });
    if (ready)
      hints += ({ _LANG_CMD_QUESTS_HINT_HAND_IN });
    if (mine)
      hints += ({ _LANG_CMD_QUESTS_HINT_ABANDON });

    text += "\n" + implode(hints, ",\n") + ".\n";
  }

  tell_object(me, handler("frames")->frame(text, _LANG_CMD_QUESTS_TITLE,
                                           this_user()->query_cols()));
  return 1;
}

// The line a number stands for, or nil when there is no such line.
private mixed * entry_at(object me, int index)
{
  mixed * lines;

  lines = entries(me);

  if (index < 1 || index > sizeof(lines))
    return nil;

  return lines[index - 1];
}

// Without a number, the one line of the right kind there is, if there is
// exactly one.
private int only_index_of(object me, int kind, int takeable_here)
{
  mixed * lines;
  int i, found;

  lines = entries(me);
  found = 0;

  for (i = 0; i < sizeof(lines); i++)
  {
    if (lines[i][ENTRY_KIND] != kind)
      continue;
    if (takeable_here && !lines[i][ENTRY_CREATURE])
      continue;
    if (found)
      return 0;
    found = i + 1;
  }

  return found;
}

private int show_info(object me, int index)
{
  object quests, quest;
  mixed * line;
  mapping * objectives;
  int * progress;
  string text;
  int j;

  line = entry_at(me, index);

  if (!line)
  {
    notify_fail(_LANG_CMD_QUESTS_NO_SUCH);
    return 0;
  }

  quests = handler(QUESTS_HANDLER, me);
  quest = quests->query_quest(line[ENTRY_ID]);

  if (!quest)
  {
    notify_fail(_LANG_CMD_QUESTS_NO_SUCH);
    return 0;
  }

  text = _LANG_CMD_QUESTS_INFO;

  // one line per objective, with the count, for a quest being done
  if (line[ENTRY_KIND] == ENTRY_MINE)
  {
    objectives = quest->query_objectives();
    progress = me->query_progress(game_name(me), line[ENTRY_ID]);

    for (j = 0; j < sizeof(objectives); j++)
      text += _LANG_CMD_QUESTS_OBJECTIVE_LINE;
  }

  tell_object(me, wrap(text, this_user()->query_cols()));
  return 1;
}

private int accept_quest(object me, int index)
{
  mixed * line;

  if (!index)
    index = only_index_of(me, ENTRY_OFFER, 0);

  line = entry_at(me, index);

  if (!line || line[ENTRY_KIND] != ENTRY_OFFER)
  {
    notify_fail(index ? _LANG_CMD_QUESTS_NOT_AN_OFFER : _LANG_CMD_QUESTS_WHICH);
    return 0;
  }

  handler(QUESTS_HANDLER, me)->accept(me, line[ENTRY_ID],
                                      line[ENTRY_CREATURE]->query_name());
  return 1;
}

private int hand_in_quest(object me, int index)
{
  mixed * line;

  if (!index)
    index = only_index_of(me, ENTRY_MINE, 1);

  line = entry_at(me, index);

  if (!line || line[ENTRY_KIND] != ENTRY_MINE || !line[ENTRY_CREATURE])
  {
    notify_fail(index ? _LANG_CMD_QUESTS_CANNOT_HAND_IN : _LANG_CMD_QUESTS_WHICH);
    return 0;
  }

  handler(QUESTS_HANDLER, me)->hand_in(me, line[ENTRY_ID]);
  return 1;
}

private int abandon_quest(object me, int index)
{
  mixed * line;

  line = entry_at(me, index);

  if (!line || line[ENTRY_KIND] != ENTRY_MINE)
  {
    notify_fail(index ? _LANG_CMD_QUESTS_NOT_YOURS : _LANG_CMD_QUESTS_WHICH);
    return 0;
  }

  handler(QUESTS_HANDLER, me)->abandon(me, line[ENTRY_ID]);
  return 1;
}

static int cmd(string str, object me, string verb)
{
  string option, rest;
  int index;

  if (!str || !strlen(str))
    return list_quests(me);

  rest = "";

  if (sscanf(str, "%s %s", option, rest) != 2)
    option = str;

  index = 0;
  sscanf(rest, "%d", index);

  if (member_array(option, _LANG_CMD_QUESTS_INFO_OPTIONS) != -1)
    return show_info(me, index);

  if (member_array(option, _LANG_CMD_QUESTS_ACCEPT_OPTIONS) != -1)
    return accept_quest(me, index);

  if (member_array(option, _LANG_CMD_QUESTS_HAND_IN_OPTIONS) != -1)
    return hand_in_quest(me, index);

  if (member_array(option, _LANG_CMD_QUESTS_ABANDON_OPTIONS) != -1)
    return abandon_quest(me, index);

  notify_fail(_LANG_CMD_QUESTS_SYNTAX + "\n");
  return 0;
}
