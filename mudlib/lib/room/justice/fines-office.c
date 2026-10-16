// A room that is a fines office. What the office does lives in
// fines-actions.c; this file only makes it a room.
//
// A fines office is meant to stand outside the city walls: the guards bar the
// wanted from the gates, so it is where somebody wanted can pay and clean the
// record. Set the law with set_fines_law("<citizenship>") when the room has no
// area to give it; once converted to a location, the area's law rules.

inherit room    "/lib/room.c";
inherit actions "/lib/room/justice/fines-actions.c";

#include <language.h>

void create()
{
  room::create();
  create_fines_sign(this_object());
}

void setup()
{
  set_light(80);
  set_short(_LANG_FINES_ROOM_SHORT);
  set_long(_LANG_FINES_ROOM_LONG);
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
