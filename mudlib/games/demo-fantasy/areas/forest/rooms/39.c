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
  add_exit(DIR_NORTHWEST, ROAD + "rooms/9.c", "road");
  add_exit(DIR_NORTH, ROAD + "rooms/10.c", "road");
  add_exit(DIR_NORTHEAST, ROAD + "rooms/11.c", "road");
  add_exit(DIR_SOUTH, ROOMS + "15.c", "forest");
  add_exit(DIR_SOUTHWEST, ROOMS + "14.c", "forest");
  add_clone(NPCS + "aranya", 3);
  add_clone(NPCS + "wolf.c", 2);
  add_clone(NPCS + "wasp.c", 1);
}
