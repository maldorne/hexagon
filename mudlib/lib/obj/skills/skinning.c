// Skinning (despellejar): takes the skin, plumage or scales off a corpse.
// Which parts a body has, and which skill takes each, is its race's; the
// work is shared with the other extraction skills (base-extraction.c).
// 10/2026 - The skinning of CcMud (despellejar, neverbot 30/03/2009) made a
//           mudlib skill, and the other parts beside it.

#include <living/skills.h>
#include <language.h>

inherit "/lib/obj/skills/base-extraction.c";

void setup()
{
  // Display name + aliases; the learning key is SKILL_SKINNING.
  set_effect_name(_LANG_SKILL_SKINNING_NAME);
  set_aliases(_LANG_SKILL_SKINNING_ALIASES);
  set_help_desc(_LANG_SKILL_SKINNING_HELP);

  setup_extraction(SKILL_SKINNING);
}
