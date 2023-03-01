/* NPC Assassin Object */

inherit "/lib/monster.c";

#include <language.h>
#include "../path.h"

void setup()
{
  set_name(_LANG_NPCS_ASSASSIN_NAME);
  add_alias(_LANG_NPCS_ASSASSIN_ALIAS);

  set_main_plural(_LANG_NPCS_ASSASSIN_MAIN_PLURAL);
  add_plural(_LANG_NPCS_ASSASSIN_PLURALS);
  
  set_short(_LANG_NPCS_ASSASSIN_SHORT);
  set_long(_LANG_NPCS_ASSASSIN_LONG);

  set_race_ob(OBJ + "races/human.c");
  set_gender(1);
  set_level(5 + random(3));

  set_wimpy(0);
  set_random_stats(6, 18);
  
  add_move_zone("erken");
  
  // (a, b) after a + random(b) heart_beats
  set_move_after(10, 10);
  set_heart_beat(1);

  add_clone(BASEOBS + "weapons/dagger", 1);
  add_clone(BASEOBS + "armours/leather", 1);

  // set_skill("dodge", 100);
  // set_skill("knife", 100);
  // set_skill("stealth", 100);
}