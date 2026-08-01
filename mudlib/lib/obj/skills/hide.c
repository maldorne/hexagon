// Hide (esconderse) — active skill. On success it clones a hide shadow onto
// the character, making them invisible in the room until they act or a
// searcher finds them (see shadows/hide_sh.c and the search skill). Ported
// from the old CcMud feat, Folken 10/2009.

#include <living/effects.h>
#include <living/skills.h>
#include <language.h>

inherit BASE_EFFECT;

#define HIDE_SHADOW "/lib/obj/skills/hide_sh.c"
#define HIDE_COST   10

void setup()
{
  // Display name + accent-less aliases; the learning key is SKILL_HIDE.
  set_effect_name(_LANG_SKILL_HIDE_NAME);
  set_aliases(_LANG_SKILL_HIDE_ALIASES);

  set_gp_cost(HIDE_COST);
  set_lockout_time(10);

  set_target_type(TARGET_TYPE_SELF);
  set_range(0);

  add_category(SKILL_TYPE_EXPLORATION, 1);

  allow_while_mounted = 0;
  allow_while_pacified = 0;
  allow_while_hidden = 0;
  allow_while_moving = 0;
  allow_in_combat = 0;

  set_help_desc(_LANG_SKILL_HIDE_HELP);

  // First message to the caster, second (room) unused.
  set_start_phrases(_LANG_SKILL_HIDE_START, "");

  set_rounds(({ "round1", "do_hide" }));
}

int round1(object caster, mixed target, mixed out_range, int time, int quiet)
{
  tell_object(caster, _LANG_SKILL_HIDE_ROUND1);
  return 1;
}

int do_hide(object caster, mixed target, mixed out_range, int time, int quiet)
{
  object room, shadow;
  int total, light_bonus;

  room = environment(caster);
  if (!room)
    return 0;

  total = caster->query_skill_ability(SKILL_HIDE);

  // Darkness helps; bright light hinders.
  if (room->query_light() <= 40)
    light_bonus = 15;
  else if (room->query_light() <= 80)
    light_bonus = 5;
  else if (room->query_light() >= 100)
    light_bonus = -5;

  total += light_bonus;
  total += caster->query_skill_malus() * 3;

  if (random(100) + 1 <= total)
  {
    tell_player(caster, _LANG_SKILL_HIDE_SUCCESS);

    // Succeeding costs extra effort.
    caster->adjust_gp(-HIDE_COST);

    shadow = clone_object(HIDE_SHADOW);
    shadow->setup_shadow(caster);
    return 1;
  }

  tell_object(caster, _LANG_SKILL_HIDE_FAIL);
  return 0;
}
