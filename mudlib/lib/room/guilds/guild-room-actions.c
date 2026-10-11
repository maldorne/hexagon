// Guild room: the main hall of a guild's house, where its members gather.
// Shared by the guild room (/lib/room/guilds/guild-room.c) and the guild room
// location component (/lib/location/components/guild-room.c). Its sign points
// at the ranking of the guild (the ranking command).
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
}

mixed * stats()
{
  return ({ ({ "Room Guild", room_guild }) });
}
