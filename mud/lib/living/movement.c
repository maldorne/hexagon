
#include <room/room.h>
#include <basic/move.h>

// from here we inherit object.c
inherit container "/lib/core/basic/container";

string msgout, msgin, mmsgout, mmsgin;
// static int in_move;

void create()
{
  msgout = "@$N se va hacia $T.";
  msgin = "@$N llega desde $F.";
  mmsgout = "@$N desaparece en una nube de humo.";
  mmsgin = "@$N aparece del suelo.";

  // from here we inherit object.c, were the call to
  // setup is, so it must be the last create call
  container::create();
}

string * query_move_messages()
{
  return ({ msgout, msgin, mmsgout, mmsgin });
}

int setmin(string str)
{
  msgin = str;
  return 1;
}

int setmout(string str)
{
  msgout = str;
  return 1;
}

int setmmin(string str)
{
  mmsgin = str;
  return 1;
}

int setmmout(string str)
{
  mmsgout = str;
  return 1;
}

int move_living(string dir, mixed dest, varargs mixed message, mixed enter)
{
  int ret, no_see;
  string arrive, leave, my_short, aux;
  object last, new_env;

  no_see = 0;

  if (!msgout)
  {
    msgout = "@$N se va hacia $T.";
    msgin = "@$N llega desde $F.";
  }

  if (!mmsgout)
  {
    mmsgout = "@$N desaparece en una nube de humo.";
    mmsgin = "@$N aparece del suelo.";
  }

  last = environment();

  if (!this_object()->query_invis())
  {
    my_short = (string)this_object()->short();
    if (my_short)
      my_short = capitalize(my_short);
  }

  // little fix, neverbot 7/03
  // norte -> el norte, entrada -> la entrada, escaleras -> las escaleras, etc
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
      enter = ({ 0, "algún sitio" });

    if (pointerp(message))
      message = message[0];

    leave = implode(explode(implode(explode((message?message:msgout), "$N"),
              my_short), "$T"), aux)+"\n";

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

// int query_in_move() { return in_move; }

// int abort_move_player(string dir)
// {
//   tell_object(this_object(), "Dejas de moverte hacia "+dir+".\n");
//   in_move = 0;
//   return 1;
// }

mixed * stats()
{
  return container::stats() + ({
      ({ "msgout", msgout, }),
      ({ "msgin", msgin, }),
      ({ "mmsgout", mmsgout, }),
      ({ "mmsgin", mmsgin, }),
    });
}
