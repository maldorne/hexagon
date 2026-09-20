// The keeper of Elfereth's shop. An elf of the forest of Elrhair who parts with
// what the woods can spare and no more.

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
  set_race_ob(OBJ + "races/elf.c");
  set_gender(1);

  set_wimpy(20);
  set_random_stats(5, 17);
  adjust_money(random(50), "copper");
  set_level(5 + random(3));
  set_real_align(200);

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
