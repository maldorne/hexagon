// Slice (rebanar) — active skill. Spinning two slashing weapons, a series of
// quick cuts at one opponent, each of them a full attack with both blades, for
// a couple of rounds.
//
// Traduit per Oskuro, 1/12/97 (slice -> rebanar), from the slice command of
//   the RL muds.
// Kaitaka: the checks on the weapons, and not crashing when disarmed mid-slice.
// Radix: the event trigger on each blow.
// Iolo: slicing breaks the hiding.
// Reformado por completo por Radkar, Enero 2004: iluminado's own version, two
//   or three turns depending on the slicer's level.
// 10/2026 - Ported to Hexagon as a skill, neverbot, from the ancient-kingdoms
//           version (std/commands/slice.c), not Radkar's.

#include <living/effects.h>
#include <living/skills.h>
#include <living/combat.h>
#include <translations/combat.h>
#include <language.h>

inherit BASE_EFFECT;

// what each series of cuts costs, on top of the skill's own cost
#define SLICE_STRIKE_COST 3
// above this light the blades are easy to see coming
#define SLICE_BRIGHT_LIGHT 40

void setup()
{
  // Display name + aliases; the learning key is SKILL_SLICE.
  set_effect_name(_LANG_SKILL_SLICE_NAME);
  set_aliases(_LANG_SKILL_SLICE_ALIASES);

  set_gp_cost(9);
  set_lockout_time(10);

  set_target_type(TARGET_TYPE_ONE);
  set_range(0);

  add_category(SKILL_TYPE_ARMED, 1);

  allow_on_self = 0;
  allow_while_mounted = 0;

  set_help_desc(_LANG_SKILL_SLICE_HELP);

  set_start_phrases(_LANG_SKILL_SLICE_START, _LANG_SKILL_SLICE_START_ROOM);

  // the first series goes at once, a second one follows
  set_fast_casting(1);
  set_rounds(({ "round_slice", "round_slice" }));
}

// The two slashing weapons the caster holds, or fewer if they do not hold them.
private object * slicing_weapons(object caster)
{
  mixed * held;
  object * blades;
  int i;

  blades = ({ });
  held = caster->query_weapons_wielded();
  if (!pointerp(held))
    return blades;

  for (i = 0; i < sizeof(held); i++)
    if (objectp(held[i]) && held[i]->query_attack_type() == SLASHING &&
        !held[i]->query_property("no_slice"))
      blades += ({ held[i] });

  return blades;
}

mixed extra_checks(string str, object caster)
{
  if (sizeof(slicing_weapons(caster)) < 2)
    return _LANG_SKILL_SLICE_NO_WEAPONS;
  return 0;
}

// How many cuts make one series: more with dexterity and level.
private int cuts_per_series(object caster)
{
  int x;

  x = caster->query_dex() + caster->query_level();
  if (x < 30)
    return 1;
  if (x < 50)
    return 2;
  return 3;
}

int round_slice(object caster, mixed target, mixed out_range, int time, int quiet)
{
  object * blades;
  object env;
  int bonus, damage_bonus, cuts, i, j;

  if (!objectp(target))
    return 0;

  // the weapons may have changed hands since the first series
  blades = slicing_weapons(caster);
  if (sizeof(blades) < 2)
  {
    tell_object(caster, _LANG_SKILL_SLICE_LOST_WEAPONS);
    return 0;
  }

  if (caster->query_gp() < SLICE_STRIKE_COST)
  {
    tell_object(caster, _LANG_SKILL_SLICE_TIRED);
    return 0;
  }
  caster->adjust_gp(-SLICE_STRIKE_COST);

  // Dexterity and practice sharpen the cuts; bright light gives them away.
  env = environment(caster);
  bonus = (caster->query_dex() - 15) / 2 +
          caster->query_skill_ability(SKILL_SLICE) / 20;
  if (env && env->query_light() > SLICE_BRIGHT_LIGHT)
    bonus -= 3;
  damage_bonus = bonus > 0 ? bonus / 2 : 0;

  tell_object(caster, SPE_ATT + _LANG_SKILL_SLICE_ME);
  tell_object(target, SPE_DFF + _LANG_SKILL_SLICE_TARGET);
  if (env)
    tell_room(env, _LANG_SKILL_SLICE_ROOM, ({ caster, target }));

  caster->attack_ob(target);

  caster->adjust_tmp_tohit_bon(bonus);
  caster->adjust_tmp_damage_bon(damage_bonus);

  cuts = cuts_per_series(caster);
  for (i = 0; i < cuts && objectp(target) && !target->query_dead(); i++)
    for (j = 0; j < 2; j++)
      blades[j]->weapon_attack(target, caster);

  caster->adjust_tmp_tohit_bon(-bonus);
  caster->adjust_tmp_damage_bon(-damage_bonus);

  return 1;
}
