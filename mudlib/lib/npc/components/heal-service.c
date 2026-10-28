// heal-service.c -- healing the wounds of whoever asks, for a price, as an NPC
// component.
//
// A service somebody sells, the counterpart of the raise component for the
// living (healer.c is another thing: an NPC that heals its own side in a
// fight). It asks the NPC for two verbs: one to explain the service, one to
// request it. A player in a fight is never served.
//
// Attributes, every one optional (store only what differs from the default):
//   "price_per_hp" - base coins for each hit point healed; without it the
//                    service is free
//   "max"          - the most hit points healed in one request (default: all
//                    the player is missing)
//   "cooldown"     - heart beats before the same player may ask again
//                    (default: none)
//   "cures"        - traits closed on request, free of charge, with the reason
//                    "cured": ({ "bleeding", "poisoned" }); the player must
//                    carry them. A player missing no hit points is still served
//                    when they carry one.
//   "only"         - who is served, as in the raise component
//   "refusal"      - what somebody left out is told; defaults to the stock text

#include <language.h>
#include <living/traits.h>

inherit component "/lib/npc/component.c";
inherit fee "/lib/npc/service-fee.c";

// The timed property stamped on the player while the cooldown lasts.
#define HEAL_COOLDOWN_PROP "heal_service_cooldown"

private int heal_price_per_hp, heal_max, heal_cooldown;
private string * heal_cures;
private mapping heal_only;
private string heal_refusal;

void create()
{
  component::create();
  heal_price_per_hp = 0;
  heal_max = 0;
  heal_cooldown = 0;
  heal_cures = ({ });
  heal_only = ([ ]);
  heal_refusal = nil;
}

mapping query_component_actions()
{
  mapping out;
  string * verbs;
  int i;

  out = ([ ]);

  verbs = _LANG_HEAL_SERVICE_INFO_VERBS;
  for (i = 0; i < sizeof(verbs); i++)
    out[verbs[i]] = "do_info";

  verbs = _LANG_HEAL_SERVICE_VERBS;
  for (i = 0; i < sizeof(verbs); i++)
    out[verbs[i]] = "do_heal";

  return out;
}

/**
 * The hit points the service would heal `who` now.
 *
 * @param who the player asking
 * @return the hit points missing, capped by "max"; 0 when none
 */
private int _query_to_heal(object who)
{
  int missing;

  missing = who->query_max_hp() - who->query_hp();
  if (missing < 0)
    missing = 0;
  if (heal_max > 0 && missing > heal_max)
    missing = heal_max;
  return missing;
}

/**
 * The traits of the "cures" attribute that `who` carries.
 *
 * @param who the player asking
 * @return their ids
 */
private string * _query_curable(object who)
{
  string * found;
  int i;

  found = ({ });
  for (i = 0; i < sizeof(heal_cures); i++)
    if (who->has_trait(heal_cures[i]))
      found += ({ heal_cures[i] });
  return found;
}

int do_info(string str)
{
  object who;
  string price_text;
  int cost;

  if (!query_entity())
    return 0;

  query_entity()->do_say(_LANG_HEAL_SERVICE_INFO);

  if (heal_price_per_hp > 0)
  {
    price_text = price_words(heal_price_per_hp);
    query_entity()->do_say(_LANG_HEAL_SERVICE_INFO_PRICE);
  }

  who = this_player();
  if (who && heal_price_per_hp > 0 && _query_to_heal(who) > 0)
  {
    cost = _query_to_heal(who) * heal_price_per_hp;
    price_text = price_words(cost);
    query_entity()->do_say(_LANG_HEAL_SERVICE_INFO_YOURS);
  }
  return 1;
}

int do_heal(string str)
{
  object who, payer;
  string price_text, * curable;
  int cost, amount, i;

  who = this_player();
  if (!who || !query_entity())
    return 0;

  if (who->query_dead())
  {
    query_entity()->do_say(_LANG_HEAL_SERVICE_DEAD);
    return 1;
  }

  if (who->query_is_fighting())
  {
    query_entity()->do_say(_LANG_HEAL_SERVICE_FIGHTING);
    return 1;
  }

  if (!meets_only(who, heal_only))
  {
    query_entity()->do_say(heal_refusal ? heal_refusal : _LANG_HEAL_SERVICE_REFUSED);
    return 1;
  }

  if (who->query_timed_property(HEAL_COOLDOWN_PROP))
  {
    query_entity()->do_say(_LANG_HEAL_SERVICE_TOO_SOON);
    return 1;
  }

  amount = _query_to_heal(who);
  curable = _query_curable(who);
  if (!amount && !sizeof(curable))
  {
    query_entity()->do_say(_LANG_HEAL_SERVICE_NOT_NEEDED);
    return 1;
  }

  // Nothing is taken until the player can pay the whole of it
  cost = amount * heal_price_per_hp;
  if (cost > 0)
  {
    price_text = price_words(cost);
    payer = find_payer(who, cost, 0);
    if (!payer)
    {
      query_entity()->do_say(_LANG_HEAL_SERVICE_CANNOT_PAY);
      return 1;
    }
    take_payment(payer, cost);
  }

  tell_object(who, _LANG_HEAL_SERVICE_YOU);
  tell_room(environment(who), _LANG_HEAL_SERVICE_ROOM, who);

  if (amount > 0)
    who->adjust_hp(amount);
  for (i = 0; i < sizeof(curable); i++)
    who->remove_trait(curable[i], TRAIT_END_CURED);

  if (heal_cooldown > 0)
    who->add_timed_property(HEAL_COOLDOWN_PROP, 1, heal_cooldown);
  return 1;
}

mapping query_auto_load_attributes()
{
  mapping out;

  out = component::query_auto_load_attributes();
  if (heal_price_per_hp > 0)
    out["price_per_hp"] = heal_price_per_hp;
  if (heal_max > 0)
    out["max"] = heal_max;
  if (heal_cooldown > 0)
    out["cooldown"] = heal_cooldown;
  if (sizeof(heal_cures))
    out["cures"] = heal_cures;
  if (map_sizeof(heal_only))
    out["only"] = heal_only;
  if (heal_refusal)
    out["refusal"] = heal_refusal;
  return out;
}

void init_auto_load_attributes(mapping args)
{
  component::init_auto_load_attributes(args);
  if (!args)
    return;
  if (intp(args["price_per_hp"]))
    heal_price_per_hp = args["price_per_hp"];
  if (intp(args["max"]))
    heal_max = args["max"];
  if (intp(args["cooldown"]))
    heal_cooldown = args["cooldown"];
  if (pointerp(args["cures"]))
    heal_cures = ({ }) + args["cures"];
  if (mappingp(args["only"]))
    heal_only = ([ ]) + args["only"];
  if (stringp(args["refusal"]))
    heal_refusal = args["refusal"];
}
