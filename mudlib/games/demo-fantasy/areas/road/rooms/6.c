/* Created by Lummen and Antiron 27-Mayo-1997 */

#include "../path.h"
#include <language.h>

inherit "/lib/room.c";

void setup()
{
  set_short(_LANG_ROAD_SHORT);
  set_long(_LANG_ROAD_LONG);
  set_light(60);
  add_exit(DIR_NORTH, ROOMS + "5.c", "road");
  add_exit(DIR_SOUTHEAST, ROOMS + "8.c", "road");
  add_exit(DIR_WEST, ROOMS + "7.c", "road");
  set_zone("road");
  add_property("no_undead", 1);
}
