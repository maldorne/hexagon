// Swim — active skill: 'swim <direction>' in water, or from a coast into it.
// The place decides everything: where one can swim, how hard each stroke is
// and what happens to whoever fails (see /lib/room/water.c and
// /lib/room/coast.c). 'swim' alone says where one can swim from here.
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

  set_rounds(({ "start_swim", "do_swim" }));
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

// The exit a typed direction stands for, if one can swim out by it.
string query_swim_exit(object caster, string str)
{
  object env, place;
  string dir, * dirs;
  int i;

  env = environment(caster);
  place = query_swim_place(env);
  if (!place || !str)
    return nil;

  str = trim(str);
  dirs = place->query_swim_directions(caster);

  dir = env->query_verb_exit(str);
  if (dir && member_array(dir, dirs) != -1)
    return dir;

  // a direction typed short, as the exit line shows it
  for (i = 0; i < sizeof(dirs); i++)
    if (EXIT_HAND->query_shorten(dirs[i]) == str)
      return dirs[i];

  return nil;
}

mixed extra_checks(string str, object caster)
{
  object env, place;
  string line;

  env = environment(caster);
  place = query_swim_place(env);

  if (!place || (!env->query_water_environment() &&
                 !sizeof(place->query_swim_directions(caster))))
    return _LANG_SKILL_SWIM_NO_WATER;

  // nothing typed: where one can swim from here
  if (!str || !strlen(trim(str)))
  {
    if (env->query_water_environment())
      line = place->query_water_dirs_string(caster) + "\n";
    else
      line = place->query_coast_look(caster);
    return line;
  }

  if (!query_swim_exit(caster, str))
    return _LANG_SKILL_SWIM_NOT_THERE;

  return 0;
}

int start_swim(object caster, mixed target, mixed out_range, int time, int quiet)
{
  if (!query_swim_exit(caster, target))
    return 0;

  tell_object(caster, _LANG_SKILL_SWIM_ROUND1);
  return 1;
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
