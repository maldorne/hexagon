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
//               is n plus m for each level of the player, either key optional.
//               Without it the service is free. A death leaves a player's money
//               on the corpse, so a ghost pays from its own purse or, failing
//               that, from the corpse lying in the same place.

#include <language.h>

inherit component "/lib/npc/component.c";
inherit fee "/lib/npc/service-fee.c";

// How long, in heart beats, before the same player may ask again.
#define RAISE_COOLDOWN 100
// The timed property stamped on the player for that long.
#define RAISE_COOLDOWN_PROP "raise_cooldown"

private mapping raise_only;
private string raise_refusal;
private mapping raise_price;

void create()
{
  component::create();
  raise_only = ([ ]);
  raise_refusal = nil;
  raise_price = ([ ]);
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

  return out;
}

int do_info(string str)
{
  object who;
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
    query_entity()->do_say(_LANG_RAISE_INFO_PRICE);
  }
  return 1;
}

int do_raise(string str)
{
  object who, payer;
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
}
