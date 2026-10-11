// A room that is the main hall of a guild's house. What the hall does lives in
// guild-room-actions.c; this file only makes it a room and puts up the sign
// that says what can be done in it. The room names its guild in setup() with
// set_room_guild(<path of the guild object>), and converting it to a location
// carries it to the guild room component.

inherit room    "/lib/room.c";
inherit actions "/lib/room/guilds/guild-room-actions.c";

void create()
{
  actions::create();
  room::create();

  create_guild_room_sign(this_object());
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
