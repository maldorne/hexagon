
#include <mud/cmd.h>
#include <translations/common.h>
#include <language.h>

inherit CMD_BASE;

void setup()
{
  set_aliases(_LANG_CMD_CONDITION_ALIAS);
  set_usage(_LANG_CMD_CONDITION_SYNTAX);
  set_help(_LANG_CMD_CONDITION_HELP);
}

static int cmd(string str, object me, string verb) 
{
  string ret;
  object * obs;
  int i;

  if (me->query_dead())
  {
    notify_fail(_LANG_CMD_DEAD);
    return 0;
  }
  
  if (!strlen(str) || (str == me->query_name()) || 
    (me->expand_nickname(str) == me->query_name()))
	{
    tell_object(me, me->health_string(1) + ".\n");
    return 1;
  }

  str = me->expand_nickname(str);

  // the flag is to filter hidden and invisible objects
  obs = find_match(str, environment(me), 1);

  if (!sizeof(obs))
  {
    notify_fail(_LANG_CMD_CONDITION_OPTIONS);
    return 0;
  }

  ret = "";

  for (i = 0; i < sizeof(obs); i++)
	{
	  if (obs[i]->query_dead())
      ret += sprintf("%17s: %s\n",
             capitalize(obs[i]->query_name()), _LANG_CMD_CONDITION_IS_DEAD);
    else if (function_exists("health_string", obs[i]) && 
            (obs[i]->query_player() || obs[i]->query_npc() ) )
      ret += sprintf("%17s: %s\n",
             capitalize(obs[i]->query_name()), capitalize(obs[i]->health_string(0)) + ".");
  }

  if (ret == "")
  {
    notify_fail(_LANG_CMD_CONDITION_OPTIONS);
    return 0;
  }
  else
    ret = _LANG_CMD_CONDITION_HEADER + ret;

  tell_object(me, ret);
  
  me->set_trivial_action();
  return 1;
}

