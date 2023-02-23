// Edited by Lummen and Antiron 26-7-97

#include <language.h>
#include "../path.h"

inherit "/lib/monster.c";

void setup()
{
  set_name(_LANG_NPCS_BARMAN_NAME);
  add_alias(_LANG_NPCS_BARMAN_ALIAS);
  set_main_plural(capitalize(_LANG_NPCS_BARMAN_PLURAL));
  add_plural(_LANG_NPCS_BARMAN_PLURAL);
  add_plural(_LANG_NPCS_BARMAN_PLURALS);
  
  set_short(capitalize(_LANG_NPCS_BARMAN_NAME));
  set_long(_LANG_NPCS_BARMAN_LONG);

  // the race object
  set_race_ob(OBJ + "races/human.c");
  set_gender(1);

  set_level(5 + random(6));
  set_wimpy(0);
  set_random_stats(3, 14);
  set_real_align(-100);
  
  adjust_money(random(10), "silver");
  
  load_chat(10,
  ({
    1, _LANG_NPCS_BARMAN_CHAT[0],
    1, _LANG_NPCS_BARMAN_CHAT[1],
    1, _LANG_NPCS_BARMAN_CHAT[2],
  }));

  load_a_chat(30,
  ({
    1, _LANG_NPCS_BARMAN_A_CHAT[0],
    1, _LANG_NPCS_BARMAN_A_CHAT[1],
  }));
} 
