
#include <mud/config.h>
#include <room/room.h>
#include <basic/move.h>
#include <translations/exits.h>
#include <translations/races.h>
#include <common/properties.h>
#include <language.h>
#include <room/location.h>

// from here we inherit object.c
inherit container "/lib/core/basic/container";

// The four movement messages, in one array so they travel together:
//   [0] out   (walking away)     [1] in   (walking in)
//   [2] mout  (teleporting away) [3] min  (teleporting in)
// An empty slot means "use the current server's language default"; only a
// value a builder/race actually set is stored. This keeps a player/NPC
// savefile free of language-specific move strings, so a character moves
// between instances of different languages and picks up that instance's
// defaults. Races (re)apply their own phrasing every login via start_player,
// in the current language, so a stored race string is always refreshed and
// never pins the character to one language.
#define MOVE_MSG_OUT  0
#define MOVE_MSG_IN   1
#define MOVE_MSG_MOUT 2
#define MOVE_MSG_MIN  3

string * move_msgs;

void create()
{
  move_msgs = ({ "", "", "", "" });

  // from here we inherit object.c, were the call to
  // setup is, so it must be the last create call
  container::create();
}

// Resolve a slot: the stored value if set, else the current-language default.
private string _move_msg(int idx)
{
  if (move_msgs && idx < sizeof(move_msgs) &&
      stringp(move_msgs[idx]) && strlen(move_msgs[idx]))
    return move_msgs[idx];

  switch (idx)
  {
    case MOVE_MSG_OUT:  return _LANG_RACES_MSG_OUT_STD;
    case MOVE_MSG_IN:   return _LANG_RACES_MSG_IN_STD;
    case MOVE_MSG_MOUT: return _LANG_RACES_MMSG_OUT_STD;
    case MOVE_MSG_MIN:  return _LANG_RACES_MMSG_IN_STD;
  }
  return "";
}

private void _ensure_move_msgs()
{
  if (!move_msgs || sizeof(move_msgs) < 4)
    move_msgs = ({ "", "", "", "" });
}

// Resolved messages (defaults filled in), the public 4-tuple contract.
string * query_move_messages()
{
  return ({ _move_msg(MOVE_MSG_OUT), _move_msg(MOVE_MSG_IN),
            _move_msg(MOVE_MSG_MOUT), _move_msg(MOVE_MSG_MIN) });
}

// The raw stored array (empty slots = default), for savefile / builder use.
string * query_move_msgs() { return move_msgs; }
void set_move_msgs(string * a) { move_msgs = a; _ensure_move_msgs(); }

void set_min(string str)   { _ensure_move_msgs(); move_msgs[MOVE_MSG_IN]   = str ? str : ""; }
void set_mout(string str)  { _ensure_move_msgs(); move_msgs[MOVE_MSG_OUT]  = str ? str : ""; }
void set_mmin(string str)  { _ensure_move_msgs(); move_msgs[MOVE_MSG_MIN]  = str ? str : ""; }
void set_mmout(string str) { _ensure_move_msgs(); move_msgs[MOVE_MSG_MOUT] = str ? str : ""; }

string query_min()   { return _move_msg(MOVE_MSG_IN); }
string query_mout()  { return _move_msg(MOVE_MSG_OUT); }
string query_mmin()  { return _move_msg(MOVE_MSG_MIN); }
string query_mmout() { return _move_msg(MOVE_MSG_MOUT); }

int move_living(string dir, mixed dest, varargs mixed message, mixed enter)
{
  int ret, no_see;
  string arrive, leave, my_short, aux;
  object last, new_env;

  no_see = 0;

  last = environment();

  if (!this_object()->query_invis())
  {
    // room messages use the display name (query_cap_name), not short(): for a
    // generated NPC short() is the examine header "<kind> (<Name>)", so using it
    // here leaked the proper name into "X leaves north" while the rest of the
    // room text (lists, combat, door open/close) shows the bare kind. Keep the
    // name for the examine header only.
    my_short = (string)this_object()->query_cap_name();
    if (my_short)
      my_short = capitalize(my_short);
  }

  // little fix, neverbot 7/03
  // north -> the north, etc
  aux = ROOM_HAND->query_exit_dir(dir);

  if (!my_short || my_short == "" || (stringp(message) && message == "none"))
  {
    no_see = 1;
  }
  else if (!dir || dir == "X")
  {
    // we are teleporting
    leave = implode(explode(query_mmout(), "$N"), my_short) + "\n";
    arrive = implode(explode(query_mmin(), "$N"), my_short) + "\n";
  }
  else
  {
    if (!enter || !pointerp(enter))
      enter = ({ 0, DIR_SOME_PLACE });

    if (pointerp(message))
      message = message[0];

    leave = implode(explode(implode(explode((message ? message : query_mout()), "$N"),
              my_short), "$T"), aux) + "\n";

    switch (enter[0])
    {
      case 0 :
        arrive = implode(explode(implode(explode(query_min(), "$N"), my_short), "$F"),
            enter[1])+"\n";
        break;
      case 1 :
        arrive = implode(explode(enter[1], "$N"), my_short)+"\n";
        break;
    }
  }

  // locations
  if (CONFIG_USE_LOCATIONS)
  {
    // if the destination is a .o file (location), we have to change the destination to the
    // real location object
    if (stringp(dest) && (dest[strlen(dest)-2..strlen(dest)-1] == ".o"))
    {
      dest = load_object(LOCATION_HANDLER)->load_location(dest);
    }
    else
    {
      // if the destination is a .c file (room), check if a location already exists
      // for that room
      string alternative;

      if (stringp(dest))
        dest = load_object(dest);

      if (!objectp(dest))
        return MOVE_EMPTY_DEST;  
        
      if ((alternative = load_object(LOCATION_HANDLER)->get_location_file_name_from_room(dest)) && 
          (file_size(alternative) > 0))
        dest = load_object(LOCATION_HANDLER)->load_location(alternative);
    }
  }

  if (arrive)
    ret = move(dest, arrive[1..], leave[1..]);
  else
    ret = move(dest);

  // important: MOVE_OK == 0, value != 0 means it did not move
  if (ret != MOVE_OK)
    return 0;

  new_env = environment();

  // Record area exploration for players (no-op for monsters and for
  // rooms-only games, which have no location/area).
  if (interactive(this_object()))
    this_object()->check_area_exploration(new_env);

  if (interactive(this_object()) && !query_property(UNKNOWN_MOVE_PROP))
    this_object()->do_look();

  if (!dir || dir == "X")
    return 1;
  // else if (no_see)
  //   return ({ });

  return 1;
}

void run_away()
{
  mixed * direcs;
  int i;

  if (this_object()->query_property(PASSED_OUT_PROP))
  {
    tell_object(this_object(), _LANG_RUN_AWAY_NOT_IN_CONDITIONS);
    return;
  }

  direcs = (mixed *)environment()->query_dest_dir();
  i = random(sizeof(direcs)/2)*2;

  tell_object(this_object(), _LANG_RUN_AWAY_RUN_AWAY);
  tell_room(environment(this_object()), _LANG_RUN_AWAY_RUN_AWAY_ROOM, ({ this_object() }));
  this_object()->insert_action(direcs[i]);
  // tell_object(this_object(), _LANG_RUN_AWAY_SEE_FEET_RUNNING);
  // write(_LANG_RUN_AWAY_FAILED_ESCAPE);
}

mixed * stats()
{
  return container::stats() + ({
      ({ "move_msgs (raw)", move_msgs, }),
      ({ "move_msgs (resolved)", query_move_messages(), }),
    });
}
