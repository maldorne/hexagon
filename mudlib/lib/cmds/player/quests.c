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
#include <user/quests.h>
#include <language.h>

inherit CMD_BASE;

// What one numbered line of the listing stands for.
#define ENTRY_KIND     0   // ENTRY_OFFER or ENTRY_MINE
#define ENTRY_ID       1   // the quest id
#define ENTRY_CREATURE 2   // who offers it, or who takes it back here; nil if nobody

#define ENTRY_OFFER 1
#define ENTRY_MINE  2
#define ENTRY_DONE  3

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
    if (member_array(id, quests->giver_of(givers[i])->quests_to_complete(me)) != -1)
      return givers[i];

  return nil;
}

// How many quests each of these creatures would offer right now. Handing one in
// can open the next of a chain, and that is how the difference is noticed.
private int * offer_counts(object me, object * givers)
{
  object quests;
  int * out;
  int i;

  quests = handler(QUESTS_HANDLER, me);
  out = allocate_int(sizeof(givers));

  for (i = 0; i < sizeof(givers); i++)
    out[i] = sizeof(quests->giver_of(givers[i])->quests_for(me));

  return out;
}

/*
 * Everything the listing numbers, in the order it shows it: first what the
 * givers here offer, grouped by giver, then the quests being done. The numbers
 * are worked out at the moment and shared by both lists, so the same number means
 * the same line in the listing and in every subcommand.
 */
