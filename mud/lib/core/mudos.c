
/* 
 *  Unique object storing global values about the current
 *  operation being performed, providing functions to the
 *  mudlib to emulate MudOS internals.
 */

#include <mud/mudos.h>


private static object command_giver, command_actor;

int set_this_player(object player) 
{
  if (!mudlib_privileges()) 
  {
    stderr("Illegal set_this_player with <" + object_name(player) + ">\n");
    return 0;
  }
  
  command_actor = player;
  return 1;
}

int set_effective_this_player(object player) 
{
  if (!mudlib_privileges()) 
  {
    stderr("Illegal set_effective_this_player with <" + object_name(player) + ">\n");
    return 0;
  }
  
  command_giver = player;
  return 1;
}

object this_player(varargs int i) 
{
  /*
  if (!AUTO_PRIV()) 
  {
    illegal();
    return 0;
  }
  */

  if (i && (i == 1))
    return command_giver;

  return command_actor;
}

// call init in ob2 with ob1 as this_player() 
void do_init(object ob1, object ob2) 
{
  if (!mudlib_privileges()) 
  {
    // illegal();
    stderr("Illegal do_init with <" + object_name(ob1) + "> and <" + object_name(ob2) + ">\n");    
    return;
  }

  command_giver = ob1;

  catch(call_other(ob2, "__call_other", "init"));
}


