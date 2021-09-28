// qatt from the old domain_communicator. 
// cmds-ized by neverbot 2001

#include <mud/cmd.h>
#include <language.h>

inherit CMD_BASE;

string query_usage()
{
  return "peace";
}

string query_help() 
{
  return "Immediately stops current combats in your environment.";
}

static int cmd(string str, object me, string verb) 
{
  object *ob;
  object *att;
  object *call;
  int i, s;

  ob = all_inventory(environment(this_player()));

  for (i = 0; i < sizeof(ob); i++) 
  {
    if (living(ob[i])) 
    {
      att = ob[i]->query_attacker_list();
      call = ob[i]->query_call_outed();

      for (s = 0; s < sizeof(att); s++) 
      {
        ob[i]->stop_fight(att[s]);
        att[s]->stop_fight(ob[i]);
      }

      for (s = 0; s < sizeof(call); s++) 
      {
        ob[i]->stop_fight(call[s]);
        call[s]->stop_fight(ob[i]);
      }
    }
  }

  tell_object(this_player(), _LANG_PEACE_MESSAGES_MINE);
  tell_room(environment(this_player()), _LANG_PEACE_MESSAGES_OTHERS, ({ this_player() }));
  return 1;
}
