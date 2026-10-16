// Steal (robar) — active skill. Takes an item, or a few coins, from somebody
// in the same place, rolled against their perception:
//
//   steal <item> from <someone>
//   steal <item> in <container> from <someone>   (an open container they carry)
//   steal coins from <someone>
//
// The words joining the parts, and the words for coins, come from the lang
// file. Nothing moves unless the roll is won; a clean theft is told only to
// the thief, a noticed one to the victim and the room, and a caught thief
// gets nothing, loses any hiding and, against a monster, is attacked.
//
// Anything can refuse to be stolen with the "nosteal" property (NOSTEAL_PROP),
// on the item, its container, the victim or the place.

#include <living/effects.h>
#include <living/skills.h>
#include <common/properties.h>
#include <basic/move.h>
#include <basic/money.h>
#include <language.h>

inherit BASE_EFFECT;

#define STEAL_COST 8
#define STEAL_LOCKOUT 15

// players below this level cannot be stolen from
#define STEAL_MIN_VICTIM_LEVEL 5
// a theft is never certain nor impossible: this percentage flips the result
#define STEAL_MIN_CHANCE 5
// margin (thief roll minus victim roll) from which the victim notices nothing
#define STEAL_CLEAN_MARGIN 15
// score bonuses and maluses
#define STEAL_HIDDEN_BONUS 20
#define STEAL_ALERT_BONUS 20
#define STEAL_ASLEEP_MALUS 30
// score lost per weight unit of the item, up to the maximum
#define STEAL_WEIGHT_FACTOR 2
#define STEAL_WEIGHT_MAX 30
// how long a victim who caught (or noticed) a thief stays alert: heartbeat
// units, 5 seconds each
#define STEAL_ALERT_TICKS 6

#define STEAL_CAUGHT  0
#define STEAL_NOTICED 1
#define STEAL_CLEAN   2

void setup()
{
  // Display name + aliases; the learning key is SKILL_STEAL.
  set_effect_name(_LANG_SKILL_STEAL_NAME);
  set_aliases(_LANG_SKILL_STEAL_ALIASES);

  set_gp_cost(STEAL_COST);
  set_lockout_time(STEAL_LOCKOUT);

  // the argument is parsed here, not looked up by the effect system
  set_target_type(TARGET_TYPE_PASS);
  set_range(0);

  add_category(SKILL_TYPE_EXPLORATION, 1);

  allow_while_mounted = 0;
  allow_while_pacified = 0;
  allow_while_hidden = 1;
  allow_while_moving = 0;
  allow_in_combat = 0;

  set_help_desc(_LANG_SKILL_STEAL_HELP);

  set_start_phrases(_LANG_SKILL_STEAL_START, "");

  set_rounds(({ "start_steal", "do_steal" }));
}

/**
 * Splits a string in two around a separator, trimmed.
 *
 * @param str the text to split
 * @param sep the separator, with its surrounding spaces (" from ")
 * @param last split at the last occurrence instead of the first
 * @return ({ before, after }), or nil when the separator is not there
 */
private string * split_on(string str, string sep, int last)
{
  string padded, before, after;
  int pos, len;

  padded = " " + str + " ";
  len = strlen(sep);
  pos = last ? strsrch(padded, sep, -1) : strsrch(padded, sep);
  if (pos < 0)
    return nil;

  before = (pos > 0) ? padded[0..pos - 1] : "";
  after = (pos + len < strlen(padded)) ? padded[pos + len..] : "";

  return ({ trim(before), trim(after) });
}

/**
 * Parses and validates a request.
 *
 * @param str what the thief typed after the verb
 * @param caster the thief
 * @return the reason why it cannot be done, or ({ victim, item, container,
 *         coins }) with item and container nil when stealing coins
 */
