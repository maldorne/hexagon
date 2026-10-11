// Follow — active skill: 'follow <somebody>'. From then on the follower goes
// wherever that somebody goes (see /lib/living/follow.c). Somebody who has
// the follow consent on is followed without a roll; anybody else is rolled
// for, this skill against their evade skill, and shakes the follower off on
// a failure. A hidden living cannot be followed: it has to be found first.
// 'stop following' (the stop command) ends it, and so does the evade skill.

#include <living/effects.h>
#include <living/skills.h>
#include <living/consents.h>
#include <language.h>

inherit BASE_EFFECT;

void setup()
{
  // Display name + aliases: the translated name is the verb itself
  set_effect_name(_LANG_SKILL_FOLLOW_NAME);
  set_aliases(_LANG_SKILL_FOLLOW_ALIASES);

  set_gp_cost(2);
  set_lockout_time(5);

  set_target_type(TARGET_TYPE_ONE);
  set_range(0);

  add_category(SKILL_TYPE_BASIC, 1);

  allow_on_self = 0;
  allow_on_hidden = 0;
  allow_while_hidden = 1;
  allow_in_combat = 1;

  set_help_desc(_LANG_SKILL_FOLLOW_HELP);
  set_help_see_also(({ _LANG_SKILL_EVADE_NAME }));

  set_start_phrases("", "");
  set_fast_casting(1);
  set_rounds(({ "do_follow" }));
}

int do_follow(object caster, mixed target, mixed out_range, int time, int quiet)
{
  int chance;

  if (!target || !living(target) || target == caster)
  {
    tell_object(caster, _LANG_SKILL_FOLLOW_NOBODY);
    return 1;
  }

  if (caster->query_following() == target)
  {
    tell_object(caster, _LANG_SKILL_FOLLOW_ALREADY);
    return 1;
  }

  caster->update_skill_used_times(SKILL_FOLLOW);

  // nothing to roll against somebody who lets anybody follow them
  if (!target->query_consent(FOLLOW_CONS))
  {
    chance = 50 + caster->query_skill_ability(SKILL_FOLLOW) -
             target->query_skill_ability(SKILL_EVADE);
    if (chance < 5)
      chance = 5;
    if (chance > 95)
      chance = 95;

    if (random(100) + 1 > chance)
    {
      tell_object(caster, _LANG_SKILL_FOLLOW_FAIL_ME);
      tell_object(target, _LANG_SKILL_FOLLOW_FAIL_TARGET);
      return 1;
    }
  }

  if (!caster->start_following(target))
  {
    tell_object(caster, _LANG_SKILL_FOLLOW_CIRCLE);
    return 1;
  }

  tell_object(caster, _LANG_SKILL_FOLLOW_ME);
  tell_object(target, _LANG_SKILL_FOLLOW_TARGET);
  tell_room(environment(caster), _LANG_SKILL_FOLLOW_ROOM, ({ caster, target }));
  return 1;
}
