/* Created by Lummen and Antiron 27-Mayo-1997 */

#include "../path.h"
#include <language.h>

inherit "/lib/room.c";

void setup()
{
  set_short(_LANG_ROAD_SHORT);
  set_long(_LANG_ROAD_LONG);
  set_light(60);
  add_exit(DIR_EAST, ROOMS + "6.c", "road");
  add_exit(DIR_NORTHEAST, ROOMS + "5.c", "road");
  add_property("no_undead", 1);
  add_clone(NPCS + "pilgrim.c", 2);
  add_exit(DIR_WEST, ELFERETH + "rooms/z1.c", "path"); 
}
