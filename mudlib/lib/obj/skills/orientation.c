// Orientation — passive skill. Measures the character's ability to find their
// way; queried by the movement / maze systems, it has no active command.

#include <living/effects.h>
#include <living/skills.h>
#include <language.h>

inherit BASE_EFFECT;

void setup()
{
  // Display name only
  set_effect_name(_LANG_SKILL_ORIENTATION_NAME);

  is_passive = 1;

  add_category(SKILL_TYPE_BASIC, 1);
  set_help_desc(_LANG_SKILL_ORIENTATION_HELP);
}
