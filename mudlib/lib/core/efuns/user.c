
#include <user/user.h>
#include <user/player.h>
#include "users/this_player.c"
#include "users/interactive.c"
#include "users/userp.c"
#include "users/catch_tell.c"

static object real_this_player()
{
  object * list;
  int i;

  if (this_player())
    return this_player();
  // if (this_player(1))
  //   return this_player(1);

  list = previous_objects();

  for (i = sizeof(list)-1; i >= 0; i--)
    if (interactive(list[i]))
    {
      return list[i];
    }

  return nil;
}

// /secure/simul_efun/player_exists.c
// from the RotD Mudlib
// returns true if there is such a player
// created by Descartes of Borg 930822

static int file_exists(string str);

static int player_exists(string str)
{
  int ret;

  // seteuid((string)master()->get_root_uid());
  ret = file_exists(PLAYERS_SAVE_DIR + "/" + str[0..0] + "/" + str + ".o");
  // seteuid(0);
  return ret;
}

static int user_exists(string str)
{
  int ret;

  // seteuid((string)master()->get_root_uid());
  ret = file_exists(USERS_SAVE_DIR + "/" + str[0..0] + "/" + str + ".o");
  // seteuid(0);
  return ret;
}

static object find_player(string str)
{
  object handler;
  handler = find_object(USER_HANDLER);

  return handler->find_player(str);
}

static object find_user(string str)
{
  object handler;
  handler = find_object(USER_HANDLER);

  return handler->find_user(str);
}

/*
object find_player(string str)
{
  mapping user_data;
  string * user_ids;
  object handler;
  int i;

  handler = find_object(USER_HANDLER);
  user_data = handler->query_user_data();
  user_ids = handler->query_user_ids();

  for (i = 0; i < sizeof(user_ids); i++)
  {
    if (str == user_data[user_ids[i]][1])
      return user_data[user_ids[i]][0];
  }

  return nil;

  // object ob;

  // if ((ob = efun::find_player(str)))
  //   if ((int)ob->query_invis() == 2)
  //     return nil;
  // return ob;
}
*/
