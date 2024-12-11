/* Created by Lummen 27-Mayo-1997 */

#include "../path.h"
#include <language.h>

inherit "/lib/room.c";

void setup()
{
  set_short(_LANG_ROAD_SHORT);
  set_long(_LANG_ROAD_LONG + " " + _LANG_ROAD_TEMPLE);
  set_light(60);
  add_exit(DIR_WEST, ROOMS + "3.c", "road");
  add_exit(DIR_NORTH, ROOMS + "raise.c", "door");
  add_room_zone("naduk-road");
  add_property("no_undead", 1);
  add_clone(NPCS + "pilgrim.c",  2);
}
