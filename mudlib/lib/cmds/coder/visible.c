// cmds extracted from global.c, neverbot 4/2003

#include <mud/cmd.h>
#include <language.h>

inherit CMD_BASE;

void setup()
{
  set_aliases(({ "visible" }));
  set_usage("visible");
  set_help("Makes you visible again.");
}

static int cmd(string str, object me, string verb)
{
  if (!me->query_invis())
  {
    tell_object(me, "You are already visible.\n");
    return 1;
  }

  tell_object(me, "You become visible.\n");
  tell_room(environment(me), me->query_cap_name() + 
    " appears suddenly.\n", me);

  me->set_invis(0);
  return 1;
}
