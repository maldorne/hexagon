/*
 * quests.c -- the one place that decides anything about a quest.
 *
 * Quest system for CcMud, neverbot 03/09: the checks lived in a mixin every
 *   provider inherited (global/basic/quests.c), and the rest in the questman
 * Rewritten for Hexagon, neverbot 09/2026: one handler decides, and the sources
 *   only call it
 *
 * Quests are offered from several places: an npc, a monster, a location, a book,
 * or an event with no object behind it. None of them decides: they call offer(),
 * accept(), hand_in() or abandon() here, and this handler is what checks whether
 * the player may, moves the player's store on and pays the rewards.
 *
 * A game has its own handler inheriting this one, and registers the quests it
 * ships in its create() with add_quest(). Quests are addressed by their id,
 * never by the path of the file that defines them.
 *
 * Rewards are granted here, from the closed vocabulary in <living/quests.h>, and
 * only once a hand-in has been accepted. A quest never hands out anything by
 * itself.
 */

#include <living/quests.h>
#include <living/skills.h>
#include <living/titles.h>
#include <language.h>

// id -> the path of the file that defines it
private mapping quest_paths;

// prototypes
object query_quest(string id);
int query_objective_progress(object who, string id, int which);
private void pay_item(object who, string path);

void create()
{
  quest_paths = ([ ]);
}

// A game registers each quest it ships. The id comes from the definition, so a
// quest cannot be registered under a name it does not answer to.
void add_quest(string path)
{
  object quest;

  catch(quest = load_object(path));

  if (!quest)
  {
    log_file(QUESTS_LOG, "cannot load " + path + "\n");
    return;
  }

  if (!strlen(quest->query_id()))
  {
    log_file(QUESTS_LOG, "no id in " + path + "\n");
    return;
  }

  quest_paths[quest->query_id()] = path;
}

string * query_quest_ids() { return map_indices(quest_paths); }

// The definition of a quest, or nil if this game has no such quest.
object query_quest(string id)
{
  object quest;

  if (!quest_paths[id])
    return nil;

  catch(quest = load_object(quest_paths[id]));

  return quest;
}

// Which game a player's quests belong to. Taken from the player, so the same
// character keeps separate progress per game.
private string game_of(object who)
{
  return game_name(who);
}

/*
 * Whether a player may take a quest. Returns one of the QUEST_* answers, so a
 * caller can tell a player why rather than only that they cannot.
 */
int check_can_take(object who, string id)
{
  object quest;
  string game;
  int i;
  string * needs;

  quest = query_quest(id);

  if (!quest)
    return QUEST_UNKNOWN;

  game = game_of(who);

  if (who->is_doing_quest(game, id))
    return QUEST_ALREADY;

  // a quest that is not repeatable is taken once and never again
  if (who->has_done_quest(game, id) && !quest->query_repeatable())
    return QUEST_DONE;

  if (quest->query_needs_level() &&
      who->query_level() < quest->query_needs_level())
    return QUEST_NEEDS;

  needs = quest->query_needs_quests();

  for (i = 0; i < sizeof(needs); i++)
    if (!who->has_done_quest(game, needs[i]))
      return QUEST_NEEDS;

  if (!quest->can_take(who))
    return QUEST_NEEDS;

  return QUEST_OK;
}

// Whether every objective of a quest the player is doing has been met.
int is_complete(object who, string id)
{
  object quest;
  mapping * objectives;
  int * progress;
  int i;

  quest = query_quest(id);
  progress = who->query_progress(game_of(who), id);

  if (!quest || !progress)
    return 0;

  objectives = quest->query_objectives();

  for (i = 0; i < sizeof(objectives); i++)
    if (progress[i] < objectives[i][OBJ_COUNT])
      return 0;

  return 1;
}

int check_can_hand_in(object who, string id)
{
  object quest;

  quest = query_quest(id);

  if (!quest)
    return QUEST_UNKNOWN;

  if (!who->is_doing_quest(game_of(who), id))
    return QUEST_UNKNOWN;

  if (!is_complete(who, id))
    return QUEST_INCOMPLETE;

  if (!quest->can_hand_in(who))
    return QUEST_INCOMPLETE;

  return QUEST_OK;
}

/*
 * Who answers for an object in matters of quests, or nil when nobody does. A
 * monster carries the giver mixin and answers for itself; an npc built out of
 * components answers through the one that deals in quests. Everything that wants
 * to know -- the room listing, the map, the quests command -- asks this and then
 * talks to whatever comes back, so neither /lib/npc.c nor any caller has to know
 * which of the two it is looking at.
 */
object giver_of(object ob)
{
  object component;

  if (!ob)
    return nil;

  component = ob->query_component_by_type("quest-giver");

  if (component)
    return component;

  if (ob->query_quest_object())
    return ob;

  return nil;
}

// Of a list of quest ids, the ones somebody could take right now. Sources hold
// the list; deciding is this handler's job, so both the monster mixin and the npc
// component ask here instead of each filtering on its own.
string * takeable(object who, string * ids)
{
  string * out;
  int i;

  out = ({ });

  for (i = 0; i < sizeof(ids); i++)
    if (check_can_take(who, ids[i]) == QUEST_OK)
      out += ({ ids[i] });

  return out;
}

