/* 
 * goto.c
 * Externalized by Baldrick june '97
 */

#include <mud/cmd.h>

inherit CMD_BASE;

string query_usage()
{
  return "goto <room file path | coder name>";
}

string query_help()
{
  return "Teleports the coder to the given room if a file path is used, " + 
    "or the home room of a coder if a coder room is used.";
}

static int cmd(string str, object me, string verb) 
{
  object dest;
  string *names, nick;

  notify_fail("Teleport where?\n");
  
  if (!strlen(str)) 
    return 0;

  if (!me->query_administrator())
    me->set_invis(0);

  nick = (string)me->expand_nickname(str);
  dest = find_living(nick);
  
  if (dest) 
  {
    if (dest = environment(dest)) 
    {
      if (dest == environment()) 
      {
        notify_fail("Miras a tu alrededor y te das cuenta de que ya estas alli.\n");
        return 0;
      }
      
      me->move_player("X", dest);
      return 1;
    } 
    else 
    {
      notify_fail(capitalize(nick) + " no se encuentra en ninguna localizacion.\n");
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
      notify_fail("No existe esa room.\n");
      return 0;
    }

    if (!(dest = find_object(str))) 
    {
      // catch(str->come_and_see_the_stars());
      catch(dest = load_object(str));
    }

    if (!dest) 
    {
      notify_fail("Fallo al cargar '"+str+"'\n");
      return 0;
    } 
    else if (!dest->query_property("location")) 
    {
      notify_fail("No es una room: '"+str+"'\n");
      return 0;
    } 
    else 
    {
      if (dest == environment()) 
      {
        notify_fail("Miras a tu alrededor y te das cuenta de que ya estas alli.\n");
        return 0;
      }

      me->move_living("X", dest);
      me->set_trivial_action();
      return 1;
    }
  }
} /* teleport() */

