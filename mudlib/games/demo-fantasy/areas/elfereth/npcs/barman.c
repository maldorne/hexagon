// Paris. Abr'98

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
  set_race_ob(OBJ + "races/elf.c");
  set_gender(1);

  set_level(5 + random(3));
  set_wimpy(0);
  set_random_stats(6, 16);
  set_real_align(200);
  
  adjust_money(random(50), "copper");
  
  load_chat(10,
  ({
    1, _LANG_NPCS_BARMAN_CHAT[0],
  }));

  load_a_chat(30,
  ({
    1, _LANG_NPCS_BARMAN_A_CHAT[0],
  }));
} 
