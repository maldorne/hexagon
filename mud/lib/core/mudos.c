
/* 
 *  Unique object storing global values about the current
 *  operation being performed, providing functions to the
 *  mudlib to emulate MudOS internals.
 */

#include <mud/mudos.h>

inherit hb "/lib/core/heart_beats";

private static object initiator_player, initiator_object;
private static string current_verb;

void create()
{
  current_verb = "";
  hb::create();
}

string query_current_verb() { return current_verb; }
void set_current_verb(string v) { current_verb = v; }

int set_initiator_object(object ob) 
{
  if (!mudlib_privileges()) 
  {
    stderr("Illegal set_initiator_object with <" + object_name(ob) + ">\n");
    return 0;
  }
  
  initiator_object = ob;
  return 1;
}

int set_initiator_player(object player) 
{
  if (!mudlib_privileges()) 
  {
    stderr("Illegal set_initiator_player with <" + object_name(player) + ">\n");
    return 0;
  }
  
  initiator_player = player;
  return 1;
}

object this_player(varargs int i) 
{
  // TODO: privileges?

  if (i && (i == 1))
    return initiator_player;

  return initiator_object;
}

// call init in ob2 with ob1 as this_player() 
void do_init(object ob1, object ob2) 
{
  if (!mudlib_privileges()) 
  {
    stderr("Illegal do_init with <" + object_name(ob1) + "> and <" + object_name(ob2) + ">\n");    
    return;
  }

  initiator_player = ob1;

  catch(call_other(ob2, "__call_other", "init"));
}
