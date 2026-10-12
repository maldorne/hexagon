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
//   string query_climb_refusal(object who) -- why the climber cannot now
//
// 'climb' alone climbs the place one stands in, if it answers the above (the
// way back up onto a bridge from the water).

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

  // the target is looked for here: what is climbed may be the place itself
  set_target_type(TARGET_TYPE_PASS);
  set_range(0);

  add_category(SKILL_TYPE_BASIC, 1);

  allow_while_mounted = 0;
  allow_while_pacified = 0;
  allow_while_hidden = 1;
  allow_on_water = 1;
  allow_while_moving = 0;
  allow_in_combat = 0;

  set_help_desc(_LANG_SKILL_CLIMB_HELP);

  set_start_phrases(_LANG_SKILL_CLIMB_START, "");

  set_rounds(({ "start_climb", "do_climb" }));
}

// What is climbed: the item named, or the place when nothing is named.
object query_climb_target(object caster, mixed str)
{
  object ob;

  if (!stringp(str) || !strlen(trim(str)))
    return environment(caster)->query_climbable() ? environment(caster) : nil;

  // not on anybody else's inventory, only on the caster's or the place's
  ob = find_one_match(str, caster);
  if (!ob)
    ob = find_one_match(str, environment(caster), !allow_on_dead);
  if (ob && (ob->query_npc() || ob->query_player()))
    return nil;

  return ob;
}

int start_climb(object caster, mixed target, mixed out_range, int time, int quiet)
{
  object ob;
  string refusal;

  ob = query_climb_target(caster, target);
  if (!ob || !ob->query_climbable())
  {
    tell_object(caster, (!stringp(target) || !strlen(trim(target))) ?
                _LANG_SKILL_CLIMB_WHAT : _LANG_SKILL_CLIMB_NOT_CLIMBABLE);
    return 0;
  }

  refusal = ob->query_climb_refusal(caster);
  if (stringp(refusal))
  {
    tell_object(caster, refusal);
    return 0;
  }

  tell_object(caster, _LANG_SKILL_CLIMB_ROUND1);
  if (ob != environment(caster))
    tell_room(environment(caster), _LANG_SKILL_CLIMB_ROUND1_ROOM, caster);
  return 1;
}

int do_climb(object caster, mixed target, mixed out_range, int time, int quiet)
{
  object ob;
  int chance;

  ob = query_climb_target(caster, target);
  if (!ob || !ob->query_climbable())
    return 0;

  chance = caster->query_skill_ability(SKILL_CLIMB) +
           ob->query_climb_modifier(caster) +
           caster->query_skill_malus() * 3;

  // a sure climb is not rolled: the thing climbed has said it cannot fail
  if (chance >= 100 || random(100) + 1 <= chance)
  {
    if (function_exists("event_climbed", ob))
      ob->event_climbed(caster);
    else
      tell_object(caster, _LANG_SKILL_CLIMB_SUCCESS);
    return 1;
  }

  if (function_exists("event_climb_failed", ob))
    ob->event_climb_failed(caster);
  else
    tell_object(caster, _LANG_SKILL_CLIMB_FAIL);
  return 0;
}
