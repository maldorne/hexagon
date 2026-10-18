// Focus (centrar) — active skill: 'focus <someone>' / 'focus off'. Keeps every
// blow of the character on one of the opponents for a while, instead of the
// one the fight would pick. Ported from the centrar command of the RL muds
// (std/commands/centrar.c of ancient-kingdoms, Wonderflug).
//
// The combat code forgets a concentrate target after one round, so a lasting
// effect (see /lib/living/effects.c) renews it every heartbeat until the time
// (two heartbeats per level) runs out, the target leaves the fight or the
// place, or the character is too tired to pay the point of energy each combat
// round costs.
//
// Each renewal also draws the target's attention: its aggro against the
// character (see adjust_aggro in /lib/living/health.c, which decides whom a
// living attacks) grows by the character's level, twice that in the defensive
// attitude (TANK_ROLE) and nothing in the supportive one (HEALER_ROLE).

#include <living/effects.h>
#include <living/skills.h>
#include <living/combat.h>
#include <language.h>

inherit BASE_EFFECT;

// renewals (heartbeats) per level of the character
#define FOCUS_TICKS_PER_LEVEL 2
// a renewal in every this many costs a point of energy (one per combat round)
#define FOCUS_COST_EVERY 2
// aggro drawn by each renewal, for each level of the character; the defensive
// attitude multiplies it
#define FOCUS_AGGRO_PER_LEVEL 1
#define FOCUS_AGGRO_TANK_FACTOR 2

void setup()
{
  // Display name + aliases; the learning key is SKILL_FOCUS.
  set_effect_name(_LANG_SKILL_FOCUS_NAME);
  set_aliases(_LANG_SKILL_FOCUS_ALIASES);

  set_gp_cost(1);
  set_lockout_time(2);

  set_target_type(TARGET_TYPE_PASS);
  set_range(0);

  add_category(SKILL_TYPE_ARMED, 1);

  allow_while_mounted = 1;
  allow_while_hidden = 1;

  set_help_desc(_LANG_SKILL_FOCUS_HELP);

  set_start_phrases("", "");
  set_fast_casting(1);
  set_rounds(({ "do_focus" }));
}

/**
 * Whether the word asks to stop focusing.
 */
private int is_off_word(string str)
{
  return member_array(lower_case(trim(str)), _LANG_SKILL_FOCUS_OFF_WORDS) != -1;
}

mixed extra_checks(string str, object caster)
{
  object ob;

  if (!str || !strlen(trim(str)))
    return _LANG_SKILL_FOCUS_SYNTAX;

  if (is_off_word(str))
  {
    if (!query_lasting(caster))
      return _LANG_SKILL_FOCUS_NOT_FOCUSED;
    return nil;
  }

  ob = find_one_match(str, environment(caster), 1);
  if (!ob || !living(ob) ||
      member_array(ob, caster->query_attacker_list()) == -1)
    return _LANG_SKILL_FOCUS_NOT_FIGHTING;

  return nil;
}

/**
 * Makes the target keener on the caster, as far as their attitude allows.
 */
private void draw_attention(object caster, object target)
{
  int amount;

  amount = query_casting_level(caster) * FOCUS_AGGRO_PER_LEVEL;
  if (amount < FOCUS_AGGRO_PER_LEVEL)
    amount = FOCUS_AGGRO_PER_LEVEL;

  switch (caster->query_combat_role())
  {
    case HEALER_ROLE:
      return;
    case TANK_ROLE:
      amount *= FOCUS_AGGRO_TANK_FACTOR;
      break;
  }

  // aggro is kept negative: the most negative one is the one attacked
  target->adjust_aggro(-amount, caster);
}

/**
 * One renewal of the focus, every heartbeat while it lasts; returning 0 ends
 * it.
 */
int lasting_tick(object caster, mixed data, int ticks_left)
{
  object target;

  target = data;

  if (!target || target->query_dead() ||
      environment(target) != environment(caster) ||
      member_array(target, caster->query_attacker_list()) == -1)
  {
    tell_object(caster, _LANG_SKILL_FOCUS_TARGET_GONE);
    return 0;
  }

  if (ticks_left % FOCUS_COST_EVERY == 0 && caster->adjust_gp(-1) < 0)
  {
    tell_object(caster, _LANG_SKILL_FOCUS_TIRED);
    return 0;
  }

  caster->set_concentrate(target);
  draw_attention(caster, target);
  return 1;
}

/**
 * The focus ended: the time ran out or something cut it short. The tick and
 * 'focus off' give their own messages, and the dead need none.
 */
void lasting_end(object caster, string id, mixed data, string reason)
{
  switch (reason)
  {
    case LASTING_END_EXPIRED:
    case LASTING_END_ERROR:
    case LASTING_END_CLEARED:
      tell_object(caster, _LANG_SKILL_FOCUS_LOST);
      break;
  }
}

int do_focus(object caster, mixed target, mixed out_range, int time, int quiet)
{
  object ob;
  int ticks;

  if (is_off_word(target))
  {
    stop_lasting(caster);
    tell_object(caster, _LANG_SKILL_FOCUS_OFF);
    return 1;
  }

  ob = find_one_match(target, environment(caster), 1);
  if (!ob || !caster->set_concentrate(ob))
  {
    tell_object(caster, _LANG_SKILL_FOCUS_NOT_FIGHTING);
    return 0;
  }

  draw_attention(caster, ob);

  ticks = query_casting_level(caster) * FOCUS_TICKS_PER_LEVEL;
  if (ticks < FOCUS_TICKS_PER_LEVEL)
    ticks = FOCUS_TICKS_PER_LEVEL;

  tell_object(caster, _LANG_SKILL_FOCUS_START_ME);
  tell_room(environment(caster), _LANG_SKILL_FOCUS_START_ROOM, caster);

  // a new focus replaces the one in force
  start_lasting(caster, ticks, ob);
  return 1;
}
