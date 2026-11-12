// trainer.c -- teaching skills for money, as an NPC component.
//
// A master that raises the ability of the percentage skills it teaches, a point
// at a time, for a price per point, between a floor and a ceiling: it teaches
// nothing to whoever is below the floor (what use can still give them is theirs
// to earn) nor past the ceiling. It asks the NPC for three verbs: one to list
// what it teaches, one to say what a training costs and one to train. Nobody is
// trained while the NPC fights, and only skills the player already knows. It never
// lowers anything.
//
// A row can also be a weapon mastery (a family of weapons: maces, hammers...). The
// ability is then the player's value in that mastery, raised with
// adjust_current_mastery_value: the player must already hold it (an entry of
// query_known_weapon_masteries, which only fighting with the weapon creates), and the
// trainer teaches it only if the guild of the player allows at least one weapon of
// the family (query_legal_weapon over the weapons of the family in
// /lib/tables/weapons.c; no guild allows every one).
//
// Attributes:
//   "skills"  - what it teaches, a mapping skill id -> a mapping with
//               "max"   the highest ability it takes the skill to
//               "min"   the lowest ability it teaches from (default 0)
//               "price" base coins for each point, this skill's own price
//               (default: the "price" attribute below)
//               "mastery" 1 when the id is a weapon mastery (the language's own
//                       MACE_MASTERY, HAMMER_MASTERY... of <translations/weapon.h>)
//               "step"  the most points one training gives (default: as many as the
//                       player asks); a player asking for more is given this many
//   "cooldown" - seconds a player has to wait after any training before the next one
//               with this trainer (default none); kept in a timed property of the
//               player, so it holds across sessions
//   "price"   - base coins for each point of every skill that has no price of
//               its own; without any price, teaching is free
//   "gate"    - a rule that has to hold before it teaches anybody, a mapping:
//               "quest"    the id of a quest the player has completed
//                          ("<game>:<name>"), or
//               "property" a property the player must have, and/or
//               "min_level" the lowest level the player may have
//   "refusal" - what somebody who does not pass the gate is told; defaults to
//               the stock text
//
// What the player types (Spanish / English):
//   lista / list                         what it teaches and the ability it
//                                        takes each skill to; for a mastery, the
//                                        player's own value, the price and when
//                                        the next point is possible
//   coste / cost <skill> [hasta / to <n>] what raising the skill costs, one
//                                        point or up to ability n
//   entrenar / train <skill> [hasta / to <n>]  does it, paying the whole price
//
// Nothing is taken until the player can pay all of it.

#include <language.h>
#include <living/skills.h>
#include <living/quests.h>

inherit component "/lib/npc/component.c";
inherit fee "/lib/npc/service-fee.c";

private mapping trainer_skills;
private int trainer_price;
private mapping trainer_gate;
private string trainer_refusal;
private int trainer_cooldown;

void create()
{
  component::create();
  trainer_skills = ([ ]);
  trainer_price = 0;
  trainer_gate = ([ ]);
  trainer_refusal = nil;
  trainer_cooldown = 0;
}

mapping query_component_actions()
{
  mapping out;
  string * verbs;
  int i;

  out = ([ ]);

  verbs = _LANG_TRAINER_LIST_VERBS;
  for (i = 0; i < sizeof(verbs); i++)
    out[verbs[i]] = "do_list";

  verbs = _LANG_TRAINER_COST_VERBS;
  for (i = 0; i < sizeof(verbs); i++)
    out[verbs[i]] = "do_cost";

  verbs = _LANG_TRAINER_TRAIN_VERBS;
  for (i = 0; i < sizeof(verbs); i++)
    out[verbs[i]] = "do_train";

  return out;
}

/**
 * The object of a skill, loaded when needed.
 */
private object _skill_object(string id)
{
  mixed * row;
  object ob;

  row = table("skills")->query_skill_data(id);
  if (!arrayp(row) || !sizeof(row))
    return nil;
  ob = find_object(row[SKILL_DATA_PATH]);
  if (!ob)
    catch(ob = load_object(row[SKILL_DATA_PATH]));
  return ob;
}

