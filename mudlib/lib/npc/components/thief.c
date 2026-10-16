// thief.c -- an NPC that now and then robs whoever passes through its place.
//
// An arrival only decides whether to try, and schedules the attempt a few
// seconds later; nothing is done inside the event. When the attempt comes
// everything is checked again (the NPC, the victim, the cooldown) and the steal
// skill is fired on the NPC's behalf. The skill rolls the result and tells the
// room; how good the NPC is at it comes from what it knows of the skill.
//
// Attributes, all optional (an attribute is stored only when it differs from
// its default):
//   chance         percent per arrival                           (33)
//   delay          seconds after the arrival, plus random(delay) (4)
//   cooldown       seconds between two attempts of this thief    (300)
//   victims        "players" or "all" (other NPCs too)           ("players")
//   fighting       1: robs victims who are in a fight            (0)
//   what           "coins", "items" or "both"                    ("coins")
//   hidden_only    1: tries only while hidden                    (1)
//   spare          1: spares its own citizenship, if it has one  (0)
//   max_per_victim attempts against one victim, 0 is unlimited   (0)

#include <living/skills.h>
#include <common/properties.h>

inherit component "/lib/npc/component.c";

#define STEAL_SKILL_PATH "/lib/obj/skills/steal.c"

#define DEFAULT_CHANCE 33
#define DEFAULT_DELAY 4
#define DEFAULT_COOLDOWN 300

#define VICTIMS_PLAYERS "players"
#define VICTIMS_ALL     "all"

#define WHAT_COINS "coins"
#define WHAT_ITEMS "items"
#define WHAT_BOTH  "both"

private int chance;
private int delay;
private int cooldown;
private string victims;
private int fighting;
private string what;
private int hidden_only;
private int spare;
private int max_per_victim;

// time() of the last attempt, 0 if none
static int last_attempt;
// the pending attempt, if any, and who it is aimed at
static int attempt_handle;
static object attempt_victim;
// victim name -> attempts made against them
static mapping tries;

void create()
{
  component::create();
  chance = DEFAULT_CHANCE;
  delay = DEFAULT_DELAY;
  cooldown = DEFAULT_COOLDOWN;
  victims = VICTIMS_PLAYERS;
  fighting = 0;
  what = WHAT_COINS;
  hidden_only = 1;
  spare = 0;
  max_per_victim = 0;
  tries = ([ ]);
}

/**
 * Whether somebody is a possible victim, apart from the cooldown and the
 * attempts already made.
 *
 * @param npc the thief
 * @param ob who is looked at
 * @return 1 if it can be robbed
 */
private int valid_victim(object npc, object ob)
{
  if (!ob || ob == npc || !living(ob) || ob->query_dead())
    return 0;
  if (environment(ob) != environment(npc))
    return 0;
  if (ob->query_coder())
    return 0;

  if (!ob->query_player())
  {
    if (victims != VICTIMS_ALL)
      return 0;
    // never another thief
    if (ob->query_component_by_type("thief"))
      return 0;
  }

  if (!fighting && sizeof(ob->query_attacker_list()))
    return 0;

  if (spare && stringp(npc->query_citizenship()) &&
      ob->query_citizenship() == npc->query_citizenship())
    return 0;

  if (max_per_victim && intp(tries[ob->query_name()]) &&
      tries[ob->query_name()] >= max_per_victim)
    return 0;

  return 1;
}

/**
 * Whether the thief itself may try now.
 *
 * @param npc the thief
 * @return 1 if it is in a state to rob
 */
private int ready(object npc)
{
  if (!npc || npc->query_dead() || !environment(npc))
    return 0;
  // the steal skill cannot be used in a fight either
  if (sizeof(npc->query_attacker_list()))
    return 0;
  if (hidden_only && !npc->query_hidden())
    return 0;
  if (last_attempt && time() - last_attempt < cooldown)
    return 0;

  return 1;
}

/**
 * Picks a random item the victim carries that can be taken.
 *
 * @param victim who is robbed
 * @return the item, or nil when there is none
 */
private object pick_item(object victim)
{
  object * carried, * candidates;
  int i;

  candidates = ({ });
  carried = all_inventory(victim);
  for (i = 0; i < sizeof(carried); i++)
    if (!carried[i]->is_money() && !carried[i]->query_in_use() &&
        !carried[i]->query_property(NOSTEAL_PROP))
      candidates += ({ carried[i] });

  if (!sizeof(candidates))
    return nil;

  return candidates[random(sizeof(candidates))];
}

