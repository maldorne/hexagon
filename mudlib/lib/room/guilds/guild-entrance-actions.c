// Guild entrance: the way into a guild's house, where the guild is explained
// and where one joins it or leaves the guild one belongs to. Shared by the
// guild entrance room (/lib/room/guilds/guild-entrance-room.c) and the guild
// entrance location component (/lib/location/components/guild-entrance.c).
// Who may go further in is decided by the guards of the house, not here.
//
// Joining a guild starts a wait of GUILD_JOIN_WAIT seconds of play before the
// same character may join another, so nobody hops from guild to guild to get
// inside one.
//
// Ported from the CcMud guild entrance, neverbot 12/2006.

#include <living/social.h>
#include <language.h>

// the guild this is the entrance of, as the path of its object
private string entrance_guild;
static object entrance_sign;

int query_guild_entrance() { return 1; }

// Where the entrance stands. The component answers with its location.
object query_entrance_place() { return this_object(); }

void create()
{
  entrance_guild = "";
}

string query_entrance_guild() { return entrance_guild; }
void set_entrance_guild(string path) { entrance_guild = path ? path : ""; }

// A guild object named with or without its extension is the same guild.
private string _guild_path(string path)
{
  if (!path || !strlen(path))
    return "";
  if (strlen(path) > 2 && path[strlen(path) - 2 ..] == ".c")
    path = path[0 .. strlen(path) - 3];
  return path;
}

private object _guild()
{
  if (!strlen(entrance_guild))
    return nil;
  return load_object(entrance_guild);
}

// The names of the races or classes a guild lists by id: the game's own object
// for each when there is one, the bare id otherwise.
private string _names_of(string * ids, string dir)
{
  string * names;
  object ob;
  int i;

  names = ({ });
  for (i = 0; i < sizeof(ids); i++)
  {
    ob = nil;
    catch(ob = load_object(dir + ids[i]));
    names += ({ (ob && ob->query_name()) ? (string)ob->query_name() : ids[i] });
  }

  return implode(names, ", ");
}

// What the sign of the entrance says: the guild, who it admits and how to
// join or leave.
private string _sign_text()
{
  object guild, place;
  string text, root;

  guild = _guild();
  if (!guild)
    return _LANG_GUILD_ENTRANCE_SIGN_NO_GUILD;

  place = query_entrance_place();
  root = place ? game_root(place) : "";

  text = _LANG_GUILD_ENTRANCE_SIGN_TITLE + (string)guild->query_short() + "\n\n" +
         (string)guild->query_long() + "\n";

  if (sizeof(guild->query_legal_races()))
    text += _LANG_GUILD_ENTRANCE_SIGN_RACES +
            _names_of(guild->query_legal_races(), root + "obj/races/") + ".\n";
  if (sizeof(guild->query_legal_classes()))
    text += _LANG_GUILD_ENTRANCE_SIGN_CLASSES +
            _names_of(guild->query_legal_classes(), "/lib/obj/classes/") + ".\n";

  return text + "\n" + _LANG_GUILD_ENTRANCE_SIGN_COMMANDS;
}

// Put up the sign that explains the guild, replacing any earlier one, so a
// place never has two.
object create_entrance_sign(object where)
{
  if (entrance_sign)
    entrance_sign->dest_me();

  entrance_sign = where->add_sign(_LANG_GUILD_ENTRANCE_SIGN_LONG, _sign_text());
  return entrance_sign;
}

// Take the sign down again, when the entrance leaves its place.
void take_down_entrance_sign()
{
  if (entrance_sign)
    entrance_sign->dest_me();
  entrance_sign = nil;
}

void init()
{
  add_action("do_join", _LANG_GUILD_ENTRANCE_CMDS_JOIN);
  add_action("do_leave", _LANG_GUILD_ENTRANCE_CMDS_LEAVE);
}

int do_join(string str)
{
  object me, guild;
  int wait;

  me = this_player();
  guild = _guild();
  if (!me || !guild)
  {
    notify_fail(_LANG_GUILD_ENTRANCE_NO_GUILD);
    return 0;
  }

  if (_guild_path(me->query_guild_ob()) == _guild_path(entrance_guild))
  {
    notify_fail(_LANG_GUILD_ENTRANCE_ALREADY_MEMBER);
    return 0;
  }

  if (me->query_guild_ob())
  {
    notify_fail(_LANG_GUILD_ENTRANCE_OTHER_GUILD);
    return 0;
  }

  if (!guild->query_legal_player(me))
  {
    notify_fail(_LANG_GUILD_ENTRANCE_NOT_LEGAL);
    return 0;
  }

  wait = (int)me->query_guild_join_wait();
  if (wait > 0)
  {
    notify_fail(_LANG_GUILD_ENTRANCE_TOO_SOON);
    return 0;
  }

  me->set_guild_ob(entrance_guild);
  if (!me->query_guild_ob())
  {
    notify_fail(_LANG_GUILD_ENTRANCE_NOT_LEGAL);
    return 0;
  }

  log_file("guilds", "[" + ctime(time(), 4) + "] " + me->query_cap_name() +
           " joins " + entrance_guild + "\n");

  tell_object(me, _LANG_GUILD_ENTRANCE_JOINED);
  tell_room(environment(me), _LANG_GUILD_ENTRANCE_JOINED_OTHERS, ({ me }));
  me->save();
  return 1;
}

int do_leave(string str)
{
  object me, guild;
  int wait;

  me = this_player();
  if (!me || !me->query_guild_ob())
  {
    notify_fail(_LANG_GUILD_ENTRANCE_NO_GUILD_TO_LEAVE);
    return 0;
  }

  guild = load_object(me->query_guild_ob());
  // a guild that does not load is not left either, in case it is broken
  if (!guild || !guild->can_abandon(me))
  {
    notify_fail(_LANG_GUILD_ENTRANCE_CANNOT_LEAVE);
    return 0;
  }

  // warned before deciding: leaving now means no guild at all for a while
  wait = (int)me->query_guild_join_wait();
  if (wait > 0)
    tell_object(me, _LANG_GUILD_ENTRANCE_LEAVE_WAIT_WARNING);

  tell_object(me, _LANG_GUILD_ENTRANCE_LEAVE_CONFIRM);
  input_to("confirm_leave", 0, me);
  return 1;
}

void confirm_leave(string answer, object me)
{
  object guild;

  if (!me)
    return;

  answer = answer ? lower_case(trim(answer)) : "";
  if (member_array(answer, _LANG_GUILD_ENTRANCE_YES) == -1)
  {
    tell_object(me, _LANG_GUILD_ENTRANCE_LEAVE_CANCELLED);
    return;
  }

  guild = load_object(me->query_guild_ob());
  if (guild)
    guild->guild_abandon(me);

  log_file("guilds", "[" + ctime(time(), 4) + "] " + me->query_cap_name() +
           " leaves " + me->query_guild_ob() + "\n");

  me->set_guild_ob("");
  tell_object(me, _LANG_GUILD_ENTRANCE_LEFT);
  me->save();
}

mixed * stats()
{
  return ({ ({ "Entrance Guild", entrance_guild }) });
}
