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
  add_exit(DIR_SOUTHEAST, ROAD + "rooms/12.c", "road");
  add_exit(DIR_SOUTHWEST, ROAD + "rooms/10.c", "road");
  add_exit(DIR_SOUTH, ROAD + "rooms/11.c", "road");
  add_exit(DIR_NORTHEAST, ROOMS + "38.c", "forest");
  add_exit(DIR_EAST, ROOMS + "37.c", "forest");
  add_exit(DIR_WEST, ROOMS + "35.c", "forest");
  add_clone(NPCS + "wolf.c", 4);
  add_property("no_undead", 1);
}
