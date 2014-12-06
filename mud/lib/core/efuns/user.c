
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

// Find first the object that is marked as living, and answers to the
// id 'str'.  A living object is an object that has called
// enable_commands().  The object must have set a name with
// set_living_name(), so its name will be entered into the hash table
// used to speed up the search for living objects.

// TODO find_living

object find_living(string name)
{
  // string nick;
  // object ob;

  // if (this_player() && (nick = (string)this_player()->expand_nickname(str)))
  //   str = nick;

  // if ((ob = efun::find_living(str)))
  //   if ((int)ob->query_invis() == 2)
  //     return nil;

  // return ob;

  return nil;
}

// Similar to find_living(), but only searches through objects that are
// interactive, or were once interactive.

// TODO find_player

object find_player(string str) 
{
  // object ob;

  // if ((ob = efun::find_player(str)))
  //   if ((int)ob->query_invis() == 2)
  //     return nil;
  // return ob;

  return nil;
} 
