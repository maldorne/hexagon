// trainer.c -- teaching skills for money, as an NPC component.
//
// A master that raises the ability of the percentage skills it teaches, a point
// at a time, for a price per point, between a floor and a ceiling: it teaches
// nothing to whoever is below the floor (what use can still give them is theirs
// to earn) nor past the ceiling. It asks the NPC for three verbs: one to list
// what it teaches, one to say what a training costs and one to train. Nobody is
// trained while the NPC fights, and only skills the player already knows.
//
// Attributes:
//   "skills"  - what it teaches, a mapping skill id -> a mapping with
//               "max"   the highest ability it takes the skill to
//               "min"   the lowest ability it teaches from (default 0)
//               "price" base coins for each point, this skill's own price
//               (default: the "price" attribute below)
//   "price"   - base coins for each point of every skill that has no price of
//               its own; without any price, teaching is free
//   "gate"    - a rule that has to hold before it teaches anybody, a mapping:
//               "quest"    the id of a quest the player has completed
//                          ("<game>:<name>"), or
//               "property" a property the player must have
//   "refusal" - what somebody who does not pass the gate is told; defaults to
//               the stock text
//
// What the player types (Spanish / English):
//   lista / list                         what it teaches and the ability it
//                                        takes each skill to
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

void create()
{
  component::create();
  trainer_skills = ([ ]);
  trainer_price = 0;
  trainer_gate = ([ ]);
  trainer_refusal = nil;
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
 * Whether the player passes the gate of the trainer.
 */
private int _passes_gate(object who)
{
  string quest, * parts;

  if (stringp(trainer_gate["quest"]))
  {
    // a quest id is "<game>:<name>", and the player keeps it whole under its game
    quest = trainer_gate["quest"];
    parts = explode(quest, ":");
    if (sizeof(parts) < 2 || !who->has_completed_quest(parts[0], quest))
      return 0;
  }
  if (stringp(trainer_gate["property"]) &&
      !who->query_property(trainer_gate["property"]))
    return 0;
  return 1;
}

/**
 * The checks every verb makes before it talks about a skill.
 *
 * @return 1 when the player may go on, 0 after the NPC said why not
 */
private int _may_ask(object who, object me)
{
  if (!who || !me)
    return 0;

  if (me->query_is_fighting())
  {
    me->do_say(_LANG_TRAINER_BUSY);
    return 0;
  }

  if (!_passes_gate(who))
  {
    me->do_say(trainer_refusal ? trainer_refusal : _LANG_TRAINER_REFUSED);
    return 0;
  }
  return 1;
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
  string what, id, skill_name;
  int to, now, max, min;
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

  if (!who->query_known_skill(id) ||
      who->query_skill_kind(id) != SKILL_KIND_PERCENTAGE)
  {
    me->do_say(_LANG_TRAINER_UNKNOWN_SKILL);
    return nil;
  }

  row = trainer_skills[id];
  now = (int)who->query_skill_ability(id);
  min = mappingp(row) && intp(row["min"]) ? row["min"] : 0;
  max = mappingp(row) && intp(row["max"]) ? row["max"] : 100;

  if (now < min)
  {
    me->do_say(_LANG_TRAINER_TOO_LOW);
    return nil;
  }
  if (now >= max)
  {
    me->do_say(_LANG_TRAINER_NOTHING_MORE);
    return nil;
  }

  if (!to)
    to = now + 1;
  if (to > max)
    to = max;
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
  string * ids, name;
  mapping row;
  int i, price;

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
    tell_object(who, _LANG_TRAINER_LIST_LINE);
  }
  return 1;
}

int do_cost(string str)
{
  object who, me;
  mixed * ask;
  string skill_name, price_text;
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
  me->do_say(_LANG_TRAINER_COST);
  return 1;
}

int do_train(string str)
{
  object who, me, payer;
  mixed * ask;
  string skill_name, price_text;
  int points, total;

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
  who->adjust_skill_ability(ask[0], points, 1);
  me->do_say(_LANG_TRAINER_DONE);
  tell_object(who, _LANG_TRAINER_TRAINED);
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
}
