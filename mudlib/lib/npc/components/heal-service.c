// heal-service.c -- healing whoever asks, for a price, as an NPC component.
//
// A service somebody sells, the counterpart of the raise component for the
// living (healer.c is another thing: an NPC that heals its own side in a
// fight). It asks the NPC for two verbs: one to explain the service, one to
// request it. For a fixed price the NPC casts one healing spell on whoever
// pays; what is healed and closed is what that spell does. A player in a
// fight is never served.
//
// Attributes, every one optional except "spell" (store only what differs from
// the default):
//   "spell"    - file of the spell cast on whoever pays
//   "price"    - base coins for each casting; without it the service is free
//   "level"    - guild level (1-20) the spell is cast at (default: the NPC's
//                guild level, or its level when it has no guild)
//   "cooldown" - heart beats before the same player may ask again
//                (default: none)
//   "only"     - who is served, as in the raise component
//   "refusal"  - what somebody left out is told; defaults to the stock text

#include <language.h>
#include <common/properties.h>
#include <living/effects.h>

inherit component "/lib/npc/component.c";
inherit fee "/lib/npc/service-fee.c";

// The timed property stamped on the player while the cooldown lasts.
#define HEAL_COOLDOWN_PROP "heal_service_cooldown"
#define HEAL_MAX_LEVEL 20
// the power the spell runs with, the neutral one: a service does not need the
// NPC to belong to the guild that teaches the spell
#define HEAL_POWER 2

private string heal_spell;
private int heal_price, heal_level, heal_cooldown;
private mapping heal_only;
private string heal_refusal;

void create()
{
  component::create();
  heal_spell = nil;
  heal_price = 0;
  heal_level = 0;
  heal_cooldown = 0;
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
 * The guild level the spell is cast at: the "level" attribute, or the NPC's
 * own guild level, or its level when it has no guild, capped to 1-20.
 */
private int _query_cast_level()
{
  int level;

  level = heal_level;
  if (level <= 0)
    level = query_entity()->query_guild_level();
  if (level <= 0)
    level = query_entity()->query_level();
  if (level > HEAL_MAX_LEVEL)
    level = HEAL_MAX_LEVEL;
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

  if (!heal_spell)
    return nil;
  spell = find_object(heal_spell);
  if (!spell)
    catch(spell = load_object(heal_spell));
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
  string price_text;

  if (!query_entity())
    return 0;

  query_entity()->do_say(_LANG_HEAL_SERVICE_INFO);

  if (heal_price > 0)
  {
    price_text = price_words(heal_price);
    query_entity()->do_say(_LANG_HEAL_SERVICE_INFO_PRICE);
  }
  return 1;
}

int do_heal(string str)
{
  object who, me, payer, spell;
  string price_text;

  who = this_player();
  me = query_entity();
  if (!who || !me)
    return 0;

  if (who->query_dead())
  {
    me->do_say(_LANG_HEAL_SERVICE_DEAD);
    return 1;
  }

  if (who->query_is_fighting())
  {
    me->do_say(_LANG_HEAL_SERVICE_FIGHTING);
    return 1;
  }

  if (!meets_only(who, heal_only))
  {
    me->do_say(heal_refusal ? heal_refusal : _LANG_HEAL_SERVICE_REFUSED);
    return 1;
  }

  if (who->query_timed_property(HEAL_COOLDOWN_PROP))
  {
    me->do_say(_LANG_HEAL_SERVICE_TOO_SOON);
    return 1;
  }

  // nothing to heal: no hit points missing and no trait carried
  if (who->query_hp() >= who->query_max_hp() && !map_sizeof(who->query_traits()))
  {
    me->do_say(_LANG_HEAL_SERVICE_NOT_NEEDED);
    return 1;
  }

  spell = _query_spell_object();
  if (!spell || _is_busy(me))
  {
    me->do_say(_LANG_HEAL_SERVICE_BUSY);
    return 1;
  }

  // Nothing is taken until the player can pay the whole of it
  payer = nil;
  if (heal_price > 0)
  {
    price_text = price_words(heal_price);
    payer = find_payer(who, heal_price, 0);
    if (!payer)
    {
      me->do_say(_LANG_HEAL_SERVICE_CANNOT_PAY);
      return 1;
    }
  }

  // and only once the spell has started
  if (!spell->cast_effect_at(who->query_name(), me, _query_cast_level(),
      HEAL_POWER))
  {
    me->do_say(_LANG_HEAL_SERVICE_BUSY);
    return 1;
  }

  if (payer)
    take_payment(payer, heal_price);
  me->do_say(_LANG_HEAL_SERVICE_START);

  if (heal_cooldown > 0)
    who->add_timed_property(HEAL_COOLDOWN_PROP, 1, heal_cooldown);
  return 1;
}

mapping query_auto_load_attributes()
{
  mapping out;

  out = component::query_auto_load_attributes();
  if (heal_spell)
    out["spell"] = heal_spell;
  if (heal_price > 0)
    out["price"] = heal_price;
  if (heal_level > 0)
    out["level"] = heal_level;
  if (heal_cooldown > 0)
    out["cooldown"] = heal_cooldown;
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
  if (stringp(args["spell"]))
    heal_spell = args["spell"];
  if (intp(args["price"]))
    heal_price = args["price"];
  if (intp(args["level"]))
    heal_level = args["level"];
  if (intp(args["cooldown"]))
    heal_cooldown = args["cooldown"];
  if (mappingp(args["only"]))
    heal_only = ([ ]) + args["only"];
  if (stringp(args["refusal"]))
    heal_refusal = args["refusal"];
}
