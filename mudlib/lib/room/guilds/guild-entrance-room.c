// A room that is the entrance of a guild's house. What the entrance does lives
// in guild-entrance-actions.c; this file only makes it a room and puts up the
// sign that explains the guild. The room names its guild in setup() with
// set_entrance_guild(<path of the guild object>), and converting it to a
// location carries it to the guild entrance component.

inherit room    "/lib/room.c";
inherit actions "/lib/room/guilds/guild-entrance-actions.c";

void create()
{
  actions::create();
  room::create();

  create_entrance_sign(this_object());
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
