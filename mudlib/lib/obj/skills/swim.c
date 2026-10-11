// Swim — active skill: 'swim' alone from a coast into the water, 'swim
// <direction>' through it. The place decides everything: where one can swim,
// how hard each stroke is and what happens to whoever fails (see
// /lib/room/water.c and /lib/room/coast.c). In water, 'swim' alone says where
// the shore is.
//
// Nobody is taught it: whoever reaches water or a coast picks it up.

#include <living/effects.h>
#include <living/skills.h>
#include <room/location.h>
#include <room/room.h>
#include <language.h>

inherit BASE_EFFECT;

void setup()
{
  // Display name + aliases: the translated name is the verb itself
  set_effect_name(_LANG_SKILL_SWIM_NAME);
  set_aliases(_LANG_SKILL_SWIM_ALIASES);

  set_gp_cost(1);
  set_lockout_time(0);

  set_target_type(TARGET_TYPE_PASS);
  set_range(0);

  add_category(SKILL_TYPE_BASIC, 1);

  allow_while_mounted = 0;
  allow_while_pacified = 1;
  allow_while_hidden = 1;
  allow_while_moving = 0;
  allow_in_combat = 0;
  allow_on_water = 1;

  set_help_desc(_LANG_SKILL_SWIM_HELP);
  set_start_phrases(_LANG_SKILL_SWIM_START, _LANG_SKILL_SWIM_START_ROOM);

  // one stroke at once: nobody is left standing on the shore meanwhile
  set_fast_casting(1);
  set_rounds(({ "do_swim" }));
}

// What answers for swimming here: a water or coast component of a location,
// or the room itself when it is built on the water or coast base.
object query_swim_place(object env)
{
  object place;

  if (!env)
    return nil;

  if (function_exists("query_component_by_type", env))
  {
    place = env->query_component_by_type(LOCATION_COMPONENT_WATER);
    if (!place)
      place = env->query_component_by_type(LOCATION_COMPONENT_COAST);
    return place;
  }

  if (env->query_water_environment() || env->query_coast())
    return env;

  return nil;
}

// The exit a stroke goes out by: the typed direction if one can swim that way,
// or, from a coast with a single way into the water, that one with nothing
// typed.
string query_swim_exit(object caster, string str)
{
  object env, place;
  string dir, * dirs;
  int i;

  env = environment(caster);
  place = query_swim_place(env);
  if (!place)
    return nil;

  dirs = place->query_swim_directions(caster);

  if (!str || !strlen(trim(str)))
  {
    if (!env->query_water_environment() && sizeof(dirs) == 1)
      return dirs[0];
    return nil;
  }

  str = trim(str);
  dir = env->query_verb_exit(str);
  if (dir && member_array(dir, dirs) != -1)
    return dir;

  // a direction typed short, as the exits are shown
  for (i = 0; i < sizeof(dirs); i++)
    if (EXIT_HAND->query_shorten(dirs[i]) == str)
      return dirs[i];

  return nil;
}

mixed extra_checks(string str, object caster)
{
  object env, place;

  env = environment(caster);
  place = query_swim_place(env);

  if (!place || !sizeof(place->query_swim_directions(caster)))
    return _LANG_SKILL_SWIM_NO_WATER;

  if (query_swim_exit(caster, str))
    return 0;

  // nothing typed in the water: where the shore is
  if (!str || !strlen(trim(str)))
  {
    if (env->query_water_environment())
      return place->query_water_look(caster);
    return _LANG_SKILL_SWIM_WHICH_WAY;
  }

  return _LANG_SKILL_SWIM_NOT_THERE;
}

int do_swim(object caster, mixed target, mixed out_range, int time, int quiet)
{
  object place;
  string dir;

  dir = query_swim_exit(caster, target);
  if (!dir)
  {
    tell_object(caster, _LANG_SKILL_SWIM_NOT_THERE);
    return 0;
  }

  // a stroke that fails is still a stroke: the place has already told them
  place = query_swim_place(environment(caster));
  place->swim(caster, dir);
  return 1;
}