private mixed parse_request(string str, object caster)
{
  object env, victim, item, container;
  string * parts;
  string what, who;
  mixed found, dark;

  if (!stringp(str) || !strlen(trim(str)))
    return _LANG_SKILL_STEAL_SYNTAX;

  env = environment(caster);
  if (!env)
    return _LANG_SKILL_STEAL_SYNTAX;

  // the victim is whoever follows the last connector
  parts = split_on(lower_case(str), _LANG_SKILL_STEAL_VICTIM_WORD, 1);
  if (!parts || !strlen(parts[0]) || !strlen(parts[1]))
    return _LANG_SKILL_STEAL_SYNTAX;

  what = parts[0];
  who = parts[1];

  // who, and where: only what the thief can see, in the same place
  found = find_one_match(who, env, 1);
  if (!objectp(found))
    return _LANG_SKILL_STEAL_NO_VICTIM;

  victim = found;

  if (victim == caster)
    return _LANG_SKILL_STEAL_SELF;
  if (!living(victim))
    return _LANG_SKILL_STEAL_NOT_LIVING;
  if (victim->query_dead())
    return _LANG_SKILL_STEAL_DEAD;
  if (victim->query_coder())
    return _LANG_SKILL_STEAL_IMMORTAL;

  if (victim->query_property(NOSTEAL_PROP) || env->query_property(NOSTEAL_PROP) ||
      caster->query_property(NOSTEAL_PROP))
    return _LANG_SKILL_STEAL_NOSTEAL;

  // players are protected while they are low level and in the places that
  // forbid fighting
  if (victim->query_player())
  {
    if (victim->query_level() < STEAL_MIN_VICTIM_LEVEL)
      return _LANG_SKILL_STEAL_PROTECTED;
    if (env->query_property(NOKILL_PROP))
      return _LANG_SKILL_STEAL_SAFE_PLACE;
  }

  // no light, or too much, and there is nothing to pick
  dark = caster->check_dark(env->query_light());
  if (intp(dark) && (dark == 1 || dark == 6))
    return _LANG_SKILL_STEAL_DARK;

  // coins
  if (member_array(what, _LANG_SKILL_STEAL_COIN_WORDS) != -1)
  {
    found = victim->query_money_array();
    if (!pointerp(found) || !sizeof(found))
      return _LANG_SKILL_STEAL_NO_COINS;

    return ({ victim, nil, nil, 1 });
  }

  // an item the victim carries, or a container they carry and an item in it
  found = find_one_match(what, victim);
  if (!objectp(found))
  {
    parts = split_on(what, _LANG_SKILL_STEAL_CONTAINER_WORD, 0);
    if (!parts || !strlen(parts[0]) || !strlen(parts[1]))
      return _LANG_SKILL_STEAL_NO_ITEM;

    found = find_one_match(parts[1], victim);
    if (!objectp(found))
      return _LANG_SKILL_STEAL_NO_CONTAINER;

    container = found;
    if (!container->query_max_weight())
      return _LANG_SKILL_STEAL_NO_CONTAINER;
    if (container->query_closed())
      return _LANG_SKILL_STEAL_CLOSED;
    if (container->query_property(NOSTEAL_PROP))
      return _LANG_SKILL_STEAL_NOSTEAL;

    found = find_one_match(parts[0], container);
    if (!objectp(found))
      return _LANG_SKILL_STEAL_NO_ITEM_IN;
  }

  item = found;

  if (item->is_money())
    return _LANG_SKILL_STEAL_MONEY_ITEM;
  if (item->query_in_use())
    return _LANG_SKILL_STEAL_IN_USE;
  if (item->query_property(NOSTEAL_PROP))
    return _LANG_SKILL_STEAL_NOSTEAL;

  return ({ victim, item, container, 0 });
}

/**
 * Writes the argument this skill parses, in the current language, for a caller
 * that fires the effect itself (an NPC thief).
 *
 * @param item the item name to take, nil for coins
 * @param victim_name the name of whoever is robbed
 * @return the text after the verb, as a player would type it
 */
