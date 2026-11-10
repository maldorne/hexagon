// Drive — passive skill. Measures how well the character drives a land vehicle. It has no active command: the riding handler reads
// it when somebody takes the driver's seat of a steed that asks for it (see
// /lib/handlers/riding.c).

#include <living/effects.h>
#include <living/skills.h>
#include <language.h>

inherit BASE_EFFECT;

void setup()
{
  set_common_effect(1);

  set_effect_name(_LANG_SKILL_DRIVE_NAME);
  set_aliases(_LANG_SKILL_DRIVE_ALIASES);

  is_passive = 1;

  add_category(SKILL_TYPE_BASIC, 1);
  set_help_desc(_LANG_SKILL_DRIVE_HELP);
}
