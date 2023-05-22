
#include <mud/cmd.h>

inherit CMD_BASE;

void setup()
{
  set_aliases(({ "restore" }));
  set_usage("restore");
  set_help("Your health points and energy points are restored.");
}

static int cmd(string str, object me, string verb) 
{
  if (!me->query_coder())
    return 0;

  if (me->query_dead())
    me->remove_ghost();
    
  me->set_hp(me->query_max_hp());
  me->set_gp(me->query_max_gp());

  tell_object(this_player(), "%^YELLOW%^Your health and stamina are restored.%^RESET%^\n");
  
  // tell_room(environment(this_player()), 
  //       "%^YELLOW%^The health and stamina of " + this_player()->query_cap_name()+
  //       " are restored.%^RESET%^\n", ({this_player()}));
  
  return 1;
}
