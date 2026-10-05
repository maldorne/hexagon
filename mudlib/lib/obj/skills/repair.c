// Repair — active skill: 'repair <weapon or armour>'. Mends a metal weapon,
// armour or shield of one's own at a smithy whose forge is lit (the blacksmith
// location component), paying for the metal and the coal it uses.
//
// Rolled against the character's ability. The better the smith, the more of the
// wear is taken out in one go and the less the materials cost; a failed attempt
// costs nothing but the effort, and never harms the item.
//
//   chance   = 30 + ability * 7 / 10, at most 95
//   restored = missing * (50 + ability) / 100, at least 1 (the whole of it from
//              ability 50 up)
//   price    = restored * the type's cost to fix * (100 - ability / 2) / 100,
//              at least 1

#include <living/effects.h>
#include <living/skills.h>
#include <room/location.h>
#include <item/item.h>
#include <basic/condition.h>
#include <language.h>

inherit BASE_EFFECT;

#define REPAIR_COST 5

// the encoding the weapon, armour and shield tables use for metal
#define MATERIAL_METAL 2

void setup()
{
  // Display name + aliases: the translated name is the verb itself
  set_effect_name(_LANG_SKILL_REPAIR_NAME);
  set_aliases(_LANG_SKILL_REPAIR_ALIASES);

  set_gp_cost(REPAIR_COST);
  set_lockout_time(5);

  set_target_type(TARGET_TYPE_ITEM);
  set_range(0);

  add_category(SKILL_TYPE_BASIC, 1);

  allow_while_mounted = 0;
  allow_while_pacified = 1;
  allow_while_hidden = 0;
  allow_while_moving = 0;
  allow_in_combat = 0;

  set_help_desc(_LANG_SKILL_REPAIR_HELP);

  set_start_phrases(_LANG_SKILL_REPAIR_START, "");

  set_rounds(({ "start_repair", "do_repair" }));
}

// Only at a smithy, and only while its forge is burning.
mixed extra_checks(string str, object caster)
{
  object forge;

  forge = (object)environment(caster)->query_component_by_type(
            LOCATION_COMPONENT_BLACKSMITH);

  if (!forge)
    return _LANG_SKILL_REPAIR_NO_FORGE;
  if (!forge->query_forge_lit())
    return _LANG_SKILL_REPAIR_FORGE_COLD;

  return 0;
}

// What mending one point of wear costs for this kind of item, or 0 when it is
// not something a smith works on.
private int cost_per_point(object item)
{
  if (item->query_weapon())
    return COST_MULTIPLIER_TO_FIX_WEAPON;
  if (item->query_shield())
    return COST_MULTIPLIER_TO_FIX_SHIELD;
  if (item->query_armour())
    return COST_MULTIPLIER_TO_FIX_ARMOUR;
  return 0;
}

private int points_to_restore(object caster, object item)
{
  int missing, restored;

  missing = (int)item->query_max_cond() - (int)item->query_cond();
  restored = missing * (50 + (int)caster->query_skill_ability(SKILL_REPAIR)) / 100;

  if (restored < 1)
    restored = 1;
  if (restored > missing)
    restored = missing;

  return restored;
}

private int price_of(object caster, object item, int restored)
{
  int price;

  price = restored * cost_per_point(item) *
          (100 - (int)caster->query_skill_ability(SKILL_REPAIR) / 2) / 100;

  return price < 1 ? 1 : price;
}

// Why this item cannot be worked here, or nil when it can.
private string refusal(object caster, object item)
{
  if (!item || environment(item) != caster)
    return _LANG_SKILL_REPAIR_NOT_CARRIED;
  if (!cost_per_point(item))
    return _LANG_SKILL_REPAIR_NOT_GEAR;
  if ((int)item->query_material() != MATERIAL_METAL)
    return _LANG_SKILL_REPAIR_NOT_METAL;
  if ((int)item->query_cond() >= (int)item->query_max_cond())
    return _LANG_SKILL_REPAIR_NOTHING_TO_DO;
  if ((int)caster->query_value() <
      price_of(caster, item, points_to_restore(caster, item)))
    return _LANG_SKILL_REPAIR_TOO_POOR;
  return nil;
}

int start_repair(object caster, mixed target, mixed out_range, int time, int quiet)
{
  string why;

  if (why = refusal(caster, target))
  {
    tell_object(caster, why);
    return 0;
  }

  tell_object(caster, _LANG_SKILL_REPAIR_ROUND1);
  tell_room(environment(caster), _LANG_SKILL_REPAIR_ROUND1_ROOM, caster);
  return 1;
}

int do_repair(object caster, mixed target, mixed out_range, int time, int quiet)
{
  mixed why;
  int chance, restored, price;
  mixed * money;

  // the item may have been handed over or the forge left to go out meanwhile
  why = refusal(caster, target);
  if (!why)
    why = extra_checks("", caster);
  if (stringp(why))
  {
    tell_object(caster, why);
    return 0;
  }

  chance = 30 + (int)caster->query_skill_ability(SKILL_REPAIR) * 7 / 10 +
           (int)caster->query_skill_malus() * 3;
  if (chance > 95)
    chance = 95;

  if (random(100) + 1 > chance)
  {
    tell_object(caster, _LANG_SKILL_REPAIR_FAIL);
    tell_room(environment(caster), _LANG_SKILL_REPAIR_FAIL_ROOM, caster);
    return 0;
  }

  restored = points_to_restore(caster, target);
  price = price_of(caster, target, restored);

  money = (mixed *)handler("money")->create_money_array(price);
  caster->pay_money(money);
  target->adjust_cond(restored);

  tell_object(caster, _LANG_SKILL_REPAIR_SUCCESS);
  tell_room(environment(caster), _LANG_SKILL_REPAIR_SUCCESS_ROOM, caster);
  return 1;
}
