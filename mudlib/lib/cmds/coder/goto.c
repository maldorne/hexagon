/* 
 * goto.c
 * Externalized by Baldrick june '97
 */

#include <mud/cmd.h>

inherit CMD_BASE;

string query_usage()
{
  return "goto <room file path | user name>";
}

string query_help()
{
  return "Teleports the coder to a given destination.\nYou will be moved " +
    "to the given room if a file path is used, " + 
    "or the current room of a user if a user name is used.";
}

static int cmd(string str, object me, string verb) 
{
  object dest;
  string *names, nick;

  notify_fail("goto where?\n");
  
  if (!strlen(str)) 
  {
    write("Syntax: " + query_usage() + "\n\n");    
    return 1;
  }

  if (!me->query_administrator())
    me->set_invis(0);

  nick = (string)me->expand_nickname(str);
  dest = find_living(nick);
  
  if (dest) 
  {
    if (dest = environment(dest)) 
    {
      if (dest == environment(me)) 
      {
        notify_fail("You look around you and notice you are already there.\n");
        return 0;
      }
      
      me->move_living("X", dest);
      return 1;
    } 
    else 
    {
      notify_fail(capitalize(nick) + " has no environment.\n");
      return 0;
    }
  } 
  else 
  {
    names = (string *)get_files(str);

    if (sizeof(names)) 
    {
      str = names[0];
    } 
    else 
    {
      notify_fail("That room does not exist.\n");
      return 0;
    }

    if (!(dest = find_object(str))) 
    {
      // catch(str->come_and_see_the_stars());
      catch(dest = load_object(str));
    }

    if (!dest) 
    {
      notify_fail("Error loading '"+str+"'\n");
      return 0;
    } 
    else if (!dest->query_property("location")) 
    {
      notify_fail("That is not a room: '"+str+"'\n");
      return 0;
    } 
    else 
    {
      if (dest == environment(me)) 
      {
        notify_fail("You look around you and notice you are already there.\n");
        return 0;
      }

      me->move_living("X", dest);
      me->set_trivial_action();
      return 1;
    }
  }
}
