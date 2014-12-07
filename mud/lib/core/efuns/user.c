
// Non-dgd efuns
// neverbot, 03/2014

#include <user.h>

int interactive(object ob)
{
  if (query_ip_number(ob) == nil)
    return FALSE;

  return TRUE;
}

int userp(object ob)
{
  return interactive(ob);
}

static nomask object this_player() 
{
  return this_user();
}

// object this_player(varargs int flag)
// {
//   return this_user(flag);
// }

object real_this_player()
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

static nomask void notify_fail(string str) 
{
  this_player()->set_fail_msg(str);
}

static nomask string query_verb()
{
  return this_player()->query_current_verb();
}

// void notify_fail(mixed str) 
// {
//   if(!this_player())
//     return;

//   if (intp(str))
//     str = "" + str;

//   // neverbot 4/2003
//   // efun::notify_fail(sprintf("%-=*s", this_player()->query_cols(),str));
//   this_player()->set_notify_fail_msg(sprintf("%-=*s",
//                                      this_player()->query_cols(),str));
// } 


// /secure/simul_efun/user_exists.c
// from the RotD Mudlib
// returns true if there is such a player
// created by Descartes of Borg 930822

// TODO user_exists
int file_exists(string str);

int user_exists(string str) 
{
  int ret;

  // seteuid((string)master()->get_root_uid());
  ret = file_exists(USERS_SAVE_DIR + "/" + str[0..0] + "/" + str + ".o");
  // seteuid(0);
  return ret;
}

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
