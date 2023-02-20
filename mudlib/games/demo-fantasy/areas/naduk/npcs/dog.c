/* Edited by Lummen and Antiron 24-7-97 */

#include <language.h>

inherit "/lib/monster.c";

void setup()
{
  set_name(_LANG_NPCS_DOG_NAME);
  set_short(capitalize(_LANG_NPCS_DOG_NAME));

  set_main_plural(capitalize(pluralize(_LANG_NPCS_DOG_NAME)));
  add_plural(pluralize(_LANG_NPCS_DOG_NAME));

  set_long(_LANG_NPCS_DOG_LONG);

  set_random_stats(3, 16);
  set_race("animal");
  set_level(3);
  set_gender(1); // for spanish language
  set_real_align(0);
  set_aggressive(0);
  set_wimpy(0);

  add_move_zone("naduk");
  set_move_after(20 + random(10), 20 + random(10));
}
