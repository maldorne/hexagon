/* Created by Lummen and Antiron 15-7-97 */

#include "../path.h"
#include <language.h>

inherit "/lib/monster.c";

void setup()
{
  set_name(_LANG_NPCS_PILGRIM_NAME);
  set_short(capitalize(_LANG_NPCS_PILGRIM_NAME));
  set_main_plural(capitalize(pluralize(_LANG_NPCS_PILGRIM_NAME)));
  add_plural(pluralize(_LANG_NPCS_PILGRIM_NAME));

  set_long(_LANG_NPCS_PILGRIM_LONG);
  
  set_race_ob(OBJ + "races/human.c");
  set_gender(1);
  
  set_real_align(10);
  set_random_stats(3, 12);
  set_wimpy(10);
  set_level(2 + random(3));
  
  adjust_money(random(30), "copper");
  
  load_chat(50, ({
      1, _LANG_NPCS_PILGRIM_CHAT[0],
      1, _LANG_NPCS_PILGRIM_CHAT[1]
    }));

  add_clone(BASEOBS + "armours/cloak", 1);
  add_clone(BASEOBS + "weapons/dagger", 1);
  init_equip();

  add_move_zone("road");
  set_move_after(20 + random(20), 20 + random(20));
}
