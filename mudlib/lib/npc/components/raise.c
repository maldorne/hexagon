// raise.c -- returning a ghost to its body, as an NPC component.
//
// A service somebody offers, not a fact about their race or trade, so it is a
// component rather than a base class -- the same shape as the guard role. It
// asks the NPC for two verbs: one to explain the service, one to request it.
// Being dead is the only condition, plus a cooldown, and optionally a
// restriction on who is raised.
//
// Attributes, every one optional (store only what differs from the default):
//   "only"    - who is raised; without it, anybody. A mapping whose keys all
//               have to hold:
//                 "guild"     - guild objects whose members are raised
//                 "race"      - race objects whose members are raised
//                 "align_min" - the lowest real alignment raised
//                 "property"  - a property the player must have set
//   "refusal" - what somebody left out is told; defaults to the stock text
//   "price"   - what raising costs, in base coins: ([ "base": n, "per_level": m ])
//               is n plus m for each level of the player, either key optional;
//               "free_up_to": l raises players of level l or lower for nothing.
//               Without it the service is free.
//   "spell"   - file of the spell the NPC casts on the ghost. With it, the
//               raising is on credit: nobody dead carries money, so the price
//               is not taken but written down as a debt of the player towards
//               this raiser (see below). Without it the old way holds: the
//               price is taken at once, from the ghost's own purse or, failing
//               that, from the corpse lying in the same place.
//   "level"   - guild level (1-20) the spell is cast at (default: the NPC's
//               guild level, or its level when it has no guild)
//
// Debts: one persistent property of the player, RAISE_DEBTS_PROP, holds all of
// them: a mapping keyed by the identity of the raiser (its npc source, so it
// survives the NPC being made again) to ([ "amount": base coins, "since": time,
// "name": capitalized name of the raiser ]). They never expire. A raiser does
// not raise somebody who still owes it; the other raisers do, each with its
// own debt. The verbs `pagar` / `pay` settle it, partially if need be.

#include <language.h>
#include <common/properties.h>
#include <living/effects.h>

inherit component "/lib/npc/component.c";
inherit fee "/lib/npc/service-fee.c";

// How long, in heart beats, before the same player may ask again.
#define RAISE_COOLDOWN 100
// The timed property stamped on the player for that long.
#define RAISE_COOLDOWN_PROP "raise_cooldown"

private mapping raise_only;
private string raise_refusal;
// The persistent property of the player that holds every debt.
#define RAISE_DEBTS_PROP "raise_debts"
#define RAISE_MAX_LEVEL 20
// the power the spell runs with, the neutral one
#define RAISE_POWER 2

private mapping raise_price;
private string raise_spell;
private int raise_level;

void create()
{
  component::create();
  raise_only = ([ ]);
  raise_refusal = nil;
  raise_price = ([ ]);
  raise_spell = nil;
  raise_level = 0;
}

/**
 * Whether `who` meets every rule of the "only" attribute.
 *
 * @param who the player asking to be raised
 * @return 1 when they are raised, 0 when left out
 */
private int _is_eligible(object who)
{
  return meets_only(who, raise_only);
}

/**
 * What raising `who` costs.
 *
 * @param who the player asking to be raised
 * @return the price in base coins, 0 when the service is free
 */
private int _query_cost(object who)
{
  int cost;

  cost = 0;
  if (intp(raise_price["free_up_to"]) &&
      who->query_level() <= raise_price["free_up_to"])
    return 0;
  if (intp(raise_price["base"]))
    cost += raise_price["base"];
  if (intp(raise_price["per_level"]))
    cost += raise_price["per_level"] * who->query_level();
  return cost > 0 ? cost : 0;
}

// The service is a fact about the component, not state to carry: nothing here
// is persisted beyond what the base already stores.
mapping query_component_actions()
{
  mapping out;
  string * verbs;
  int i;

  out = ([ ]);

  verbs = _LANG_RAISE_INFO_VERBS;
  for (i = 0; i < sizeof(verbs); i++)
    out[verbs[i]] = "do_info";

  verbs = _LANG_RAISE_VERBS;
  for (i = 0; i < sizeof(verbs); i++)
    out[verbs[i]] = "do_raise";

  verbs = _LANG_RAISE_PAY_VERBS;
  for (i = 0; i < sizeof(verbs); i++)
    out[verbs[i]] = "do_pay";

  return out;
}

