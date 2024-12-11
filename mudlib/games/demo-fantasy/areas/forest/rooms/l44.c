// Edited by Lummen 18-7-97

#include "../path.h"
#include <language.h>

inherit "/lib/room.c";

void setup()
{
  set_short(_LANG_DEEP_FOREST_SHORT);
  set_long(_LANG_DEEP_FOREST_LONG);
  set_light(40);
  add_item(_LANG_FOREST_TREE_ITEMS, _LANG_FOREST_TREE_DESC);
  add_room_zone("forest");  
  add_property("no_undead", 1);
  add_exit(DIR_EAST, ROOMS + "l43.c", "forest");
  add_exit(DIR_SOUTH, ROOMS + "41.c", "forest");
  add_exit(DIR_NORTH, ROOMS + "l43.c", "forest");
  add_exit(DIR_SOUTHEAST, ROOMS + "l42.c", "forest");
  add_exit(DIR_NORTHEAST, ROOMS + "l44.c", "forest");
  add_exit(DIR_NORTHWEST, ROOMS + "l42.c", "forest");
  add_exit(DIR_SOUTHWEST, ROOMS + "l44.c", "forest"); 
  add_exit(DIR_WEST, ROOMS + "41.c", "forest");
}
