// Edited by Lummen 21-8-97

#include <language.h>
#include "../path.h"

inherit "/lib/monster.c";

void setup() 
{
  set_name(_LANG_NPCS_GUARD_NAME);
  set_main_plural(capitalize(pluralize(_LANG_NPCS_GUARD_NAME)));
  add_plural(pluralize(_LANG_NPCS_GUARD_NAME));
  
  set_short(capitalize(_LANG_NPCS_GUARD_NAME));
  set_long(_LANG_NPCS_GUARD_LONG);
  
  // the race object
  set_race_ob(OBJ + "races/human.c");
  set_gender(1);
  
  set_real_align(-100 + random(50));
  set_random_stats(3, 18);
  set_wimpy(0);
  set_level(6 + random(5));
  
  adjust_money(100 + random(400), "copper");
  
  load_chat(50, ({
      1, _LANG_NPCS_GUARD_CHAT[0],
      1, _LANG_NPCS_GUARD_CHAT[1],
      1, _LANG_NPCS_GUARD_CHAT[2]
    }));

  load_a_chat(50, ({
      1, _LANG_NPCS_GUARD_A_CHAT[0],
      1, _LANG_NPCS_GUARD_A_CHAT[1],
      1, _LANG_NPCS_GUARD_A_CHAT[2]
    }));

  add_clone(BASEOBS + "armours/chain_mail.c", 1);
  add_clone(BASEOBS + "weapons/long_sword.c", 1);
  add_clone(BASEOBS + "shields/small_wooden_shield.c", 1);
  init_equip();
}
