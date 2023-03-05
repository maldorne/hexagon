/* Created by Lummen 27-Mayo-1997 */

#include "../path.h"
#include <language.h>

inherit "/lib/room.c";

static object door;

void setup()
{
  set_short(_LANG_ROAD_SHORT);
  set_long(_LANG_ROAD_LONG);
  set_light(60);
  add_exit(DIR_NORTH, ROOMS + "2.c", "road");
  add_exit(DIR_SOUTH, ROOMS + "5.c", "road");
  
  door = add_exit(DIR_EAST, ROOMS + "guild.c", "door");
  if (door)
    door->set_init_status(0);

  set_zone("road");
  add_property("no_undead", 1);
  add_clone(NPCS + "snake.c", 1);
  // add_clone(ITEMS + "board.c", 1);
}
