// Climb — passive skill. Measures how well the character climbs; whatever can be
// climbed rolls against it, so it has no active command of its own.

#include <living/effects.h>
#include <living/skills.h>
#include <language.h>

inherit BASE_EFFECT;

void setup()
{
  // Display name + accent-less aliases (both translated per language)
  set_effect_name(_LANG_SKILL_CLIMB_NAME);
  set_aliases(_LANG_SKILL_CLIMB_ALIASES);

  is_passive = 1;

  add_category(SKILL_TYPE_EXPLORATION, 1);
  set_help_desc(_LANG_SKILL_CLIMB_HELP);
}