/**
 * The identity of this raiser in the debts: its npc source, which a new clone
 * of the same NPC keeps.
 */
private string _query_raiser_id()
{
  object me;
  string id;

  me = query_entity();
  id = me->query_npc_source();
  return id ? id : base_name(me);
}

/**
 * The debt `who` owes this raiser.
 *
 * @return the debt entry, nil when they owe nothing
 */
private mapping _query_debt(object who)
{
  mixed debts;

  debts = who->query_property(RAISE_DEBTS_PROP);
  if (!mappingp(debts) || !mappingp(debts[_query_raiser_id()]))
    return nil;
  return debts[_query_raiser_id()];
}

/**
 * Writes down (or, with 0, erases) what `who` owes this raiser, leaving the
 * debts to the other raisers as they are.
 */
private void _set_debt(object who, int amount)
{
  mixed debts;

  debts = who->query_property(RAISE_DEBTS_PROP);
  debts = mappingp(debts) ? ([ ]) + debts : ([ ]);

  if (amount > 0)
    debts[_query_raiser_id()] = ([ "amount": amount, "since": time(),
      "name": query_entity()->query_cap_name() ]);
  else
    debts[_query_raiser_id()] = nil;

  if (map_sizeof(debts))
    who->add_property(RAISE_DEBTS_PROP, debts);
  else
    who->remove_property(RAISE_DEBTS_PROP);
}

/**
 * The guild level the spell is cast at: the "level" attribute, or the NPC's
 * own guild level, or its level when it has no guild, capped to 1-20.
 */
private int _query_cast_level()
{
  int level;

  level = raise_level;
  if (level <= 0)
    level = query_entity()->query_guild_level();
  if (level <= 0)
    level = query_entity()->query_level();
  if (level > RAISE_MAX_LEVEL)
    level = RAISE_MAX_LEVEL;
  return level < 1 ? 1 : level;
}

/**
 * The spell object of the service, loaded when needed.
 *
 * @return the spell, nil when it is not set or does not load
 */
private object _query_spell_object()
{
  object spell;

  if (!raise_spell)
    return nil;
  spell = find_object(raise_spell);
  if (!spell)
    catch(spell = load_object(raise_spell));
  return spell;
}

/**
 * Whether the NPC is in the middle of casting, or cannot cast now.
 */
private int _is_busy(object me)
{
  return me->query_active_effect("effect") ||
    me->query_property(PROPERTY_IS_CASTING) ||
    me->check_props(NOSKILL_PROP) ||
    me->query_property(PASSED_OUT_PROP);
}

int do_info(string str)
{
  object who;
  mapping debt;
  string price_text;
  int cost;

  if (!query_entity())
    return 0;

  query_entity()->do_say(_LANG_RAISE_INFO);

  who = this_player();
  cost = who ? _query_cost(who) : 0;
  if (cost)
  {
    price_text = price_words(cost);
    query_entity()->do_say(raise_spell ? _LANG_RAISE_INFO_CREDIT :
      _LANG_RAISE_INFO_PRICE);
  }

  debt = who ? _query_debt(who) : nil;
  if (debt)
  {
    price_text = price_words(debt["amount"]);
    query_entity()->do_say(_LANG_RAISE_DEBT_REMINDER);
  }
  return 1;
}

