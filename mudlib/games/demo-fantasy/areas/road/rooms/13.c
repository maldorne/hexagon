/* Created by Lummen and Antiron 27-Mayo-1997 */

#include "../path.h"
#include <language.h>

inherit "/lib/room.c";

void setup()
{
  set_short(_LANG_ROAD_NEAR_FOREST_SHORT);
  set_long(_LANG_ROAD_FOREST_LONG);
  set_light(60);
  add_exit(DIR_EAST, NADUK + "rooms/a1.c", "road");
  add_exit(DIR_WEST, ROOMS + "12.c", "road");
  add_property("no_undead", 1);
  add_clone(NPCS + "pilgrim.c", 1);
  add_clone(NPCS + "farmer.c", 1);
}
