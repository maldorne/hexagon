/* Edited by Lummen and Antiron 15-7-97 */

#include "../path.h"
#include <language.h>

inherit "/lib/monster.c";

void setup() 
{
  set_name(_LANG_NPCS_GNOLL_NAME);
  set_main_plural(capitalize(pluralize(_LANG_NPCS_GNOLL_NAME)));
  add_plural(pluralize(_LANG_NPCS_GNOLL_NAME));

  set_short(capitalize(_LANG_NPCS_GNOLL_NAME));
  set_long(_LANG_NPCS_GNOLL_LONG);
  
  // the race object
  set_race_ob(OBJ + "races/gnoll.c");
  set_gender(1);
  
  set_real_align(-60);
  set_random_stats(3, 12);
  set_level(random(3) + 3);
  set_aggressive(0);
  
  adjust_money(10, "copper");
    
  load_a_chat(50, ({
      1, _LANG_NPCS_GNOLL_A_CHAT[0],
      1, _LANG_NPCS_GNOLL_A_CHAT[1]
    }));

  add_clone(BASEOBS + "weapons/hand_axe.c", 1);
  add_clone(BASEOBS + "armours/leather.c", 1);
  init_equip();

  add_move_zone("goblin_road");
  set_move_after(20 + random(10), 20 + random(10));
}
