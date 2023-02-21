// Paris, Apr 98

#include "../path.h"
#include <language.h>

inherit "/lib/monster.c";

void setup()
{
  set_name(_LANG_NPCS_ELF_NAME);
  set_short(capitalize(_LANG_NPCS_ELF_NAME));
  set_main_plural(capitalize(pluralize(_LANG_NPCS_ELF_NAME)));
  add_plural(pluralize(_LANG_NPCS_ELF_NAME));

  // if we don't have a long description, the generic one from the race
  // object will be use instead /game/whatever/obj/races/something.c
  // set_long(_LANG_NPCS_ELF_LONG);

  // the race object
  set_race_ob(OBJ + "races/elf.c");
  set_gender(random(2) + 1);
  
  set_real_align(-30);
  set_random_stats(3, 16);
  set_level(3 + random(3));
  
  adjust_money(random(5), "silver");
    
  load_chat(50, ({
      1, _LANG_NPCS_ELF_CHAT[0],
      1, _LANG_NPCS_ELF_CHAT[1]
    }));

  load_a_chat(30, ({
      1, _LANG_NPCS_ELF_A_CHAT[0]
    }));

  add_clone(BASEOBS + "weapons/dagger", 1);
  add_clone(BASEOBS + "armours/leather", 1);
  add_clone(BASEOBS + "armours/trousers", 1);
  add_clone(BASEOBS + "armours/slippers", 1);
  init_equip();
}
