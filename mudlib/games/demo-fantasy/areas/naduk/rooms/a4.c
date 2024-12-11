// Edited by Lummen 27-7-97

#include "../path.h"
#include <language.h>

inherit "/lib/room.c";

void setup()
{
  set_short(_LANG_NADUK_SQUARE_SHORT);
  set_long(_LANG_NADUK_SQUARE_LONG);
  add_exit(DIR_NORTH, ROOMS + "as.c", "door");
  add_exit(DIR_SOUTH, ROOMS + "ap.c", "door");
  add_exit(DIR_WEST, ROOMS + "a3.c", "road");
  add_exit(DIR_EAST, ROOMS + "a7.c", "road"); 
  add_exit(DIR_NORTHWEST, ROOMS + "a6.c", "road"); 
  add_exit(DIR_SOUTHWEST, ROOMS + "a5.c", "road");
  set_light(60);
  add_room_zone("naduk");
  add_clone(NPCS + "citizen.c", 1);
  add_clone(NPCS + "guard.c", 1);
  add_clone(ITEMS + "fountain.c", 1);
  add_property("no_undead", 1);
}