string build_request(string item, string victim_name)
{
  if (!item)
    item = _LANG_SKILL_STEAL_COIN_WORDS[0];

  return item + _LANG_SKILL_STEAL_VICTIM_WORD + victim_name;
}

// Why the theft cannot start, before anything is said or spent.
mixed extra_checks(string str, object caster)
{
  mixed request;

  request = parse_request(str, caster);
  if (stringp(request))
    return request;

  return 0;
}

int start_steal(object caster, mixed target, mixed out_range, int time, int quiet)
{
  mixed request;

  // checked again: things may have changed since the theft was started
  request = parse_request(target, caster);
  if (stringp(request))
  {
    tell_object(caster, request);
    return 0;
  }

  tell_object(caster, _LANG_SKILL_STEAL_ROUND1);
  return 1;
}

/**
 * Rolls the thief's score against the victim's.
 *
 * @param caster the thief
 * @param victim who is robbed
 * @param item what is taken, nil for coins
 * @return STEAL_CAUGHT, STEAL_NOTICED or STEAL_CLEAN
 */
private int roll_theft(object caster, object victim, object item)
{
  int thief, target, margin, penalty;

  thief = caster->query_dex() + caster->query_level() + caster->query_per() / 2 +
          caster->query_skill_ability(SKILL_STEAL) / 2;
  if (caster->query_hidden())
    thief += STEAL_HIDDEN_BONUS;
  if (item)
  {
    penalty = item->query_weight() * STEAL_WEIGHT_FACTOR;
    thief -= (penalty > STEAL_WEIGHT_MAX) ? STEAL_WEIGHT_MAX : penalty;
  }

  target = victim->query_per() + victim->query_level() + victim->query_int() / 2;
  if (sizeof(victim->query_attacker_list()) ||
      victim->query_property(STEAL_ALERT_PROP))
    target += STEAL_ALERT_BONUS;
  if (victim->query_property(SLEEP_PROP) ||
      victim->query_property(PASSED_OUT_PROP) ||
      victim->query_property(PARALYZE_PROP))
    target -= STEAL_ASLEEP_MALUS;

  // random() needs a positive bound
  if (thief < 1)
    thief = 1;
  if (target < 1)
    target = 1;

  margin = (random(thief) + thief / 4) - (random(target) + target / 4);

  // never certain, never impossible
  if (random(100) < STEAL_MIN_CHANCE)
    margin = (margin <= 0) ? 1 : 0;

  if (margin <= 0)
    return STEAL_CAUGHT;
  if (margin <= STEAL_CLEAN_MARGIN)
    return STEAL_NOTICED;
  return STEAL_CLEAN;
}

// A message with its actor and target tokens resolved and the thing taken in
// place of $item$.
private string steal_message(string template, object caster, object victim,
                             string what)
{
  return replace_string(personalize_string(template, caster, victim),
                        "$item$", what);
}

// How an item is called in a message.
private string item_name(object item)
{
  mixed name;

  name = item->short();
  if (!stringp(name))
    name = item->query_short();
  if (!stringp(name))
    return "?";

  return name;
}

private void log_theft(object caster, object victim, string what, string result)
{
  log_file("steal", "[" + ctime(time(), 4) + "] " + caster->query_name() +
    " -> " + victim->query_name() + ": " + what + " (" + result + ")\n");
}

/**
 * Takes a few coins from each currency the victim carries.
 *
 * @param caster the thief
 * @param victim who is robbed
 * @return a description of what was taken, or nil when nothing was
 */