private mixed * entries(object me)
{
  object quests, quest;
  object * givers;
  mixed * out;
  string * ids, * chains, * steps;
  string chain;
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

  // completed: not in the listing of what is going on, but numbered all the
  // same, so 'misiones hechas' and 'misiones info' agree on the numbers. In the
  // order they are shown there: the loose ones, then each chain by its steps.
  ids = map_indices(me->query_completed_quests(game_name(me)));
  chains = ({ });

  for (i = 0; i < sizeof(ids); i++)
  {
    quest = quests->query_quest(ids[i]);
    chain = quest ? quest->query_chain() : "";

    if (strlen(chain))
    {
      if (member_array(chain, chains) == -1)
        chains += ({ chain });
      continue;
    }

    out += ({ ({ ENTRY_DONE, ids[i], nil }) });
  }

  for (i = 0; i < sizeof(chains); i++)
  {
    steps = quests->query_chain_steps(chains[i]);

    for (j = 0; j < sizeof(steps); j++)
      if (member_array(steps[j], ids) != -1)
        out += ({ ({ ENTRY_DONE, steps[j], nil }) });
  }

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

  if (handler(QUESTS_HANDLER, me)->objectives_met(me, id))
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

// The commands that can be typed, one under the other, lined up after the
// lead word, the last one closing the sentence.
private string hints_text(string * hints)
{
  string lead;

  lead = _LANG_CMD_QUESTS_HINT_LEAD;

  return lead + implode(hints, ",\n" + sprintf("%*s", strlen(lead), "")) + ".\n";
}

private int list_quests(object me)
{
  object quests, quest, creature, last_giver;
  mixed * lines;
  string * hints;
  string text, which;
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

    if (lines[index - 1][ENTRY_KIND] == ENTRY_DONE)
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
      text += _LANG_CMD_QUESTS_COMPLETE_HERE;
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
    which = _LANG_CMD_QUESTS_ANY_NUMBER;
    hints = ({ _LANG_CMD_QUESTS_HINT_INFO });
    if (offers)
      hints += ({ _LANG_CMD_QUESTS_HINT_ACCEPT });
    if (ready)
      hints += ({ _LANG_CMD_QUESTS_HINT_COMPLETE });
    if (mine)
      hints += ({ _LANG_CMD_QUESTS_HINT_ABANDON });

    text += "\n" + hints_text(hints);
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

/*
 * The chain a quest is a step of: what the whole thing is for, and every step in
 * order, saying which are completed, which is this one and which are still to
 * come. Empty for a quest that stands alone.
 */
private string chain_block(object me, object quest)
{
  object quests, step_quest;
  string * ids;
  string chain, chain_title, chain_description, text, mark;
  int j;

  chain = quest->query_chain();

  if (!strlen(chain))
    return "";

  quests = handler(QUESTS_HANDLER, me);
  chain_title = quests->query_chain_title(chain);
  chain_description = quests->query_chain_description(chain);
  ids = quests->query_chain_steps(chain);

  text = "\n" + _LANG_CMD_QUESTS_CHAIN_INTRO + _LANG_CMD_QUESTS_CHAIN_TITLE;

  if (strlen(chain_description))
    text += chain_description;

  text += "\n" + _LANG_CMD_QUESTS_CHAIN_STEPS_INTRO;

  for (j = 0; j < sizeof(ids); j++)
  {
    step_quest = quests->query_quest(ids[j]);

    if (!step_quest)
      continue;

    if (me->has_completed_quest(game_name(me), ids[j]))
      mark = _LANG_CMD_QUESTS_STEP_DONE;
    else if (ids[j] == quest->query_id())
      mark = _LANG_CMD_QUESTS_STEP_HERE;
    else
      mark = _LANG_CMD_QUESTS_STEP_TO_COME;

    text += _LANG_CMD_QUESTS_CHAIN_STEP;
  }

  return text;
}

// The number a completed quest carries in the shared listing, or zero.
private int only_index_of_id(mixed * lines, string id)
{
  int i;

  for (i = 0; i < sizeof(lines); i++)
    if (lines[i][ENTRY_KIND] == ENTRY_DONE && lines[i][ENTRY_ID] == id)
      return i + 1;

  return 0;
}

// The quests already completed, the ones in a chain grouped under its name, with
// the numbers of the shared listing so 'misiones info' takes them too.
private int list_done(object me)
{
  object quests, quest;
  mapping done;
  mixed * lines;
  string * chains, * steps;
  string text, chain, chain_title, id;
  int index, j, k, shown;

  quests = handler(QUESTS_HANDLER, me);
  done = me->query_completed_quests(game_name(me));
  lines = entries(me);
  text = "";
  chains = ({ });
  shown = 0;

  // loose quests first, and the chains each under its own name below
  for (index = 1; index <= sizeof(lines); index++)
  {
    if (lines[index - 1][ENTRY_KIND] != ENTRY_DONE)
      continue;

    id = lines[index - 1][ENTRY_ID];
    quest = quests->query_quest(id);

    if (!quest)
      continue;

    chain = quest->query_chain();

    if (strlen(chain))
    {
      if (member_array(chain, chains) == -1)
        chains += ({ chain });
      continue;
    }

    text += _LANG_CMD_QUESTS_DONE_ENTRY;
    shown++;
  }

  for (j = 0; j < sizeof(chains); j++)
  {
    chain = chains[j];
    chain_title = quests->query_chain_title(chain);
    text += (strlen(text) ? "\n" : "") + _LANG_CMD_QUESTS_CHAIN_TITLE;

    // in the order the chain declares, not the order the savefile keeps them
    steps = quests->query_chain_steps(chain);

    for (k = 0; k < sizeof(steps); k++)
    {
      index = only_index_of_id(lines, steps[k]);

      if (!index)
        continue;

      id = steps[k];
      quest = quests->query_quest(id);

      if (!quest)
        continue;

      text += _LANG_CMD_QUESTS_DONE_ENTRY;
      shown++;
    }
  }

  if (!shown)
    text = _LANG_CMD_QUESTS_DONE_NONE;

  tell_object(me, handler("frames")->frame(text, _LANG_CMD_QUESTS_DONE_HEADER,
                                           this_user()->query_cols()));
  return 1;
}

private int show_info(object me, int index)
{
  object quests, quest;
  mixed * line;
  mapping * objectives;
  int * progress;
  string * hints;
  string text, which;
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

  text = quest->query_description();
  if (strlen(text) && text[strlen(text) - 1] != '\n')
    text += "\n";

  which = "" + index;

  hints = ({ });

  if (line[ENTRY_KIND] == ENTRY_OFFER)
    hints = ({ _LANG_CMD_QUESTS_HINT_ACCEPT });
  else if (line[ENTRY_KIND] == ENTRY_MINE)
  {
    // one line per objective, with the count, for a quest being done
    objectives = quest->query_objectives();
    progress = me->query_progress(game_name(me), line[ENTRY_ID]);

    if (sizeof(objectives))
      text += "\n";

    for (j = 0; j < sizeof(objectives); j++)
      text += _LANG_CMD_QUESTS_OBJECTIVE_LINE;

    if (line[ENTRY_CREATURE])
      hints += ({ _LANG_CMD_QUESTS_HINT_COMPLETE });
    hints += ({ _LANG_CMD_QUESTS_HINT_ABANDON });
  }

  text += chain_block(me, quest);

  if (sizeof(hints))
    text += "\n" + hints_text(hints);

  tell_object(me, handler("frames")->frame(text, quest->query_title(),
                                           this_user()->query_cols()));
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

private int complete_quest(object me, int index)
{
  mixed * line;
  object * givers;
  object creature;
  int * before, * after;
  int i;

  if (!index)
    index = only_index_of(me, ENTRY_MINE, 1);

  line = entry_at(me, index);

  if (!line || line[ENTRY_KIND] != ENTRY_MINE || !line[ENTRY_CREATURE])
  {
    notify_fail(index ? _LANG_CMD_QUESTS_CANNOT_COMPLETE : _LANG_CMD_QUESTS_WHICH);
    return 0;
  }

  // whoever is here may have had nothing to offer until this was completed
  givers = givers_here(me);
  before = offer_counts(me, givers);

  handler(QUESTS_HANDLER, me)->complete(me, line[ENTRY_ID]);

  after = offer_counts(me, givers);

  for (i = 0; i < sizeof(givers); i++)
    if (after[i] > before[i])
    {
      creature = givers[i];
      tell_object(me, _LANG_CMD_QUESTS_NEW_OFFERS);
    }

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

  if (member_array(option, _LANG_CMD_QUESTS_DONE_OPTIONS) != -1)
    return list_done(me);

  if (member_array(option, _LANG_CMD_QUESTS_INFO_OPTIONS) != -1)
    return show_info(me, index);

  if (member_array(option, _LANG_CMD_QUESTS_ACCEPT_OPTIONS) != -1)
    return accept_quest(me, index);

  if (member_array(option, _LANG_CMD_QUESTS_COMPLETE_OPTIONS) != -1)
    return complete_quest(me, index);

  if (member_array(option, _LANG_CMD_QUESTS_ABANDON_OPTIONS) != -1)
    return abandon_quest(me, index);

  notify_fail(_LANG_CMD_QUESTS_SYNTAX + "\n");
  return 0;
}
