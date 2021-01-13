/* Created by Lummen and Antiron 15-7-97 */

#include <language.h>

inherit "/lib/monster.c";

void setup()
{
  set_name(_LANG_NPCS_BEAR_NAME);
  set_short(_LANG_NPCS_BEAR_SHORT);
  add_alias(_LANG_NPCS_BEAR_ALIASES);

  set_main_plural(_LANG_NPCS_BEAR_PLURAL);
  add_plural(_LANG_NPCS_BEAR_PLURAL_ALIASES);

  set_long(_LANG_NPCS_BEAR_LONG);
  
  set_race("animal");
  set_gender(1); // spanish language
  
  set_real_align(30);
  set_random_stats(3, 16);
  set_wimpy(0);
  set_level(3 + random(2));
  
  set_join_fight_mess(_LANG_NPCS_BEAR_JOIN_FIGHT);
}
