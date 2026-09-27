// Climb — active skill: 'climb <something>'. It rolls the character's ability
// against whatever is being climbed, and what happens at the top belongs to
// that thing, not to the skill.
//
// Anything can be climbed by answering these, all optional but the first:
//
//   int query_climbable()                  -- nonzero: it can be climbed
//   int query_climb_modifier(object who)   -- added to the climber's ability;
//                                             a chance of 100 or more is not
//                                             rolled at all, it just succeeds
//   void event_climbed(object who)         -- the climber made it up
//   void event_climb_failed(object who)    -- the climber slipped

#include <living/effects.h>
#include <living/skills.h>
#include <language.h>

inherit BASE_EFFECT;

#define CLIMB_COST 5

void setup()
{
  // Display name + aliases: the translated name is the verb itself
  set_effect_name(_LANG_SKILL_CLIMB_NAME);
  set_aliases(_LANG_SKILL_CLIMB_ALIASES);

  set_gp_cost(CLIMB_COST);
  set_lockout_time(5);

  set_target_type(TARGET_TYPE_ITEM);
  set_range(0);

  add_category(SKILL_TYPE_EXPLORATION, 1);

  allow_while_mounted = 0;
  allow_while_pacified = 0;
  allow_while_hidden = 1;
  allow_while_moving = 0;
  allow_in_combat = 0;

  set_help_desc(_LANG_SKILL_CLIMB_HELP);

  set_start_phrases(_LANG_SKILL_CLIMB_START, "");

  set_rounds(({ "start_climb", "do_climb" }));
}

int start_climb(object caster, mixed target, mixed out_range, int time, int quiet)
{
  if (!target || !target->query_climbable())
  {
    tell_object(caster, _LANG_SKILL_CLIMB_NOT_CLIMBABLE);
    return 0;
  }

  tell_object(caster, _LANG_SKILL_CLIMB_ROUND1);
  tell_room(environment(caster), _LANG_SKILL_CLIMB_ROUND1_ROOM, caster);
  return 1;
}

int do_climb(object caster, mixed target, mixed out_range, int time, int quiet)
{
  int chance;

  if (!target || !target->query_climbable())
    return 0;

  chance = caster->query_skill_ability(SKILL_CLIMB) +
           target->query_climb_modifier(caster) +
           caster->query_skill_malus() * 3;

  // a sure climb is not rolled: the thing climbed has said it cannot fail
  if (chance >= 100 || random(100) + 1 <= chance)
  {
    if (function_exists("event_climbed", target))
      target->event_climbed(caster);
    else
      tell_object(caster, _LANG_SKILL_CLIMB_SUCCESS);
    return 1;
  }

  if (function_exists("event_climb_failed", target))
    target->event_climb_failed(caster);
  else
    tell_object(caster, _LANG_SKILL_CLIMB_FAIL);
  return 0;
}
