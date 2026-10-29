// Sneak (sigilar) — active skill: a step in a direction that nobody sees,
// neither leaving nor arriving, and that keeps a hiding. The same roll lets a
// hidden living open or close a door in silence (see door.c). Ported from the
// ancient-kingdoms command (std/commands/sneak.c), neverbot 10/2026.

#include <living/effects.h>
#include <living/skills.h>
#include <common/properties.h>
#include <language.h>

inherit BASE_EFFECT;

// energy spent on every step made unseen in front of witnesses
#define SNEAK_COST      1
// a living shining more than this cannot sneak
#define SNEAK_MAX_LIGHT 20
// the place may forbid it with this property
#define NO_SNEAK_PROP   "no_sneak"

void setup()
{
  set_effect_name(_LANG_SKILL_SNEAK_NAME);
  set_aliases(_LANG_SKILL_SNEAK_ALIASES);

  // the cost is spent by do_sneak, only when there are witnesses
  set_gp_cost(0);
  set_lockout_time(0);
  set_target_type(TARGET_TYPE_PASS);
  set_range(0);
  add_category(SKILL_TYPE_EXPLORATION, 1);

  allow_while_mounted = 0;
  allow_while_hidden = 1;
  allow_in_combat = 0;
  allow_while_moving = 0;

  set_help_desc(_LANG_SKILL_SNEAK_HELP);
  set_start_phrases("", "");
  // the step is taken at once, inside the typed command
  set_fast_casting(1);
  set_rounds(({ "do_sneak" }));
}

/**
 * Livings in a place who could notice somebody: everyone but the walker who is
 * not dead, asleep or passed out.
 * @param who the one who moves or acts
 * @param place where to count; nil counts nothing
 */
int count_witnesses(object who, object place)
{
  object * obs;
  int i, total;

  if (!place)
    return 0;

  obs = all_inventory(place);
  for (i = 0; i < sizeof(obs); i++)
  {
    if (obs[i] == who || !living(obs[i]) || obs[i]->query_dead())
      continue;
    if (obs[i]->query_property(SLEEP_PROP) ||
        obs[i]->query_property(PASSED_OUT_PROP))
      continue;
    total++;
  }

  return total;
}

/**
 * Whether an action of `who` goes unnoticed. With nobody to notice it always
 * does, at no cost. With witnesses it takes a point of energy (without it no
 * roll is made and the action is noticed) and the roll of ability, dexterity,
 * darkness and armour; the point is spent only when the roll succeeds.
 * @param who the one who acts
 * @param place the place they are in
 * @param other another place that would notice too (where a step arrives), or nil
 */
int try_unnoticed(object who, object place, varargs object other)
{
  int total;

  if (!count_witnesses(who, place) && !count_witnesses(who, other))
    return 1;

  if (who->query_gp() < SNEAK_COST)
    return 0;

  // ability and dexterity; darkness helps; armour hinders. Never told.
  total = who->query_skill_ability(SKILL_SNEAK) +
          (who->query_dex() - 15) * 2 + who->query_skill_malus() * 3;
  if (place && place->query_light() <= 40)
    total += 10;

  if (random(100) + 1 > total)
    return 0;

  who->adjust_gp(-SNEAK_COST);
  return 1;
}

/**
 * Whether a hidden living opens or closes a door unnoticed (door.c). Only
 * somebody who is hiding and may use the skill gets the roll.
 */
int try_quiet_door(object who, object place, object other)
{
  if (!who || !who->query_hidden() || who->query_skill_ability(SKILL_SNEAK) < 1 ||
      !query_usable_by(who))
    return 0;

  return try_unnoticed(who, place, other);
}

mixed extra_checks(string str, object caster)
{
  object env;

  if (!str || !strlen(trim(str)))
    return _LANG_SKILL_SNEAK_WHICH_WAY;

  env = environment(caster);
  if (!env)
    return _LANG_SKILL_SNEAK_NOT_THERE;
  if (env->query_property(NO_SNEAK_PROP))
    return _LANG_SKILL_SNEAK_NO_SURFACE;
  if (caster->query_light() > SNEAK_MAX_LIGHT)
    return _LANG_SKILL_SNEAK_BRIGHT;
  if (!env->query_verb_exit(trim(str)))
    return _LANG_SKILL_SNEAK_NOT_THERE;

  return 0;
}

int do_sneak(object caster, mixed target, mixed out_range, int time, int quiet)
{
  object env, dest;
  string dir;
  int silent, moved, was, gp;

  env = environment(caster);
  dir = env ? env->query_verb_exit(trim((string)target)) : nil;
  if (!dir)
  {
    tell_object(caster, _LANG_SKILL_SNEAK_NOT_THERE);
    return 0;
  }

  // the place the step arrives in notices too
  catch(dest = env->query_dest_object(dir));
  gp = caster->query_gp();
  silent = try_unnoticed(caster, env, dest);

  was = caster->query_sneaking();
  if (silent)
    caster->set_sneaking(1);
  catch(moved = env->do_exit_command("", dir, caster));
  caster->set_sneaking(was);

  if (!moved)
  {
    // no step, no cost
    caster->adjust_gp(gp - caster->query_gp());
    tell_object(caster, _LANG_SKILL_SNEAK_STUCK);
    return 0;
  }

  tell_object(caster, _LANG_SKILL_SNEAK_GO);
  return 1;
}
