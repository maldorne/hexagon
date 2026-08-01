// Search — active skill. Searches the current room for hidden objects and
// reveals them, scaling with the character's ability. Pairs with the hide
// skill (which is what puts objects into the room's hidden list).

#include <living/effects.h>
#include <living/skills.h>
#include <language.h>

inherit BASE_EFFECT;

void setup()
{
  // Display name + accent-less aliases (translated per language). The
  // learning key is the English id SKILL_SEARCH (= this filename).
  set_effect_name(_LANG_SKILL_SEARCH_NAME);
  set_aliases(_LANG_SKILL_SEARCH_ALIASES);

  set_gp_cost(2);
  set_target_type(TARGET_TYPE_NONE);
  set_range(0);

  add_category(SKILL_TYPE_BASIC, 1);

  // Cannot search in combat, while hidden, or as a spirit.
  allow_in_combat = 0;
  allow_while_hidden = 0;
  allow_on_dead = 0;
  allow_while_dead = 0;
  set_lockout_time(0);

  set_help_desc(_LANG_SKILL_SEARCH_HELP);

  // No start-cast phrases; it is an instant effect.
  set_start_phrases("", "");
  set_fast_casting(1);

  set_rounds(({ "do_search" }));
}

int do_search(object caster, mixed target, mixed out_range, int time, int quiet)
{
  object env;
  object * hidden;
  object * found;
  string res;
  int ability, i;

  env = environment(caster);
  if (!env)
    return 0;

  ability = caster->query_skill_ability(SKILL_SEARCH);

  hidden = env->query_hidden_objects();
  if (!hidden)
    hidden = ({ });
  hidden -= ({ nil });

  // Each hidden object is found independently on an ability check.
  found = ({ });
  for (i = 0; i < sizeof(hidden); i++)
    if (random(100) < ability)
      found += ({ hidden[i] });

  for (i = 0; i < sizeof(found); i++)
    env->remove_hidden_object(found[i]);

  if (!quiet)
  {
    tell_object(caster, _LANG_SKILL_SEARCH_START);
    tell_room(env, caster->query_cap_name() + _LANG_SKILL_SEARCH_ROOM,
      ({ caster }));
  }

  if (sizeof(found))
  {
    res = _LANG_SKILL_SEARCH_FOUND;
    for (i = 0; i < sizeof(found); i++)
      res += "  " + found[i]->short() + "\n";
    tell_object(caster, res);
  }
  else
    tell_object(caster, _LANG_SKILL_SEARCH_NOTHING);

  event(env, "player_search");
  return 1;
}
