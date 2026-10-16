// Gutting (destripar): takes the heart out of a corpse.
// Which parts a body has, and which skill takes each, is its race's; the
// work is shared with the other extraction skills (base-extraction.c).
// 10/2026 - Written beside the skinning skill, neverbot.

#include <living/skills.h>
#include <language.h>

inherit "/lib/obj/skills/base-extraction.c";

void setup()
{
  // Display name + aliases; the learning key is SKILL_GUTTING.
  set_effect_name(_LANG_SKILL_GUTTING_NAME);
  set_aliases(_LANG_SKILL_GUTTING_ALIASES);
  set_help_desc(_LANG_SKILL_GUTTING_HELP);

  setup_extraction(SKILL_GUTTING);
}
