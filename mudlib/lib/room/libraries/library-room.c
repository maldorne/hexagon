// A room that is a library. What the library does lives in
// library-actions.c; this file only makes it a room and puts up the sign
// that explains it. The room declares its books in setup() with
// add_library_book(<path of the text>), and converting it to a location
// carries them to the library component.

inherit room    "/lib/room.c";
inherit actions "/lib/room/libraries/library-actions.c";

#include <language.h>

void create()
{
  actions::create();
  room::create();

  create_library_sign(this_object());
}

void setup()
{
  set_light(80);
  set_short(_LANG_LIBRARY_ROOM_SHORT);
  set_long(_LANG_LIBRARY_ROOM_LONG);
}

void init()
{
  room::init();
  actions::init();
}

mixed * stats()
{
  return room::stats() + actions::stats();
}