int do_raise(string str)
{
  object who, payer, me, spell;
  string price_text;
  int cost;

  who = this_player();
  if (!who || !query_entity())
    return 0;

  if (who->query_timed_property(RAISE_COOLDOWN_PROP))
  {
    query_entity()->do_say(_LANG_RAISE_TOO_SOON);
    return 1;
  }

  if (!who->query_dead())
  {
    query_entity()->do_say(_LANG_RAISE_NOT_DEAD);
    return 1;
  }

  if (!_is_eligible(who))
  {
    query_entity()->do_say(raise_refusal ? raise_refusal : _LANG_RAISE_REFUSED);
    return 1;
  }

  // Whoever still owes this raiser is not raised again by it
  if (_query_debt(who))
  {
    price_text = price_words(_query_debt(who)["amount"]);
    query_entity()->do_say(_LANG_RAISE_DEBT_REFUSED);
    return 1;
  }

  if (raise_spell)
  {
    me = query_entity();
    spell = _query_spell_object();
    if (!spell || _is_busy(me))
    {
      me->do_say(_LANG_RAISE_BUSY);
      return 1;
    }

    // The ghost carries no money: the price is told and kept as a debt, once
    // the spell has started
    if (!spell->cast_effect_at(who->query_name(), me, _query_cast_level(),
        RAISE_POWER))
    {
      me->do_say(_LANG_RAISE_BUSY);
      return 1;
    }

    cost = _query_cost(who);
    if (cost)
    {
      _set_debt(who, cost);
      price_text = price_words(cost);
      me->do_say(_LANG_RAISE_DEBT_TOLD);
    }
    who->add_timed_property(RAISE_COOLDOWN_PROP, 1, RAISE_COOLDOWN);
    return 1;
  }

  // The price is checked before the gesture and taken after the player is back,
  // so somebody who cannot be raised (remove_ghost refuses) pays nothing.
  cost = _query_cost(who);
  if (cost)
  {
    price_text = price_words(cost);
    payer = find_payer(who, cost, 1);
    if (!payer)
    {
      query_entity()->do_say(_LANG_RAISE_CANNOT_PAY);
      return 1;
    }
  }

  // Everybody present sees the gesture, the raised player included -- it is the
  // one thing here that remove_ghost cannot know about. It then tells the player
  // they are back and tells the room, each in their own words, so saying either
  // of those again would only repeat it.
  tell_room(environment(who), _LANG_RAISE_GESTURE);

  who->remove_ghost();
  if (who->query_dead())
    return 1;

  if (cost)
    take_payment(payer, cost);
  who->add_timed_property(RAISE_COOLDOWN_PROP, 1, RAISE_COOLDOWN);
  return 1;
}

/**
 * Pays the debt to this raiser with what the player carries, all of it or as
 * much as covers; what is left stays owed.
 */
int do_pay(string str)
{
  object who, payer;
  mapping debt;
  string price_text, left_text;
  int owed, paid;

  who = this_player();
  if (!who || !query_entity())
    return 0;

  debt = _query_debt(who);
  if (!debt)
  {
    query_entity()->do_say(_LANG_RAISE_NO_DEBT);
    return 1;
  }

  owed = debt["amount"];
  paid = query_purse_value(who);
  if (paid > owed)
    paid = owed;
  payer = paid > 0 ? find_payer(who, paid, 0) : nil;
  if (!payer || !take_payment(payer, paid))
  {
    price_text = price_words(owed);
    query_entity()->do_say(_LANG_RAISE_DEBT_NO_MONEY);
    return 1;
  }

  _set_debt(who, owed - paid);
  if (paid >= owed)
  {
    query_entity()->do_say(_LANG_RAISE_DEBT_SETTLED);
    return 1;
  }

  price_text = price_words(paid);
  left_text = price_words(owed - paid);
  query_entity()->do_say(_LANG_RAISE_DEBT_PART_PAID);
  return 1;
}

mapping query_auto_load_attributes()
{
  mapping out;

  out = component::query_auto_load_attributes();
  if (map_sizeof(raise_only))
    out["only"] = raise_only;
  if (raise_refusal)
    out["refusal"] = raise_refusal;
  if (map_sizeof(raise_price))
    out["price"] = raise_price;
  if (raise_spell)
    out["spell"] = raise_spell;
  if (raise_level > 0)
    out["level"] = raise_level;
  return out;
}

void init_auto_load_attributes(mapping args)
{
  component::init_auto_load_attributes(args);
  if (!args)
    return;
  if (mappingp(args["only"]))
    raise_only = ([ ]) + args["only"];
  if (stringp(args["refusal"]))
    raise_refusal = args["refusal"];
  if (mappingp(args["price"]))
    raise_price = ([ ]) + args["price"];
  if (stringp(args["spell"]))
    raise_spell = args["spell"];
  if (intp(args["level"]))
    raise_level = args["level"];
}
