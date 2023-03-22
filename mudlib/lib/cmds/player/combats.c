
#include <mud/cmd.h>
#include <translations/common.h>
#include <language.h>

inherit CMD_BASE;

void setup()
{
  set_aliases(_LANG_CMD_COMBATS_ALIAS);
  set_usage(_LANG_CMD_COMBATS_SYNTAX);
  set_help(_LANG_CMD_COMBATS_HELP);
}

static int cmd(string str, object me, string verb)
{
  object * obs;
  obs = me->query_attacker_list() + me->query_call_outed();

  if (!sizeof(obs)) 
  {
    notify_fail(_LANG_CMD_COMBATS_NOBODY);
    return 0;
  }
  
  tell_object(me, _LANG_CMD_COMBATS_MSG);
  return 1;
}
