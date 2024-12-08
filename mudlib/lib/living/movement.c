
#include <room/room.h>
#include <basic/move.h>
#include <translations/exits.h>
#include <translations/races.h>
#include <common/properties.h>
#include <language.h>
#include <room/location.h>

// from here we inherit object.c
inherit container "/lib/core/basic/container";

string msgout, msgin, mmsgout, mmsgin;

void create()
{
  msgout = _LANG_RACES_MSG_IN_STD;
  msgin = _LANG_RACES_MSG_OUT_STD;
  mmsgout = _LANG_RACES_MMSG_OUT_STD;
  mmsgin = _LANG_RACES_MMSG_IN_STD;

  // from here we inherit object.c, were the call to
  // setup is, so it must be the last create call
  container::create();
}

string * query_move_messages()
{
  return ({ msgout, msgin, mmsgout, mmsgin });
}

void set_min(string str) { msgin = str; }
void set_mout(string str) { msgout = str; }
void set_mmin(string str) { mmsgin = str; }
void set_mmout(string str) { mmsgout = str; }

string query_min() { return msgin; }
string query_mout() { return msgout; }
string query_mmin() { return mmsgin; }
string query_mmout() { return mmsgout; }

int move_living(string dir, mixed dest, varargs mixed message, mixed enter)
{
  int ret, no_see;
  string arrive, leave, my_short, aux;
  object last, new_env;

  no_see = 0;

  if (!msgout)
  {
    msgin = _LANG_RACES_MSG_IN_STD;
    msgout = _LANG_RACES_MSG_OUT_STD;
  }

  if (!mmsgout)
  {
    mmsgin = _LANG_RACES_MMSG_IN_STD;
    mmsgout = _LANG_RACES_MMSG_OUT_STD;
  }

  last = environment();

  if (!this_object()->query_invis())
  {
    my_short = (string)this_object()->short();
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
    leave = implode(explode(mmsgout, "$N"), my_short) + "\n";
    arrive = implode(explode(mmsgin, "$N"), my_short) + "\n";
  }
  else
  {
    if (!enter || !pointerp(enter))
      enter = ({ 0, DIR_SOME_PLACE });

    if (pointerp(message))
      message = message[0];

    leave = implode(explode(implode(explode((message ? message : msgout), "$N"),
              my_short), "$T"), aux) + "\n";

    switch (enter[0])
    {
      case 0 :
        arrive = implode(explode(implode(explode(msgin, "$N"), my_short), "$F"),
            enter[1])+"\n";
        break;
      case 1 :
        arrive = implode(explode(enter[1], "$N"), my_short)+"\n";
        break;
    }
  }

  // locations
  // if the destination is a .o file, we have to change the destination to the
  // real location object
  if (stringp(dest) && (dest[strlen(dest)-2..strlen(dest)-1] == ".o"))
  {
    dest = load_object(LOCATION_HANDLER)->load_location(dest);
  }
  else
  {
    string alternative;

    if (stringp(dest))
      dest = load_object(dest);

    if (!objectp(dest))
      return MOVE_EMPTY_DEST;  
      
    if ((alternative = load_object(LOCATION_HANDLER)->get_location_file_name_from_room(dest)) && 
        (file_size(alternative) > 0))
      dest = load_object(LOCATION_HANDLER)->load_location(alternative);
  }

  if (arrive)
    ret = container::move(dest, arrive[1..], leave[1..]);
  else
    ret = container::move(dest);

  // important: MOVE_OK == 0, value != 0 means it did not move
  if (ret != MOVE_OK)
    return 0;

  new_env = environment();

  if (interactive(this_object()) && !query_property(UNKNOWN_MOVE_PROP))
    this_object()->do_look();

  if (!dir || dir == "X")
    return 1;
  // else if (no_see)
  //   return ({ });

  return 1;
} /* move_player() */

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
      ({ "msgout", msgout, }),
      ({ "msgin", msgin, }),
      ({ "mmsgout", mmsgout, }),
      ({ "mmsgin", mmsgin, }),
    });
}
