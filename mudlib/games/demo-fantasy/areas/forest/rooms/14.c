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
  add_exit(DIR_SOUTHWEST, ROOMS + "17.c", "forest");
  add_exit(DIR_EAST, ROOMS + "15.c", "forest");
  add_exit(DIR_NORTHEAST, ROOMS + "39.c", "forest");
  add_clone(NPCS + "wasp.c", 3);
 add_property("no_undead", 1);
}
