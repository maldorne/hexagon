/* Created by Lummen 16-7-97 */

#include "../path.h"
#include <language.h>

inherit "/lib/monster.c";

void setup()
{
  set_name(_LANG_NPCS_SPIDER_NAME);
  set_short(_LANG_NPCS_SPIDER_SHORT);
  add_alias(_LANG_NPCS_SPIDER_ALIASES);

  set_main_plural(_LANG_NPCS_SPIDER_PLURAL);
  add_plural(_LANG_NPCS_SPIDER_PLURAL_ALIASES);

  set_long(_LANG_NPCS_SPIDER_LONG);
  
  set_race_ob(OBJ + "races/spider.c");
  set_gender(2); // spanish language
  
  set_real_align(0);
  set_random_stats(3, 14);
  set_wimpy(0);
  set_level(2 + random(3));
}