/**
 * The attempt itself, called out from an arrival.
 */
void try_theft()
{
  object npc, victim, item, skill;
  string request, name;
  int coins;

  victim = attempt_victim;
  attempt_victim = nil;
  attempt_handle = 0;

  npc = query_entity();
  if (!ready(npc) || !valid_victim(npc, victim))
    return;

  // coins, an item, or either when both are allowed
  coins = (what == WHAT_COINS) || (what == WHAT_BOTH && random(2));
  if (!coins)
  {
    item = pick_item(victim);
    if (!item)
      return;
  }

  name = victim->query_name();
  request = coins ? STEAL_SKILL_PATH->build_request(nil, name) :
                    STEAL_SKILL_PATH->build_request(item->query_name(), name);

  // whatever the skill would refuse (a protected victim, a safe place) costs
  // the thief nothing
  if (stringp(STEAL_SKILL_PATH->extra_checks(request, npc)))
    return;

  last_attempt = time();
  tries[name] = (intp(tries[name]) ? tries[name] : 0) + 1;

  skill = load_object(STEAL_SKILL_PATH);
  skill->cast_effect(request, npc, 0);
}

/**
 * Somebody arrived where the owner stands: maybe schedule an attempt.
 */
void event_enter(object ob, varargs string mess, object from, mixed avoid)
{
  object npc;

  npc = query_entity();
  if (!npc || attempt_handle || !ob || ob == npc)
    return;
  if (!ready(npc) || !valid_victim(npc, ob))
    return;
  if (random(100) >= chance)
    return;

  attempt_victim = ob;
  attempt_handle = call_out("try_theft", delay + random(delay));
}

void dest_me()
{
  if (attempt_handle)
    remove_call_out(attempt_handle);
  attempt_handle = 0;
  component::dest_me();
}

mapping query_auto_load_attributes()
{
  mapping attrs;

  attrs = component::query_auto_load_attributes();

  if (chance != DEFAULT_CHANCE)
    attrs["chance"] = chance;
  if (delay != DEFAULT_DELAY)
    attrs["delay"] = delay;
  if (cooldown != DEFAULT_COOLDOWN)
    attrs["cooldown"] = cooldown;
  if (victims != VICTIMS_PLAYERS)
    attrs["victims"] = victims;
  if (fighting)
    attrs["fighting"] = fighting;
  if (what != WHAT_COINS)
    attrs["what"] = what;
  if (!hidden_only)
    attrs["hidden_only"] = 0;
  if (spare)
    attrs["spare"] = spare;
  if (max_per_victim)
    attrs["max_per_victim"] = max_per_victim;

  return attrs;
}

void init_auto_load_attributes(mapping args)
{
  component::init_auto_load_attributes(args);
  if (!args)
    return;

  if (intp(args["chance"]))
    chance = args["chance"];
  if (intp(args["delay"]) && args["delay"] > 0)
    delay = args["delay"];
  if (intp(args["cooldown"]))
    cooldown = args["cooldown"];
  if (stringp(args["victims"]))
    victims = args["victims"];
  if (intp(args["fighting"]))
    fighting = args["fighting"];
  if (stringp(args["what"]))
    what = args["what"];
  if (intp(args["hidden_only"]))
    hidden_only = args["hidden_only"];
  if (intp(args["spare"]))
    spare = args["spare"];
  if (intp(args["max_per_victim"]))
    max_per_victim = args["max_per_victim"];
}

mixed * stats()
{
  mixed * lines;

  lines = component::stats();

  if (chance != DEFAULT_CHANCE)
    lines += ({ ({ "Chance", chance }) });
  if (delay != DEFAULT_DELAY)
    lines += ({ ({ "Delay", delay }) });
  if (cooldown != DEFAULT_COOLDOWN)
    lines += ({ ({ "Cooldown", cooldown }) });
  if (victims != VICTIMS_PLAYERS)
    lines += ({ ({ "Victims", victims }) });
  if (fighting)
    lines += ({ ({ "Robs fighters", fighting }) });
  if (what != WHAT_COINS)
    lines += ({ ({ "Takes", what }) });
  if (!hidden_only)
    lines += ({ ({ "Hidden only", 0 }) });
  if (spare)
    lines += ({ ({ "Spares own citizenship", spare }) });
  if (max_per_victim)
    lines += ({ ({ "Max per victim", max_per_victim }) });

  return lines + ({ ({ "Last attempt", last_attempt ? ctime(last_attempt) : "never" }) });
}