// Of a list of quest ids, the ones somebody could hand in right now.
string * handable(object who, string * ids)
{
  string * out;
  int i;

  out = ({ });

  for (i = 0; i < sizeof(ids); i++)
    if (check_can_hand_in(who, ids[i]) == QUEST_OK)
      out += ({ ids[i] });

  return out;
}

/*
 * Give a player a quest. `source` is whoever handed it over, remembered so the
 * log can say where it came from. Returns a QUEST_* answer.
 */
int accept(object who, string id, varargs string source)
{
  object quest;
  int answer;

  answer = check_can_take(who, id);

  if (answer != QUEST_OK)
    return answer;

  quest = query_quest(id);

  if (!source)
    source = "";

  who->start_quest(game_of(who), id, quest->query_objective_count(), source);

  tell_object(who, _LANG_QUEST_ACCEPTED);

  return QUEST_OK;
}

void abandon(object who, string id)
{
  object quest;

  if (!who->is_doing_quest(game_of(who), id))
    return;

  quest = query_quest(id);

  who->abandon_quest(game_of(who), id);

  if (quest)
    tell_object(who, _LANG_QUEST_ABANDONED);
}

/*
 * Pay one reward. Every kind the system knows is here and nowhere else, so a
 * quest cannot invent one, and nothing is paid before the hand-in is accepted.
 */
private void pay_reward(object who, mapping reward)
{
  mixed value;

  value = reward[REWARD_VALUE];

  switch (reward[REWARD_KIND])
  {
    case REWARD_XP:
      who->adjust_xp(value);
      break;

    case REWARD_JOB_XP:
      who->adjust_job_xp(value);
      break;

    // ({ amount, currency })
    case REWARD_MONEY:
      who->adjust_money(value[0], value[1]);
      break;

    case REWARD_ITEM:
      pay_item(who, value);
      break;

    case REWARD_TITLE:
      handler(TITLES_HANDLER, who)->grant(who, value);
      break;

    case REWARD_SKILL:
      who->add_known_skill(value);
      break;
  }
}

// An item reward is a path, cloned into the player. It is dropped where they
// stand when they cannot carry it, rather than lost.
private void pay_item(object who, string path)
{
  object item;

  catch(item = clone_object(path));

  if (!item)
  {
    log_file(QUESTS_LOG, "cannot clone the item of a reward: " + path + "\n");
    return;
  }

  if (item->move(who))
    item->move(environment(who));
}

/*
 * Take a finished quest back. Checks first, because a hand-in that is refused
 * must pay nothing, then pays every reward and writes the quest down as done.
 */
int hand_in(object who, string id)
{
  object quest;
  mapping * rewards;
  int answer, i;

  answer = check_can_hand_in(who, id);

  if (answer != QUEST_OK)
    return answer;

  quest = query_quest(id);
  rewards = quest->query_rewards();

  who->finish_quest(game_of(who), id);

  for (i = 0; i < sizeof(rewards); i++)
    pay_reward(who, rewards[i]);

  tell_object(who, _LANG_QUEST_FINISHED);

  return QUEST_OK;
}

/*
 * Progress. Every event that can advance a quest ends up here: the handler walks
 * the player's active quests and credits the objectives whose target matches,
 * so no caller has to know what anybody is doing.
 */
private void advance(object who, string kind, string target, int amount)
{
  string game;
  string * ids;
  object quest;
  mapping * objectives;
  int i, j, before, after;

  game = game_of(who);
  ids = map_indices(who->query_active_quests(game));

  for (i = 0; i < sizeof(ids); i++)
  {
    quest = query_quest(ids[i]);

    if (!quest)
      continue;

    objectives = quest->query_objectives();

    for (j = 0; j < sizeof(objectives); j++)
    {
      if (objectives[j][OBJ_KIND] != kind ||
          objectives[j][OBJ_TARGET] != target)
        continue;

      before = query_objective_progress(who, ids[i], j);
      after = who->adjust_progress(game, ids[i], j, amount,
                                   objectives[j][OBJ_COUNT]);

      if (after == before)
        continue;

      if (after >= objectives[j][OBJ_COUNT])
        tell_object(who, _LANG_QUEST_OBJECTIVE_DONE);
      else
        tell_object(who, _LANG_QUEST_OBJECTIVE_PROGRESS);
    }
  }
}

int query_objective_progress(object who, string id, int which)
{
  int * progress;

  progress = who->query_progress(game_of(who), id);

  if (!progress || which < 0 || which >= sizeof(progress))
    return 0;

  return progress[which];
}

// Something died. Credit goes to everybody who was fighting it, not only to
// whoever landed the last blow.
void killed(object * attackers, string target)
{
  int i;

  for (i = 0; i < sizeof(attackers); i++)
    if (attackers[i] && attackers[i]->query_player())
      advance(attackers[i], OBJECTIVE_KILL, target, 1);
}

// A player reached a place.
void reached(object who, string where)
{
  advance(who, OBJECTIVE_REACH, where, 1);
}

// A player talked to somebody.
void talked(object who, string target)
{
  advance(who, OBJECTIVE_TALK, target, 1);
}

// A player picked something up, or parted with it: an amount of -1 is what takes
// the counter back down.
void carried(object who, string target, int amount)
{
  advance(who, OBJECTIVE_GET, target, amount);
}
