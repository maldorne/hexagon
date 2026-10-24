// Guild room component: the location is the main hall of a guild's house, with
// the same behaviour as the guild room (/lib/room/guilds/guild-room.c), both
// from guild-room-actions.c. On every load it puts up the sign that says what
// can be done in the hall.

#include <room/location.h>

inherit component "/lib/location/component.c";
inherit actions   "/lib/room/guilds/guild-room-actions.c";

void create()
{
  actions::create();
  component::create();

  set_type(LOCATION_COMPONENT_GUILD_ROOM);
}

object query_guild_room_place() { return query_entity(); }

void init()
{
  actions::init();
}

void initialize(object loc)
{
  component::initialize(loc);

  if (!loc)
    return;

  create_guild_room_sign(loc);
}

// Take the sign out of the location. The location destructs a component it
// drops without calling it, so whoever removes this one calls this first.
void take_down()
{
  take_down_guild_room_sign();
}

string query_info()
{
  return strlen(query_room_guild()) ? query_room_guild() : "no guild";
}

mapping query_auto_load_attributes()
{
  return component::query_auto_load_attributes() +
         ([ "room_guild" : query_room_guild(),
            "teach_min" : query_teach_min_level(),
            "teach_max" : query_teach_max_level(),
            "teacher"   : query_teacher() ]);
}

void init_auto_load_attributes(mapping args)
{
  component::init_auto_load_attributes(args);

  if (args && stringp(args["room_guild"]))
    set_room_guild(args["room_guild"]);
  if (args && intp(args["teach_min"]) && intp(args["teach_max"]))
    set_teaching(args["teach_min"], args["teach_max"],
                 stringp(args["teacher"]) ? args["teacher"] : nil);
}

mixed * stats()
{
  return component::stats() + actions::stats();
}
