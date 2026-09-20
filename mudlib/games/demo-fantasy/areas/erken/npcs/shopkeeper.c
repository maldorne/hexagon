// The keeper of Erken's shop. The goblins hold the town, and he sells whatever
// they bring him without asking where it came from.

#include <language.h>
#include "../path.h"

inherit "/lib/monster.c";

void setup()
{
  set_name(_LANG_NPCS_SHOPKEEPER_NAME);
  add_alias(_LANG_NPCS_SHOPKEEPER_ALIAS);
  set_main_plural(capitalize(_LANG_NPCS_SHOPKEEPER_PLURAL));
  add_plural(_LANG_NPCS_SHOPKEEPER_PLURAL);
  add_plural(_LANG_NPCS_SHOPKEEPER_PLURALS);

  set_short(capitalize(_LANG_NPCS_SHOPKEEPER_NAME));
  set_long(_LANG_NPCS_SHOPKEEPER_LONG);

  // the race object
  set_race_ob(OBJ + "races/goblin.c");
  set_gender(1);

  set_wimpy(10);
  set_random_stats(6, 18);
  adjust_money(random(50), "copper");
  set_level(5 + random(3));
  set_real_align(-200);

  load_chat(10,
  ({
    1, _LANG_NPCS_SHOPKEEPER_CHAT[0],
    1, _LANG_NPCS_SHOPKEEPER_CHAT[1],
    1, _LANG_NPCS_SHOPKEEPER_CHAT[2],
  }));

  load_a_chat(30,
  ({
    1, _LANG_NPCS_SHOPKEEPER_A_CHAT[0],
    1, _LANG_NPCS_SHOPKEEPER_A_CHAT[1],
  }));
}
