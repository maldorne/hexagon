// Guild entrance component: the location is the entrance of a guild's house,
// with the same behaviour as the guild entrance room
// (/lib/room/guilds/guild-entrance-room.c), both from guild-entrance-actions.c.
// On every load it puts up the sign that explains the guild.

#include <room/location.h>

inherit component "/lib/location/component.c";
inherit actions   "/lib/room/guilds/guild-entrance-actions.c";

void create()
{
  actions::create();
  component::create();

  set_type(LOCATION_COMPONENT_GUILD_ENTRANCE);
}

object query_entrance_place() { return query_my_location(); }

void init()
{
  actions::init();
}

void initialize(object loc)
{
  component::initialize(loc);

  if (!loc)
    return;

  create_entrance_sign(loc);
}

// Name the guild, and say so on the sign at once.
void set_entrance_guild(string path)
{
  actions::set_entrance_guild(path);
  if (query_my_location())
    create_entrance_sign(query_my_location());
}

// Take the sign out of the location. The location destructs a component it
// drops without calling it, so whoever removes this one calls this first.
void take_down()
{
  take_down_entrance_sign();
}

string query_info()
{
  return strlen(query_entrance_guild()) ? query_entrance_guild() : "no guild";
}

mapping query_auto_load_attributes()
{
  return component::query_auto_load_attributes() +
         ([ "entrance_guild" : query_entrance_guild() ]);
}

void init_auto_load_attributes(mapping args)
{
  component::init_auto_load_attributes(args);

  if (args && stringp(args["entrance_guild"]))
    actions::set_entrance_guild(args["entrance_guild"]);
}

mixed * stats()
{
  return component::stats() + actions::stats();
}
