/* Created by Lummen 27-Mayo-1997 */

#include "../path.h"
#include <language.h>
#include <translations/language.h>
#include <common/frames.h>

inherit "/lib/room.c";

void setup()
{
  set_short(_LANG_CROSSING_SHORT);
  set_long(_LANG_CROSSING_LONG); 
  set_light(60);
  add_exit(DIR_NORTHWEST, ROOMS + "6.c", "road"); // mountains
  add_exit(DIR_EAST, ROOMS + "9.c", "road"); // erken
  add_exit(DIR_SOUTHWEST, ROOMS + "21.c", "road"); // naduk
  
  add_sign(_LANG_CROSSING_SIGN_LONG,
    _LANG_CROSSING_SIGN_MESS,
    STD_LANG,
    DEFAULT_FRAME_STYLE,
    _LANG_CROSSING_SIGN_SHORT, 
    _LANG_CROSSING_SIGN_NAME);

  set_zone("road");
  add_property("no_undead", 1);
  add_clone(NPCS + "farmer.c", 2);
}
