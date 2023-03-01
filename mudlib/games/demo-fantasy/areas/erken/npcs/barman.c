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
  set_race_ob(OBJ + "races/goblin.c");
  set_gender(1);

  set_wimpy(10);
  set_gender(1);
  set_random_stats(6, 18);
  adjust_money(random(50), "copper");
  set_level(5 + random(3));
  set_real_align(-200);
  
  load_chat(10,
  ({
    1, _LANG_NPCS_BARMAN_CHAT[0],
    1, _LANG_NPCS_BARMAN_CHAT[1],
  }));

  load_a_chat(30,
  ({
    1, _LANG_NPCS_BARMAN_A_CHAT[0],
  }));

  add_clone(BASEOBS + "weapons/club", 1);
  add_clone(BASEOBS + "armours/chain_mail", 1);
  init_equip();
} 
