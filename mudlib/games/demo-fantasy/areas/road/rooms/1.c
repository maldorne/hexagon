/* Created by Lummen 27-Mayo-1997 */

#include "../path.h"
#include <language.h>

inherit "/lib/room.c";

static object door;
static object sign;

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

  add_room_zone("naduk-road");
  add_property("no_undead", 1);
  add_clone(NPCS + "snake.c", 1);

  sign = add_sign(_LANG_ROAD_SIGN_LONG, read_file(doc("newbie.txt")),
                  _LANG_ROAD_SIGN_NAME, _LANG_ROAD_SIGN_SHORT);
  if (sign)
    sign->add_alias(_LANG_ROAD_SIGN_ALIASES);
}
