
#include <mud/cmd.h>

inherit CMD_BASE;

static int cmd(string str, object me, string verb) 
{
  if (!me->query_coder())
    return 0;

  if (me->query_dead())
    me->remove_ghost();
    
  me->set_hp(me->query_max_hp());
  me->set_gp(me->query_max_gp());

  tell_object(this_player(),"%^YELLOW%^Tu vida y energía se recuperan.%^RESET%^\n");
  
  tell_room(environment(this_player()), 
        "%^YELLOW%^La vida y energía de " + this_player()->query_cap_name()+
        " se recuperan.%^RESET%^\n", ({this_player()}));
  
  return 1;
}
