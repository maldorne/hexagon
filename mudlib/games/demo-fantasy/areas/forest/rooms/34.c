// Edited by Lummen 18-7-97

#include "../path.h"
#include <language.h>

inherit "/lib/room.c";

void setup()
{
  set_short(_LANG_FOREST_NEST_SHORT);
  set_long(_LANG_FOREST_NEST_LONG);
  set_light(20);
  add_item(_LANG_FOREST_WEB_ITEMS, _LANG_FOREST_WEB_DESC);
  add_exit(DIR_SOUTH, ROOMS + "33.c", "forest");
  add_clone(NPCS + "spider_queen.c", 1);
}