/**
 * The name the player reads for a skill.
 */
private string _skill_name(string id)
{
  object ob;

  ob = _skill_object(id);
  return ob ? (string)ob->query_effect_name() : id;
}

/**
 * The id of the skill the player means, by its id, its name or an alias; nil
 * when this master does not teach it.
 */
private string _find_skill(string what)
{
  string * ids, * aliases;
  string id;
  object ob;
  int i;

  what = lower_case(what);
  ids = keys(trainer_skills);
  for (i = 0; i < sizeof(ids); i++)
  {
    id = ids[i];
    if (what == id || what == lower_case(_skill_name(id)))
      return id;
    ob = _skill_object(id);
    aliases = ob ? (string *)ob->query_aliases() : nil;
    if (aliases && member_array(what, aliases) != -1)
      return id;
  }
  return nil;
}

/**
 * Base coins one point of the skill costs.
 */
private int _price_of(string id)
{
  mapping row;

  row = trainer_skills[id];
  if (mappingp(row) && intp(row["price"]))
    return row["price"];
  return trainer_price;
}

/**
 * What a player who does not pass the gate of the trainer is told.
 *
 * @return nil when the player passes it
 */
private string _gate_refusal(object who)
{
  string quest, * parts;
  string text;

  text = trainer_refusal ? trainer_refusal : _LANG_TRAINER_REFUSED;

  if (stringp(trainer_gate["quest"]))
  {
    // a quest id is "<game>:<name>", and the player keeps it whole under its game
    quest = trainer_gate["quest"];
    parts = explode(quest, ":");
    if (sizeof(parts) < 2 || !who->has_completed_quest(parts[0], quest))
      return text;
  }
  if (stringp(trainer_gate["property"]) &&
      !who->query_property(trainer_gate["property"]))
    return text;
  if (intp(trainer_gate["min_level"]) &&
      (int)who->query_level() < trainer_gate["min_level"])
    return _LANG_TRAINER_LEVEL;
  return nil;
}

/**
 * The checks every verb makes before it talks about a skill.
 *
 * @return 1 when the player may go on, 0 after the NPC said why not
 */
private int _may_ask(object who, object me)
{
  string refusal;

  if (!who || !me)
    return 0;

  if (me->query_is_fighting())
  {
    me->do_say(_LANG_TRAINER_BUSY);
    return 0;
  }

  refusal = _gate_refusal(who);
  if (refusal)
  {
    me->do_say(refusal);
    return 0;
  }
  return 1;
}

/**
 * The player's value in a weapon mastery, -1 when they never held it: the
 * entry only exists once they have fought with a weapon of the family.
 */
private int _mastery_value(object who, string mastery)
{
  mapping known;

  known = (mapping)who->query_known_weapon_masteries();
  if (!mappingp(known) || member_array(mastery, map_indices(known)) == -1)
    return -1;
  return (int)known[mastery];
}

/**
 * Whether the guild of the player allows at least one weapon of the family; a
 * player with no guild may use them all.
 */
private int _mastery_legal(object who, string mastery)
{
  string guild;
  string * names;
  int i;

  guild = (string)who->query_guild_ob();
  if (!stringp(guild))
    return 1;

  names = (string *)table("weapons")->query_weapons_of_mastery(mastery);
  for (i = 0; i < sizeof(names); i++)
    if (guild->query_legal_weapon(names[i]))
      return 1;
  return 0;
}

/**
 * The timed property that keeps the wait between two trainings with this master.
 */
private string _cooldown_key(object me)
{
  return "trainer:" + me->query_name();
}

/**
 * Seconds a player has still to wait for the next training, 0 when none.
 */
private int _cooldown_left(object who, object me)
{
  int until;

  if (trainer_cooldown <= 0)
    return 0;
  until = (int)who->query_timed_property(_cooldown_key(me));
  return until > time() ? until - time() : 0;
}

/**
 * Whether the row is a weapon mastery.
 */
