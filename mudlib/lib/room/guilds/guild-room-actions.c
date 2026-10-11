// Guild room: the main hall of a guild's house, where its members gather.
// Shared by the guild room (/lib/room/guilds/guild-room.c) and the guild room
// location component (/lib/location/components/guild-room.c). Its members
// advance a level of the guild here (avanzar), spending the experience the
// guild asks for it, which coste shows them here (the general coste command,
// elsewhere, shows every kind of level).
//
// Ported from the CcMud guild room, Folken 12/2006 and 07/2010.

#include <language.h>

// the guild this is the hall of, as the path of its object
private string room_guild;
static object room_sign;

int query_guild_room() { return 1; }

// Where the hall stands. The component answers with its location.
object query_guild_room_place() { return this_object(); }

void create()
{
  room_guild = "";
}

string query_room_guild() { return room_guild; }
void set_room_guild(string path) { room_guild = path ? path : ""; }

// A guild object named with or without its extension is the same guild.
private string _guild_path(string path)
{
  if (!path || !strlen(path))
    return "";
  if (strlen(path) > 2 && path[strlen(path) - 2 ..] == ".c")
    path = path[0 .. strlen(path) - 3];
  return path;
}

// Put up the sign that says what can be done in the hall, replacing any earlier
// one, so a place never has two.
object create_guild_room_sign(object where)
{
  if (room_sign)
    room_sign->dest_me();

  room_sign = where->add_sign(_LANG_GUILD_ROOM_SIGN_LONG,
                              _LANG_GUILD_ROOM_SIGN_TEXT);
  return room_sign;
}

// Take the sign down again, when the hall leaves its place.
void take_down_guild_room_sign()
{
  if (room_sign)
    room_sign->dest_me();
  room_sign = nil;
}

void init()
{
  add_action("do_advance", _LANG_GUILD_ROOM_CMDS_ADVANCE);
  add_action("do_cost", _LANG_GUILD_ROOM_CMDS_COST);
}

// What the next level of the guild costs a member. Anybody else gets the
// general coste command.
int do_cost(string str)
{
  object me, guild;
  int cost, have, next;

  me = this_player();
  if (!me || !strlen(room_guild) ||
      _guild_path(me->query_guild_ob()) != _guild_path(room_guild) ||
      !(guild = load_object(room_guild)))
    return 0;

  if ((int)me->query_guild_level() >= (int)guild->query_max_level())
  {
    write(_LANG_GUILD_ROOM_MAX_LEVEL);
    return 1;
  }

  next = (int)me->query_guild_level() + 1;
  cost = (int)guild->query_next_level_xp(me);
  have = (int)me->query_xp();
  write(_LANG_GUILD_ROOM_COST);
  return 1;
}

// A member of the guild goes up a level of it, paying the experience the guild
// asks for the next one.
int do_advance(string str)
{
  object me, guild;
  int cost, have;

  me = this_player();
  if (!me || !strlen(room_guild) ||
      _guild_path(me->query_guild_ob()) != _guild_path(room_guild))
  {
    notify_fail(_LANG_GUILD_ROOM_NOT_MEMBER);
    return 0;
  }

  guild = load_object(room_guild);
  if (!guild)
  {
    notify_fail(_LANG_GUILD_ROOM_NOT_MEMBER);
    return 0;
  }

  if ((int)me->query_guild_level() >= (int)guild->query_max_level())
  {
    notify_fail(_LANG_GUILD_ROOM_MAX_LEVEL);
    return 0;
  }

  cost = (int)guild->query_next_level_xp(me);
  have = (int)me->query_xp();
  if (have < cost)
  {
    notify_fail(_LANG_GUILD_ROOM_NOT_ENOUGH_XP);
    return 0;
  }

  me->adjust_xp(-cost);
  me->adjust_guild_level(1);

  log_file("guilds", "[" + ctime(time(), 4) + "] " + me->query_cap_name() +
           " advances to level " + me->query_guild_level() + " of " +
           room_guild + "\n");

  tell_room(environment(me), _LANG_GUILD_ROOM_ADVANCED_OTHERS, ({ me }));
  me->save();
  return 1;
}

mixed * stats()
{
  return ({ ({ "Room Guild", room_guild }) });
}
