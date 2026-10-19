// Evade — active skill: 'evade [somebody]'. Tries to shake off those who
// follow you and are with you, or only the one named, each rolled for: this
// skill against their follow skill. The better one's evade, the harder they
// are to follow at all (see the follow skill).

#include <living/effects.h>
#include <living/skills.h>
#include <language.h>

inherit BASE_EFFECT;

void setup()
{
  set_common_effect(1);

  // Display name + aliases: the translated name is the verb itself
  set_effect_name(_LANG_SKILL_EVADE_NAME);
  set_aliases(_LANG_SKILL_EVADE_ALIASES);

  set_gp_cost(2);
  set_lockout_time(5);

  set_target_type(TARGET_TYPE_PASS);
  set_range(0);

  add_category(SKILL_TYPE_BASIC, 1);

  allow_while_hidden = 1;
  allow_in_combat = 1;

  set_help_desc(_LANG_SKILL_EVADE_HELP);
  set_help_see_also(({ _LANG_SKILL_FOLLOW_NAME }));

  set_start_phrases("", "");
  set_fast_casting(1);
  set_rounds(({ "do_evade" }));
}

int do_evade(object caster, mixed target, mixed out_range, int time, int quiet)
{
  object * list;
  object follower;
  int i, chance;

  // those following the caster who are with them, or only the one named
  list = ({ });
  for (i = 0; i < sizeof(caster->query_followers()); i++)
  {
    follower = caster->query_followers()[i];
    if (follower && environment(follower) == environment(caster) &&
        (!stringp(target) || !strlen(target) || follower->id(target)))
      list += ({ follower });
  }

  if (!sizeof(list))
  {
    tell_object(caster, _LANG_SKILL_EVADE_NOBODY);
    return 1;
  }

  caster->update_skill_used_times(SKILL_EVADE);

  for (i = 0; i < sizeof(list); i++)
  {
    follower = list[i];
    chance = 50 + caster->query_skill_ability(SKILL_EVADE) -
             follower->query_skill_ability(SKILL_FOLLOW);
    if (chance < 5)
      chance = 5;
    if (chance > 95)
      chance = 95;

    if (random(100) + 1 <= chance)
    {
      follower->stop_following(1);
      tell_object(caster, _LANG_SKILL_EVADE_ME);
      tell_object(follower, _LANG_SKILL_EVADE_FOLLOWER);
    }
    else
      tell_object(caster, _LANG_SKILL_EVADE_FAIL_ME);
  }

  return 1;
}
