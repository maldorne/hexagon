// qatt del antiguo domain_communicator. 
// Hecho comando por neverbot 2001

#include <mud/cmd.h>

inherit CMD_BASE;

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

  tell_object(this_player(),"Alzas tu mano y exclamas: "+
      "%^YELLOW%^¡Dejad de luchar, mortales!%^RESET%^\n");
  
  tell_room(environment(this_player()), this_player()->query_cap_name()+
        " alza su mano y exclama: %^YELLOW%^¡Dejad de luchar, mortales!"+ 
        "%^RESET%^\n", ({this_player()}));
  return 1;
}
