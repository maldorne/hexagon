
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

  // The command hash is built at boot, and this_player() is the mud's own
  // handler then rather than nothing at all -- so being non-nil is not enough,
  // it has to be a player before the help text can quote a name or a hit point
  // count. Outside a player's context it falls back to the shortcut names.
  me = this_player();
  if (me && !me->query_player())
    me = nil;
  user = me ? me->user() : nil;

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

