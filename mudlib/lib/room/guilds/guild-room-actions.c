// Guild room: the main hall of a guild's house, where its members gather.
// Shared by the guild room (/lib/room/guilds/guild-room.c) and the guild room
// location component (/lib/location/components/guild-room.c).
//
// It keeps the guild's most famous members: whoever of the guild walks in is
// weighed against the list. The list is the guild's, not the hall's, so it is
// kept in one file per guild in the game's save directory.
//
// Ported from the CcMud guild room, Folken 12/2006 and 07/2010.

#include <language.h>

// how many members the list of the most famous keeps
#define GUILD_ROOM_FAMOUS 10

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

// Where the list of the most famous members is kept: one file per guild, in
// the save directory of the game the hall belongs to.
private string _famous_dir()
{
  object place;

  place = query_guild_room_place();
  return game_save_dir(place ? place : this_object()) + "guilds/";
}

private string _famous_file()
{
  string * parts;

  parts = explode(_guild_path(room_guild), "/");
  return _famous_dir() + parts[sizeof(parts) - 1] + "-famous.json";
}

// The most famous members, as name -> ({ guild level, class name }).
mapping query_famous_members()
{
  string text;
  mixed data;

  if (!strlen(room_guild) || !file_exists(_famous_file()))
    return ([ ]);

  text = read_file(_famous_file());
  data = (text && strlen(text)) ? json_decode(text) : nil;
  return mappingp(data) ? data : ([ ]);
}

private void _save_famous(mapping famous)
{
  mkdir(_famous_dir());
  if (file_exists(_famous_file()))
    rm(_famous_file());
  write_file(_famous_file(), json_encode(famous));
}

// Weigh a member who walks in against the list: a place for them while it is
// not full, or the place of whoever has the lowest guild level below theirs.
// Coders and those who have just joined do not count.
void note_member(object who)
{
  mapping famous;
  string name, lowest;
  string * names;
  int level, i;

  if (!who || !interactive(who) || who->query_coder() ||
      !strlen(room_guild) ||
      _guild_path(who->query_guild_ob()) != _guild_path(room_guild))
    return;

  level = (int)who->query_guild_level();
  if (level <= 1)
    return;

  famous = query_famous_members();
  name = (string)who->query_cap_name();

  // already on it: only the level may have changed
  if (!undefinedp(famous[name]))
  {
    if (famous[name][0] != level)
    {
      famous[name] = ({ level, (string)who->query_class_name() });
      _save_famous(famous);
    }
    return;
  }

  if (map_sizeof(famous) < GUILD_ROOM_FAMOUS)
  {
    famous[name] = ({ level, (string)who->query_class_name() });
    _save_famous(famous);
    tell_object(who, _LANG_GUILD_ROOM_NOW_FAMOUS);
    return;
  }

  names = map_indices(famous);
  lowest = nil;
  for (i = 0; i < sizeof(names); i++)
    if (famous[names[i]][0] < level &&
        (!lowest || famous[names[i]][0] < famous[lowest][0]))
      lowest = names[i];

  if (!lowest)
    return;

  map_delete(famous, lowest);
  famous[name] = ({ level, (string)who->query_class_name() });
  _save_famous(famous);
  tell_object(who, _LANG_GUILD_ROOM_NOW_FAMOUS_OVER);
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
  add_action("do_members", _LANG_GUILD_ROOM_CMDS_MEMBERS);

  if (this_player())
    note_member(this_player());
}

int do_members(string str)
{
  mapping famous;
  string * names;
  string ret;
  int i, j;

  famous = query_famous_members();
  if (!map_sizeof(famous))
  {
    tell_object(this_player(), _LANG_GUILD_ROOM_NO_FAMOUS);
    return 1;
  }

  // highest guild level first
  names = map_indices(famous);
  for (i = 0; i < sizeof(names); i++)
    for (j = i + 1; j < sizeof(names); j++)
      if (famous[names[j]][0] > famous[names[i]][0])
      {
        string swap;

        swap = names[i];
        names[i] = names[j];
        names[j] = swap;
      }

  ret = _LANG_GUILD_ROOM_FAMOUS_HEADER;
  for (i = 0; i < sizeof(names); i++)
    ret += "  " + names[i] + " (" + _LANG_GUILD_ROOM_LEVEL + " " +
           famous[names[i]][0] + ")\n";

  tell_object(this_player(), ret + "\n");
  return 1;
}

mixed * stats()
{
  return ({ ({ "Room Guild", room_guild }) });
}
