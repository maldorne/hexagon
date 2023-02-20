// Edited by Lummen 17-7-97

#include <language.h>
#include "../path.h"

inherit "/lib/monster.c";

void setup() 
{
  set_name(_LANG_NPCS_FARMER_NAME);
  set_main_plural(capitalize(pluralize(_LANG_NPCS_FARMER_NAME)));
  add_plural(pluralize(_LANG_NPCS_FARMER_NAME));
  
  // the race object
  set_race_ob(OBJ + "races/human.c");
  set_gender(1);
  
  set_real_align(-200 + random(400));
  set_random_stats(3, 15);
  set_wimpy(15);
  set_level(2 + random(3));
  
  adjust_money(10 + random(10), "copper");
  
  set_short(capitalize(_LANG_NPCS_FARMER_NAME));
  set_long(_LANG_NPCS_FARMER_LONG);
  
  load_chat(50, ({
      1, _LANG_NPCS_FARMER_CHAT[0],
      1, _LANG_NPCS_FARMER_CHAT[1]
    }));

  add_clone("/baseobs/weapons/club", 1);
  init_equip();
}

