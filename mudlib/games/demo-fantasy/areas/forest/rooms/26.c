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
  add_room_zone("forest");  
  add_property("no_undead", 1);
  add_exit(DIR_SOUTH, ROOMS + "28.c", "forest");
  add_exit(DIR_WEST, ROOMS + "16.c", "forest");
  add_exit(DIR_NORTH, ROOMS + "27.c", "forest");
  add_exit(DIR_EAST, ROOMS + "41.c", "forest");
  add_clone(NPCS + "aranya", 2);
}
