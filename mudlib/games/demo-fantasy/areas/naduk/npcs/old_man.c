// Edited by Lummen and Antiron 21-9-97

#include <language.h>
#include "../path.h"

inherit "/lib/monster.c";

void setup()
{
  set_name(_LANG_NPCS_OLD_MAN_NAME);
  add_alias(_LANG_NPCS_OLD_MAN_ALIAS);
  set_main_plural(capitalize(_LANG_NPCS_OLD_MAN_PLURAL));
  add_plural(_LANG_NPCS_OLD_MAN_PLURAL);
  add_plural(_LANG_NPCS_OLD_MAN_PLURALS);
  
  set_short(capitalize(_LANG_NPCS_OLD_MAN_NAME));
  set_long(_LANG_NPCS_OLD_MAN_LONG);

  // the race object
  set_race_ob(OBJ + "races/human.c");
  set_gender(1);

  set_level(10);
  set_wimpy(0);
  set_random_stats(3, 16);
  set_real_align(-100);
  
  adjust_money(random(10), "copper");
  
  load_chat(50,
  ({
    1, _LANG_NPCS_OLD_MAN_CHAT[0],
    1, _LANG_NPCS_OLD_MAN_CHAT[1],
    1, _LANG_NPCS_OLD_MAN_CHAT[2],
  }));

  add_clone(BASEOBS + "/armours/cloak.c", 1);
  init_equip();
} 
