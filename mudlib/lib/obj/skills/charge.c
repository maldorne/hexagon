// Charge (cargar) — active skill: 'charge <someone>'. A rush at one opponent,
// shield first, to knock the breath out of them. Ported from the cargar command
// of the RL muds (std/commands/charge.c of ancient-kingdoms, Spp 99/2000,
// Aokromes 11/2001); the door-breaking mode is not ported.
//
// The insult that opens the charge starts the fight; the rush lands on the next
// round. The blow is rolled against the target's dexterity and charisma and
// against the caster's:
//
//   hit      = random(target cha + dex) - random(caster cha + dex) - level / 2 < 0
//   damage   = (two dice of 6 per 4 levels + twice the ac of the shield held,
//              or half the dice without a shield) * 30 / (20 + target's ac)
//
// A target who dodges leaves the charger hurt by a tenth of the damage, and
// once in a while stunned for a moment.

#include <living/effects.h>
#include <living/skills.h>
#include <common/properties.h>
#include <living/combat.h>
#include <language.h>

inherit BASE_EFFECT;

#define CHARGE_COST 10
#define CHARGE_LOCKOUT 8
// the dice a charge rolls: one per this many levels, one at least
#define CHARGE_LEVELS_PER_DIE 4
// chances (1 in this many) of staggering after a dodged charge
#define CHARGE_STUMBLE_ODDS 25
// how long the stagger lasts
#define CHARGE_STUMBLE_TIME 2

void setup()
{
  // Display name + aliases; the learning key is SKILL_CHARGE.
  set_effect_name(_LANG_SKILL_CHARGE_NAME);
  set_aliases(_LANG_SKILL_CHARGE_ALIASES);

  set_gp_cost(CHARGE_COST);
  set_lockout_time(CHARGE_LOCKOUT);

  set_target_type(TARGET_TYPE_ONE);
  set_range(0);

  add_category(SKILL_TYPE_ARMED, 1);

  allow_on_self = 0;
  allow_while_mounted = 0;
  allow_while_hidden = 1;

  set_help_desc(_LANG_SKILL_CHARGE_HELP);
  set_help_see_also(({ _LANG_SKILL_FOCUS_NAME }));

  set_start_phrases("", "");

  // the challenge goes at once, the rush follows on the next round
  set_fast_casting(1);
  set_rounds(({ "challenge", "do_charge" }));
}

// The guild that teaches this may withhold it from a member who has strayed.
mixed extra_checks(string str, object caster)
{
  return query_guild_refusal(caster);
}

/**
 * The shield's armour class the caster holds, or the penalty of rushing
 * without one.
 */
private int shield_bonus(object caster)
{
  mixed * shields;
  int i, ac;

  shields = caster->query_shields_held();
  ac = 0;

  for (i = 0; i < sizeof(shields); i++)
    if (objectp(shields[i]))
      ac += shields[i]->query_shield_ac();

  return ac;
}

int challenge(object caster, mixed target, mixed out_range, int time, int quiet)
{
  string guild;

  if (!objectp(target) || target->query_dead())
  {
    tell_object(caster, _LANG_SKILL_CHARGE_NOTHING);
    return 0;
  }

  // no charging at a brother in arms
  guild = caster->query_guild_ob();
  if (guild && guild == target->query_guild_ob())
  {
    tell_object(caster, _LANG_SKILL_CHARGE_COMRADE);
    return 0;
  }

  tell_object(caster, SPE_ATT + _LANG_SKILL_CHARGE_CHALLENGE_ME);
  tell_object(target, SPE_DFF + _LANG_SKILL_CHARGE_CHALLENGE_TARGET);
  tell_room(environment(caster), _LANG_SKILL_CHARGE_CHALLENGE_ROOM,
            ({ caster, target }));

  caster->attack_ob(target);
  return 1;
}

int do_charge(object caster, mixed target, mixed out_range, int time, int quiet)
{
  object env;
  int level, damage, ac, dodge, shield;

  if (!objectp(target) || target->query_dead())
  {
    tell_object(caster, _LANG_SKILL_CHARGE_NO_TARGET);
    return 0;
  }

  env = environment(caster);
  if (environment(target) != env)
  {
    tell_object(caster, _LANG_SKILL_CHARGE_FLED_ME);
    tell_object(target, _LANG_SKILL_CHARGE_FLED_TARGET);
    return 0;
  }

  level = caster->query_level();
  if (level < 1)
    level = 1;

  tell_object(caster, SPE_ATT + _LANG_SKILL_CHARGE_RUSH_ME);
  tell_object(target, SPE_DFF + _LANG_SKILL_CHARGE_RUSH_TARGET);
  tell_room(env, _LANG_SKILL_CHARGE_RUSH_ROOM, ({ caster, target }));

  // a shield gives the weight to the blow; without one it is a plain shove
  damage = roll(1 + level / CHARGE_LEVELS_PER_DIE, 6) +
           roll(1 + level / CHARGE_LEVELS_PER_DIE, 6);
  shield = shield_bonus(caster);
  if (shield > 0)
    damage += shield * 2;
  else
    damage = damage / 2;

  // the target's armour cushions it
  ac = target->query_total_ac();
  if (ac < 10)
    ac = 10;
  damage = damage * 30 / (20 + ac);
  if (damage < 1)
    damage = 1;

  // the quicker and the more imposing of the two decides who gets out of the way
  dodge = random(target->query_cha() + target->query_dex()) -
          random(caster->query_cha() + caster->query_dex()) - level / 2;

  if (dodge >= 0)
  {
    tell_object(caster, SPE_FAIL + _LANG_SKILL_CHARGE_DODGED_ME);
    tell_object(target, SPE_FAIL + _LANG_SKILL_CHARGE_DODGED_TARGET);
    tell_room(env, _LANG_SKILL_CHARGE_DODGED_ROOM, ({ caster, target }));

    // the momentum hurts whoever carries it
    caster->adjust_hp(-(damage / 10), caster);

    if (!random(CHARGE_STUMBLE_ODDS))
    {
      tell_object(caster, _LANG_SKILL_CHARGE_STUMBLE_ME);
      tell_object(target, _LANG_SKILL_CHARGE_STUMBLE_TARGET);
      tell_room(env, _LANG_SKILL_CHARGE_STUMBLE_ROOM, ({ caster, target }));
      caster->add_timed_property(PASSED_OUT_PROP,
        _LANG_SKILL_CHARGE_STUMBLE_REASON, CHARGE_STUMBLE_TIME);
    }

    return 1;
  }

  tell_object(caster, SPE_ATT + _LANG_SKILL_CHARGE_HIT_ME);
  tell_object(target, SPE_DFF + _LANG_SKILL_CHARGE_HIT_TARGET);
  tell_room(env, _LANG_SKILL_CHARGE_HIT_ROOM, ({ caster, target }));

  target->adjust_hp(-damage, caster);
  return 1;
}