private int _is_mastery(string id)
{
  mapping row;

  row = trainer_skills[id];
  return mappingp(row) && row["mastery"];
}

/**
 * The skill and the ability the player asks for: "<skill>" is one point more,
 * "<skill> hasta <n>" up to n.
 *
 * @return ({ id, current ability, target ability }), nil after the NPC said what
 *         is wrong
 */
private mixed * _parse(object who, object me, string str)
{
  string what, id, skill_name, left_text;
  int to, now, max, min, left;
  mapping row;

  if (!str || !strlen(str))
  {
    me->do_say(_LANG_TRAINER_WHICH);
    return nil;
  }

  to = 0;
  what = str;
  if (sscanf(str, "%s " + _LANG_TRAINER_UP_TO + " %d", what, to) != 2)
    what = str;

  id = _find_skill(what);
  if (!id)
  {
    me->do_say(_LANG_TRAINER_NOT_TAUGHT);
    return nil;
  }
  skill_name = _skill_name(id);
  row = trainer_skills[id];
  min = mappingp(row) && intp(row["min"]) ? row["min"] : 0;
  max = mappingp(row) && intp(row["max"]) ? row["max"] : 100;

  if (_is_mastery(id))
  {
    if (!_mastery_legal(who, id))
    {
      me->do_say(_LANG_TRAINER_MASTERY_ILLEGAL);
      return nil;
    }
    now = _mastery_value(who, id);
    if (now < 0)
    {
      me->do_say(_LANG_TRAINER_MASTERY_NEVER);
      return nil;
    }
  }
  else
  {
    if (!who->query_known_skill(id) ||
        who->query_skill_kind(id) != SKILL_KIND_PERCENTAGE)
    {
      me->do_say(_LANG_TRAINER_UNKNOWN_SKILL);
      return nil;
    }
    now = (int)who->query_skill_ability(id);
  }

  if (now < min)
  {
    me->do_say(_is_mastery(id) ? _LANG_TRAINER_MASTERY_TOO_LOW : _LANG_TRAINER_TOO_LOW);
    return nil;
  }
  if (now >= max)
  {
    me->do_say(_is_mastery(id) ? _LANG_TRAINER_MASTERY_FULL : _LANG_TRAINER_NOTHING_MORE);
    return nil;
  }

  left = _cooldown_left(who, me);
  if (left > 0)
  {
    left_text = seconds_to_time_string(left);
    me->do_say(_LANG_TRAINER_COOLDOWN);
    return nil;
  }

  if (!to)
    to = now + 1;
  if (to > max)
    to = max;
  if (mappingp(row) && intp(row["step"]) && to > now + row["step"])
    to = now + row["step"];
  if (to <= now)
  {
    me->do_say(_LANG_TRAINER_NOTHING_MORE);
    return nil;
  }

  return ({ id, now, to });
}

int do_list(string str)
{
  object who, me;
  string * ids, name, value_text, status, left_text;
  mapping row;
  int i, price, min, max, now, left;

  who = this_player();
  me = query_entity();
  if (!_may_ask(who, me))
    return 1;

  ids = keys(trainer_skills);
  tell_object(who, _LANG_TRAINER_LIST_HEADER);
  for (i = 0; i < sizeof(ids); i++)
  {
    row = trainer_skills[ids[i]];
    name = _skill_name(ids[i]);
    price = _price_of(ids[i]);
    if (!_is_mastery(ids[i]))
    {
      tell_object(who, _LANG_TRAINER_LIST_LINE);
      continue;
    }

    // a mastery: the player's own value and what stops them from the next point
    min = intp(row["min"]) ? row["min"] : 0;
    max = intp(row["max"]) ? row["max"] : 100;
    now = _mastery_value(who, ids[i]);
    left = _cooldown_left(who, me);
    left_text = left > 0 ? seconds_to_time_string(left) : "";
    value_text = now < 0 ? _LANG_TRAINER_VALUE_NONE : now + "%";
    if (!_mastery_legal(who, ids[i]))
      status = _LANG_TRAINER_STATUS_ILLEGAL;
    else if (now < 0)
      status = _LANG_TRAINER_STATUS_NEVER;
    else if (now < min)
      status = _LANG_TRAINER_STATUS_TOO_LOW;
    else if (now >= max)
      status = _LANG_TRAINER_STATUS_FULL;
    else if (left > 0)
      status = _LANG_TRAINER_STATUS_WAIT;
    else
      status = _LANG_TRAINER_STATUS_READY;
    tell_object(who, _LANG_TRAINER_MASTERY_LINE);
  }
  return 1;
}

