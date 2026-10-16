// A room that is a temple. What the temple does lives in temple-actions.c;
// this file only makes it a room. Give it its deity in setup with
// set_temple_deity("<deity path>.c"); once converted to a location, the temple
// component keeps the same deity.

inherit room    "/lib/room.c";
inherit actions "/lib/room/temple/temple-actions.c";

#include <language.h>

void create()
{
  room::create();
  create_temple_sign(this_object());
}

void setup()
{
  set_light(60);
  set_short(_LANG_TEMPLE_ROOM_SHORT);
  set_long(_LANG_TEMPLE_ROOM_LONG);
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
