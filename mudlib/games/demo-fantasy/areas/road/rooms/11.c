/* Created by Lummen and Antiron 15-Julio-1997 */

#include "../path.h"
#include <language.h>

inherit "/lib/room.c";

void setup()
{
  set_short(_LANG_ROAD_NEAR_FOREST_SHORT);
  set_long(_LANG_ROAD_FOREST_LONG);
  set_light(60);
  add_exit(DIR_EAST, ROOMS + "12.c", "road");
  add_exit(DIR_WEST, ROOMS + "10.c", "road");
  add_exit(DIR_NORTH, FOREST + "rooms/36.c", "forest");
  add_exit(DIR_SOUTH, FOREST + "rooms/40.c", "forest");
  add_exit(DIR_SOUTHWEST, FOREST + "rooms/39.c", "forest");
  set_zone("road");
  add_property("no_undead", 1);
  add_clone(NPCS + "pilgrim.c", 3);
}
