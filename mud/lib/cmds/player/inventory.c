
#include <mud/cmd.h>

inherit CMD_BASE;

void setup()
{
  position = 0;
}

static int cmd(string str, object me, string verb) 
{
  if (me->query_dead())
  {
    notify_fail("Estando muerto no necesitas conocer eso.\n");
    return 0;
  }

  tell_object(me, me->query_living_contents(1));
  me->set_trivial_action();
  return 1;
}

