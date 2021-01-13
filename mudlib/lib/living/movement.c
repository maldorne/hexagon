
#include <room/room.h>
#include <basic/move.h>
#include <translations/exits.h>
#include <translations/races.h>

// from here we inherit object.c
inherit container "/lib/core/basic/container";

string msgout, msgin, mmsgout, mmsgin;

void create()
{
  msgout = _LANG_RACES_MSG_IN_STD;
  msgin = _LANG_RACES_MSG_OUT_STD;
  mmsgout = _LANG_RACES_MMSG_IN_STD;
  mmsgin = _LANG_RACES_MMSG_OUT_STD;

  // from here we inherit object.c, were the call to
  // setup is, so it must be the last create call
  container::create();
}

string * query_move_messages()
{
  return ({ msgout, msgin, mmsgout, mmsgin });
}

void setmin(string str) { msgin = str; }
void setmout(string str) { msgout = str; }
void setmmin(string str) { mmsgin = str; }
void setmmout(string str) { mmsgout = str; }

int move_living(string dir, mixed dest, varargs mixed message, mixed enter)
{
  int ret, no_see;
  string arrive, leave, my_short, aux;
  object last, new_env;

  no_see = 0;

  if (!msgout)
  {
    msgout = _LANG_RACES_MSG_IN_STD;
    msgin = _LANG_RACES_MSG_OUT_STD;
  }

  if (!mmsgout)
  {
    mmsgout = _LANG_RACES_MMSG_IN_STD;
    mmsgin = _LANG_RACES_MMSG_OUT_STD;
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

  if (arrive)
    ret = container::move(dest, arrive[1..], leave[1..]);
  else
    ret = container::move(dest);

  // important: MOVE_OK == 0, value != 0 means it did not move
  if (ret)
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

mixed * stats()
{
  return container::stats() + ({
      ({ "msgout", msgout, }),
      ({ "msgin", msgin, }),
      ({ "mmsgout", mmsgout, }),
      ({ "mmsgin", mmsgin, }),
    });
}
