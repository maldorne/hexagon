
#include <mud/cmd.h>
#include <language.h>

inherit CMD_BASE;

void setup()
{
  set_aliases(_LANG_CMD_PROMPT_ALIAS);
  set_usage(_LANG_PROMPT_SYNTAX);
  // set_help(_LANG_PROMPT_HELP);
}

string query_help()
{
  // local variables to use _LANG_PROMPT_HELP
  object me, user;
  me = this_player();
  user = me->user();

  return _LANG_PROMPT_HELP;
}

static int cmd(string str, object me, string verb) 
{
  string ret;
  object user;

  user = me->user();

  if (!strlen(str))
  {
    ret = _LANG_PROMPT_CURRENT;
    ret += _LANG_PROMPT_SYNTAX;

    tell_object(me, ret);
  }
  else if (str == _LANG_PROMPT_CLEAN)
  {
    user->set_prompt("");
    tell_object(me, _LANG_PROMPT_OK);
  }
  else
  {
    user->set_prompt(str);
    tell_object(me, _LANG_PROMPT_OK);
  }

  me->set_trivial_action();
  return 1;
}

