/* Created by Lummen 16-7-97 */

#include <language.h>

inherit "/lib/monster.c";

void setup()
{
  set_name(_LANG_NPCS_WASP_NAME);
  set_short(_LANG_NPCS_WASP_SHORT);
  add_alias(_LANG_NPCS_WASP_ALIASES);

  set_main_plural(_LANG_NPCS_WASP_PLURAL);
  add_plural(_LANG_NPCS_WASP_PLURAL_ALIASES);

  set_long(_LANG_NPCS_WASP_LONG);

  set_race("animal");
  set_gender(2); // spanish language

  set_real_align(0);
  set_random_stats(3, 12);
  set_wimpy(0);
  set_level(3 + random(3));
}
