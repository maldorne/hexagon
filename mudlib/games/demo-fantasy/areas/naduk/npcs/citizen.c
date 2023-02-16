// Edited by Lummen and Antiron 24-7-97

#include <language.h>
#include "../path.h"

inherit "/lib/monster.c";

void setup() 
{
  int gender;
  gender = random(2) + 1;

  if (gender == 1)
  {
    set_name(_LANG_NPCS_CITIZEN_NAME_MALE);
    set_main_plural(capitalize(pluralize(_LANG_NPCS_CITIZEN_NAME_MALE)));
    add_plural(pluralize(_LANG_NPCS_CITIZEN_NAME_MALE));
    
    set_short(capitalize(_LANG_NPCS_CITIZEN_NAME_MALE));
    set_long(_LANG_NPCS_CITIZEN_LONG_MALE);
    
    set_gender(gender);
  }
  else
  {
    set_name(_LANG_NPCS_CITIZEN_NAME_FEMALE);
    set_main_plural(capitalize(pluralize(_LANG_NPCS_CITIZEN_NAME_FEMALE)));
    add_plural(pluralize(_LANG_NPCS_CITIZEN_NAME_FEMALE));
    
    set_short(capitalize(_LANG_NPCS_CITIZEN_NAME_FEMALE));
    set_long(_LANG_NPCS_CITIZEN_LONG_FEMALE);
    
    set_gender(gender);
  }

  // the race object
  set_race_ob(OBJ + "races/human.c");
  
  set_real_align(0);
  set_random_stats(3, 12);
  set_wimpy(15);
  set_level(2 + random(3));
  
  adjust_money(10+random(10), "copper");
  
  load_chat(50, ({
      1, _LANG_NPCS_CITIZEN_CHAT[0],
      1, _LANG_NPCS_CITIZEN_CHAT[1]
    }));

  add_move_zone("naduk");
  set_move_after(20+random(10),20+random(10));
}

