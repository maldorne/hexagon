// Edited by Lummen 18-7-97

#include "../path.h"
#include <language.h>

inherit "/lib/room.c";

void setup()
{
  set_short(_LANG_MALLORN_SHORT);
  set_long(_LANG_MALLORN_LONG);
  add_item(_LANG_FOREST_RIVER_ITEMS, _LANG_FOREST_RIVER_DESC);
  set_light(40);
  add_exit(DIR_SOUTHWEST, ROOMS + "36.c", "forest");
  add_clone(NPCS + "wolf.c", 1);
  add_clone(NPCS + "bear.c", 2);
  add_clone(ITEMS + "mallorn.c", 1);
}
