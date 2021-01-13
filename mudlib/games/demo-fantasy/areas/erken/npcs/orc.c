// Edited by Lummen 16-7-97

#include "../path.h"
#include <language.h>

inherit "/lib/monster.c";

void setup() 
{
  set_name(_LANG_NPCS_ORC_NAME);
  set_main_plural(capitalize(pluralize(_LANG_NPCS_ORC_NAME)));
  add_plural(pluralize(_LANG_NPCS_ORC_NAME));

  set_short(capitalize(_LANG_NPCS_ORC_NAME));
  // if we don't have a long description, the generic one from the race
  // object will be use instead /game/whatever/obj/races/something.c
  // set_long(_LANG_NPCS_ORC_LONG);
  
  // the race object
  set_race_ob(OBJ + "races/orc.c");
  set_gender(random(2) + 1);
  
  set_real_align(60);
  set_random_stats(3, 18);
  set_level(random(5)+6);
  set_wimpy(0);
  
  adjust_money(random(30), "copper");
    
  load_chat(50, ({
      1, _LANG_NPCS_ORC_CHAT[0],
      1, _LANG_NPCS_ORC_CHAT[1],
      1, _LANG_NPCS_ORC_CHAT[2],
      1, _LANG_NPCS_ORC_CHAT[3]
    }));


  add_clone(BASEOBS + "weapons/scimitar", 1);
  add_clone(BASEOBS + "armours/chainmail", 1);
  add_clone(BASEOBS + "armours/helmet", 1);
  init_equip();
}
