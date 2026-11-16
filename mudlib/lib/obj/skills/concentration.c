// Concentration — passive skill. How well a caster holds on to what it is doing
// when it is hurt, and how fast it recovers from a spell. It has no command: the
// effects code (lib/living/effects.c, lib/effects/base_effect.c) reads its
// ability. Based on the work of Gestur (2002), Radkar and Lummen (2004).
//
// 11/2026 - Made a Hexagon skill, neverbot.

#include <living/effects.h>
#include <living/skills.h>
#include <language.h>

inherit BASE_EFFECT;

void setup()
{
  set_effect_name(_LANG_SKILL_CONCENTRATION_NAME);
  set_aliases(_LANG_SKILL_CONCENTRATION_ALIASES);

  is_passive = 1;

  add_category(SKILL_TYPE_BASIC, 1);
  set_help_desc(_LANG_SKILL_CONCENTRATION_HELP);
}
