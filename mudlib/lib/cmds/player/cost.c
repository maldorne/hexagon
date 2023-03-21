#include <mud/cmd.h>
#include <language.h>

inherit CMD_BASE;

void setup()
{
  set_aliases(_LANG_CMD_COST_ALIAS);
  set_usage(_LANG_CMD_COST_SYNTAX);
  set_help(_LANG_CMD_COST_HELP);
}

static int cmd(string str, object me, string verb) 
{
  string ret;
  object ob;

  ret = _LANG_CMD_COST_INTRO;

  ob = load_object(me->query_class_ob());
  if (ob) 
    ret += _LANG_CMD_COST_CLASS;

  ob = load_object(me->query_guild_ob());
  if (ob) 
    ret += _LANG_CMD_COST_GUILD;

  ob = load_object(me->query_job_ob());
  if (ob) 
    ret += _LANG_CMD_COST_JOB;

  tell_object(me, ret);
  
  me->set_trivial_action();
  return 1;
}
