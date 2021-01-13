/* Created by Lummen and Antiron 15-Julio-1997 */

#include "../path.h"
#include <language.h>

inherit "/lib/room.c";

void setup()
{
  set_short(_LANG_ROAD_SHORT);
  set_long(_LANG_ROAD_FAR_FOREST_LONG);
  set_light(60);
  add_exit(DIR_SOUTH, ROOMS + "25.c", "road");
  add_exit(DIR_NORTH, ROOMS + "24.c", "road");
  set_zone("goblin_road");
  add_property("no_undead", 1);
}
