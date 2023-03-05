/* Created by Lummen 15-7-97 */

#include "../path.h"
#include <language.h>

inherit "/lib/room.c";

void setup()
{
  set_short(_LANG_RAISEROOM_SHORT);
  set_long(_LANG_RAISEROOM_LONG);

  add_item(_LANG_RAISEROOM_STATUE, _LANG_RAISEROOM_STATUE_DESC);
  add_item(_LANG_RAISEROOM_ALTAR, _LANG_RAISEROOM_ALTAR_DESC);

  set_light(60);
  add_exit(DIR_SOUTH, ROOMS + "4.c", "door");
  add_property("no_undead", 1);

  add_clone(NPCS + "healer.c", 1);
}
