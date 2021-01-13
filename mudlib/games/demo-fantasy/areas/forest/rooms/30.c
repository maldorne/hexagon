// Edited by Lummen 18-7-97

#include "../path.h"
#include <language.h>

inherit "/lib/room.c";

void setup()
{
  set_short(_LANG_FOREST_SHORT);
  set_long(_LANG_FOREST_LONG);
  set_light(40);
  add_item(_LANG_FOREST_TREE_ITEMS, _LANG_FOREST_TREE_DESC);
  set_zone("forest");  
  add_property("no_undead", 1);
  add_exit(DIR_WEST, ROOMS + "20.c", "forest");
  add_exit(DIR_EAST, ROOMS + "29.c", "forest");
  add_exit(DIR_SOUTH, ROOMS + "32.c", "forest");
}
