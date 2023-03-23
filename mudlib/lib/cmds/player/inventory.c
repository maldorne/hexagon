
#include <mud/cmd.h>
#include <language.h>

inherit CMD_BASE;

void setup()
{
  set_aliases(_LANG_CMD_INVENTORY_ALIAS);
  set_usage(_LANG_CMD_INVENTORY_SYNTAX);
  set_help(_LANG_CMD_INVENTORY_HELP);
}

static int cmd(string str, object me, string verb) 
{
  if (me->query_dead())
  {
    notify_fail(_LANG_CMD_DEAD);
    return 0;
  }

  tell_object(me, me->query_living_contents(1));
  me->set_trivial_action();
  return 1;
}

