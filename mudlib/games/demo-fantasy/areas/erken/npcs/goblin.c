/* Edited by Lummen and Antiron 15-7-97 */

#include "../path.h"
#include <language.h>

inherit "/lib/monster.c";

void setup() 
{
  set_name(_LANG_NPCS_GOBLIN_NAME);
  set_main_plural(capitalize(pluralize(_LANG_NPCS_GOBLIN_NAME)));
  add_plural(pluralize(_LANG_NPCS_GOBLIN_NAME));

  set_short(capitalize(_LANG_NPCS_GOBLIN_NAME));
  // if we don't have a long description, the generic one from the race
  // object will be use instead /game/whatever/obj/races/something.c
  // set_long(_LANG_NPCS_GOBLIN_LONG);
  
  // the race object
  set_race_ob(OBJ + "races/goblin.c");
  set_gender(random(2) + 1);
  
  set_real_align(30);
  set_random_stats(3, 12);
  set_level(random(3)+3);
  
  adjust_money(10, "copper");
    
  load_chat(50, ({
      1, _LANG_NPCS_GOBLIN_CHAT[0],
      1, _LANG_NPCS_GOBLIN_CHAT[1]
    }));

  load_a_chat(50, ({
      1, _LANG_NPCS_GOBLIN_A_CHAT[0],
      1, _LANG_NPCS_GOBLIN_A_CHAT[1]
    }));

  add_clone(BASEOBS + "weapons/dagger", 1);
  add_clone(BASEOBS + "armours/leather", 1);
  init_equip();

  add_move_zone("erken");
  set_move_after(20 + random(10), 20 + random(10));
}
