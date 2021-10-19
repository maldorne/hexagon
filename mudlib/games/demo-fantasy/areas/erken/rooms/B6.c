// Created by Lummen 26-7-97.

#include "../path.h"
#include <language.h>

inherit "/lib/room.c";

void setup()
{
  set_short(_LANG_ERKEN_SHORT);
  set_long(_LANG_ERKEN_LONG);
  set_light(40);
  add_item(_LANG_ERKEN_HOUSE_ITEM, _LANG_ERKEN_HOUSE_DESC);
  set_zone("erken");
  add_property("no_undead", 1);
  add_exit(DIR_SOUTH, ROOMS + "BS.c", "door");
  add_exit(DIR_EAST, ROOMS + "B4.c", "road");
  add_exit(DIR_NORTHEAST, ROOMS + "B2.c", "road");
  add_clone(NPCS + "orc.c", 1);
}
