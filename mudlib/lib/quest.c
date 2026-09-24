/*
 * quest.c -- what one quest asks for and what it pays.
 *
 * Quest system for CcMud, neverbot 03/09
 * Quest system evolution, neverbot 06/14: quests became objects of their own
 * Rewritten for Hexagon, neverbot 09/2026: a list of objectives instead of one
 *   counter, a stable id instead of the file path, and a declared reward list
 *   instead of code in valid_finish()
 *
 * A quest definition is a blueprint and is never cloned: it holds no player
 * state, only the description of the work. A game writes one file per quest
 * under /games/<game>/quests/ inheriting this one, and its quests handler
 * registers them.
 *
 * The id is what a player's savefile remembers, so it is a stable string and
 * carries the game it comes from -- "<game>:kill-the-wasps" -- and stays the
 * same however the file is later moved or renamed.
 *
 * Objectives are a list, each one a mapping:
 *
 *   ([ OBJ_KIND: OBJECTIVE_KILL, OBJ_TARGET: "<template id>", OBJ_COUNT: 5,
 *      OBJ_TEXT: "..." ])
 *
 * Rewards are a list too, and the handler is what grants them:
 *
 *   ([ REWARD_KIND: REWARD_SKILL, REWARD_VALUE: SKILL_CLIMB ])
 *
 * A quest that needs more than a level or a finished quest to be taken, or more
 * than its objectives to be handed in, overrides can_take() or can_hand_in().
 */

inherit "/lib/core/object";

#include <living/quests.h>

private string id;
private string title;
private string description;
// who takes the quest back: a template id, so any npc of that kind will do
private string hand_in;
// where it is handed in, when it is a place rather than somebody
private string hand_in_place;
// what the giver says when the quest is taken
private string accept_message;
// what the taker says when the quest is handed in, beyond the rewards
private string complete_message;
private mapping * objectives;
private mapping * rewards;
// ids of quests that have to be handed in before this one is offered
private string * needs_quests;
// the chain this quest is a step of, and which step it is. The first step is
// also where the chain's own name and text are declared, so a chain needs no
// file of its own.
private string chain;
private int chain_step;
private string chain_title;
private string chain_description;
private int needs_level;
private int repeatable;

void create()
{
  id = "";
  title = "";
  description = "";
  hand_in = "";
  hand_in_place = "";
  accept_message = "";
  complete_message = "";
  objectives = ({ });
  rewards = ({ });
  needs_quests = ({ });
  needs_level = 0;
  chain = "";
  chain_step = 0;
  chain_title = "";
  chain_description = "";
  repeatable = 0;

  // /lib/core/object's create() is what calls setup()
  ::create();
}

void set_id(string str) { id = str; }
string query_id() { return id; }

void set_title(string str) { title = str; }
string query_title() { return title; }

void set_description(string str) { description = str; }
string query_description() { return description; }

void set_hand_in(string str) { hand_in = str; }
string query_hand_in() { return hand_in; }

void set_hand_in_place(string str) { hand_in_place = str; }
string query_hand_in_place() { return hand_in_place; }

void set_accept_message(string str) { accept_message = str; }
string query_accept_message() { return accept_message; }

void set_complete_message(string str) { complete_message = str; }
string query_complete_message() { return complete_message; }

void set_needs_level(int i) { needs_level = i; }
int query_needs_level() { return needs_level; }

void set_repeatable(int i) { repeatable = i; }
int query_repeatable() { return repeatable; }

// Which chain this quest belongs to, and its place in it, counting from one.
void set_chain(string chain_id, int step) { chain = chain_id; chain_step = step; }
string query_chain() { return chain; }
int query_chain_step() { return chain_step; }

// The chain's own name and what it is all for, declared by its first step.
void set_chain_title(string str) { chain_title = str; }
string query_chain_title() { return chain_title; }

void set_chain_description(string str) { chain_description = str; }
string query_chain_description() { return chain_description; }

void add_needs_quest(string quest_id) { needs_quests += ({ quest_id }); }
string * query_needs_quests() { return needs_quests; }

// One thing the player has to do. count defaults to one, and text is what the
// quest log shows for this objective.
void add_objective(string kind, string target, varargs int count, string text)
{
  if (!count)
    count = 1;

  objectives += ({ ([ OBJ_KIND   : kind,
                      OBJ_TARGET : target,
                      OBJ_COUNT  : count,
                      OBJ_TEXT   : text ]) });
}

mapping * query_objectives() { return objectives; }
int query_objective_count() { return sizeof(objectives); }

void add_reward(string kind, mixed value)
{
  rewards += ({ ([ REWARD_KIND : kind, REWARD_VALUE : value ]) });
}

mapping * query_rewards() { return rewards; }

// Extra conditions to be offered this quest, beyond the level and the quests it
// needs, which the handler checks by itself. A quest that wants more overrides
// this and returns zero to refuse.
int can_take(object who)
{
  return 1;
}

// Extra conditions to hand it in, beyond having met every objective.
int can_hand_in(object who)
{
  return 1;
}
