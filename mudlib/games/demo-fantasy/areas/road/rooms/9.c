/* Created by Lummen and Antiron 15-Julio-1997 */

#include "../path.h"
#include <language.h>

inherit "/lib/room.c";

void setup()
{
  set_short(_LANG_ROAD_SHORT);
  set_long(_LANG_ROAD_NEAR_FOREST_LONG);
  set_light(60);
  add_exit(DIR_EAST, ROOMS + "10.c", "road");
  add_exit(DIR_WEST, ROOMS + "8.c", "road");
  add_exit(DIR_SOUTHEAST, FOREST + "rooms/39.c", "forest");
  add_room_zone("naduk-road");
  add_property("no_undead", 1);
  add_clone(NPCS + "farmer.c", 2);
}