int do_cost(string str)
{
  object who, me;
  mixed * ask;
  string skill_name, price_text, cooldown_text;
  int points;

  who = this_player();
  me = query_entity();
  if (!_may_ask(who, me))
    return 1;

  ask = _parse(who, me, str);
  if (!ask)
    return 1;

  skill_name = _skill_name(ask[0]);
  points = ask[2] - ask[1];
  price_text = price_words(points * _price_of(ask[0]));
  if (_is_mastery(ask[0]))
  {
    cooldown_text = seconds_to_time_string(trainer_cooldown);
    me->do_say(_LANG_TRAINER_MASTERY_COST);
    return 1;
  }
  me->do_say(_LANG_TRAINER_COST);
  return 1;
}

int do_train(string str)
{
  object who, me, payer;
  mixed * ask;
  string skill_name, price_text, cooldown_text;
  int points, total, value;

  who = this_player();
  me = query_entity();
  if (!_may_ask(who, me))
    return 1;

  ask = _parse(who, me, str);
  if (!ask)
    return 1;

  skill_name = _skill_name(ask[0]);
  points = ask[2] - ask[1];
  total = points * _price_of(ask[0]);
  price_text = price_words(total);

  payer = nil;
  if (total > 0)
  {
    payer = find_payer(who, total, 0);
    if (!payer)
    {
      me->do_say(_LANG_TRAINER_CANNOT_PAY);
      return 1;
    }
  }

  if (payer)
    take_payment(payer, total);

  if (_is_mastery(ask[0]))
  {
    value = (int)who->adjust_current_mastery_value(ask[0], points);
    cooldown_text = seconds_to_time_string(trainer_cooldown);
    me->do_say(_LANG_TRAINER_DONE);
    tell_object(who, trainer_cooldown > 0 ? _LANG_TRAINER_MASTERY_TRAINED :
                     _LANG_TRAINER_MASTERY_TRAINED_NOW);
  }
  else
  {
    who->adjust_skill_ability(ask[0], points, 1);
    me->do_say(_LANG_TRAINER_DONE);
    tell_object(who, _LANG_TRAINER_TRAINED);
  }

  // the wait until the next training starts now
  if (trainer_cooldown > 0)
    who->add_timed_property(_cooldown_key(me), time() + trainer_cooldown, trainer_cooldown);
  return 1;
}

mapping query_auto_load_attributes()
{
  mapping out;

  out = component::query_auto_load_attributes();
  if (map_sizeof(trainer_skills))
    out["skills"] = trainer_skills;
  if (trainer_price > 0)
    out["price"] = trainer_price;
  if (map_sizeof(trainer_gate))
    out["gate"] = trainer_gate;
  if (trainer_refusal)
    out["refusal"] = trainer_refusal;
  if (trainer_cooldown > 0)
    out["cooldown"] = trainer_cooldown;
  return out;
}

void init_auto_load_attributes(mapping args)
{
  component::init_auto_load_attributes(args);
  if (!args)
    return;
  if (mappingp(args["skills"]))
    trainer_skills = ([ ]) + args["skills"];
  if (intp(args["price"]))
    trainer_price = args["price"];
  if (mappingp(args["gate"]))
    trainer_gate = ([ ]) + args["gate"];
  if (stringp(args["refusal"]))
    trainer_refusal = args["refusal"];
  if (intp(args["cooldown"]))
    trainer_cooldown = args["cooldown"];
}
