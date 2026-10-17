// A room that is a jail cell. What the cell does lives in jail-actions.c; this
// file only makes it a room and refuses its exits to a prisoner. The
// citizenship's prison (`crimes set <citizenship> prison <file> [<file> ...]`)
// should name this room among its cells.

inherit room    "/lib/room.c";
inherit actions "/lib/room/justice/jail-actions.c";

#include <language.h>

void create()
{
  room::create();
}

void setup()
{
  set_light(60);
  set_short(_LANG_JAIL_ROOM_SHORT);
  set_long(_LANG_JAIL_ROOM_LONG);
}

void init()
{
  room::init();
  actions::init();
}

// Asked by do_exit_command when an exit is typed.
string query_exit_refusal(string dir, object who)
{
  return query_jail_refusal(dir, who);
}
