/* Created by Lummen and Antiron 15-7-97 */

#include <language.h>

inherit "/lib/monster.c";

void setup()
{
  set_name(_LANG_NPCS_WOLF_NAME);
  set_short(_LANG_NPCS_WOLF_SHORT);
  add_alias(_LANG_NPCS_WOLF_ALIASES);

  set_main_plural(_LANG_NPCS_WOLF_PLURAL);
  add_plural(_LANG_NPCS_WOLF_PLURAL_ALIASES);

  set_long(_LANG_NPCS_WOLF_LONG);
  
  set_race("animal");
  set_gender(1); // spanish language
  
  set_real_align(100 - random(200));
  set_random_stats(6, 15);
  set_wimpy(0);
  set_level(3 + random(3));
  
  add_move_zone("forest");
  set_move_after(20+random(10), 20+random(10));
}