private string take_coins(object caster, object victim)
{
  mixed array;
  string * types;
  int * amounts;
  mixed taken;
  string type;
  int i, level, amount, removed, unit, coins;

  array = victim->query_money_array();
  if (!pointerp(array))
    return nil;

  // what the victim carries, copied: adjusting their money changes the array
  types = ({ });
  amounts = ({ });
  for (i = 0; i + 1 < sizeof(array); i += 2)
  {
    type = array[i];
    amount = array[i + 1];
    types += ({ type });
    amounts += ({ amount });
  }

  level = caster->query_level();
  taken = ({ });

  for (i = 0; i < sizeof(types); i++)
  {
    // a handful: more with level, never more than they carry
    amount = level / 3 + random(level / 3 + 1) + 1;
    if (amount > amounts[i])
      amount = amounts[i];

    // what the victim's purse says it gave up, in coins of this currency
    removed = victim->adjust_money(-amount, types[i]);
    unit = MONEY_HAND->query_value(types[i]);
    if (removed >= 0 || unit < 1)
      continue;

    coins = (-removed) / unit;
    if (coins < 1)
      continue;

    caster->adjust_money(coins, types[i]);
    taken += ({ types[i], coins });
  }

  if (!sizeof(taken))
    return nil;

  return MONEY_HAND->money_string(taken);
}

int do_steal(object caster, mixed target, mixed out_range, int time, int quiet)
{
  mixed request;
  object victim, item, container, holder, env;
  string what, logged;
  int coins, outcome;

  request = parse_request(target, caster);
  if (stringp(request))
  {
    tell_object(caster, request);
    return 0;
  }

  victim = request[0];
  item = request[1];
  container = request[2];
  coins = request[3];
  env = environment(caster);

  // the roll comes first: nothing moves, nor is spent, before it
  outcome = roll_theft(caster, victim, item);

  if (outcome == STEAL_CAUGHT)
  {
    logged = coins ? "coins" : item_name(item);

    if (caster->query_hidden())
      caster->dest_hide_shadow();

    tell_object(caster, steal_message(_LANG_SKILL_STEAL_CAUGHT_ME, caster, victim, ""));
    tell_object(victim, steal_message(_LANG_SKILL_STEAL_CAUGHT_VICTIM, caster, victim, ""));
    tell_room(env, steal_message(_LANG_SKILL_STEAL_CAUGHT_ROOM, caster, victim, ""),
      ({ caster, victim }));

    victim->add_timed_property(STEAL_ALERT_PROP, 1, STEAL_ALERT_TICKS);

    // a monster goes for the thief; a player only knows
    if (victim->query_npc())
      victim->attack_ob(caster);

    log_theft(caster, victim, logged, "caught");
    return 1;
  }

  if (coins)
  {
    what = take_coins(caster, victim);
    if (!what)
    {
      tell_object(caster, _LANG_SKILL_STEAL_NO_COINS);
      return 0;
    }
  }
  else
  {
    // still on the victim (or in their container)? and did the move work?
    holder = container ? container : victim;
    if (!item || !victim || environment(victim) != env ||
        (container && environment(container) != victim) ||
        environment(item) != holder)
    {
      tell_object(caster, _LANG_SKILL_STEAL_GONE);
      return 0;
    }

    what = item_name(item);
    if (item->move(caster) != MOVE_OK)
    {
      tell_object(caster, steal_message(_LANG_SKILL_STEAL_CANNOT_CARRY,
        caster, victim, what));
      log_theft(caster, victim, what, "could not carry");
      return 1;
    }
  }

  if (outcome == STEAL_CLEAN)
  {
    tell_object(caster, steal_message(_LANG_SKILL_STEAL_CLEAN_ME, caster, victim, what));
    log_theft(caster, victim, what, "clean");
    return 1;
  }

  // noticed: the victim knows who did it, and the thief is no longer hidden
  if (caster->query_hidden())
    caster->dest_hide_shadow();

  tell_object(caster, steal_message(_LANG_SKILL_STEAL_NOTICED_ME, caster, victim, what));
  tell_object(victim, steal_message(_LANG_SKILL_STEAL_NOTICED_VICTIM, caster, victim, what));
  tell_room(env, steal_message(_LANG_SKILL_STEAL_NOTICED_ROOM, caster, victim, what),
    ({ caster, victim }));

  victim->add_timed_property(STEAL_ALERT_PROP, 1, STEAL_ALERT_TICKS);

  log_theft(caster, victim, what, "noticed");
  return 1;
}
