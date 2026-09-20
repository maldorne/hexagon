// free-title - a title written by hand, for a coder
//
// It takes precedence over any earned title while it is set, and clearing it
// brings back whatever the coder was wearing.

#include <mud/cmd.h>
#include <language.h>

inherit CMD_BASE;

void setup()
{
  set_aliases(_LANG_CMD_FREE_TITLE_ALIASES);
  set_usage(_LANG_CMD_FREE_TITLE_SYNTAX);
  set_help(_LANG_CMD_FREE_TITLE_HELP);
}

static int cmd(string str, object me, string verb)
{
  if (!str || !strlen(str))
  {
    tell_object(me, strlen(me->query_manual_title()) ?
      _LANG_CMD_FREE_TITLE_SHOW : _LANG_CMD_FREE_TITLE_NONE);
    tell_object(me, _LANG_CMD_FREE_TITLE_SYNTAX + "\n");
    return 1;
  }

  if (str == "-c")
  {
    me->set_title("");
    tell_object(me, _LANG_CMD_FREE_TITLE_CLEARED);
    return 1;
  }

  me->set_title(str);
  tell_object(me, _LANG_CMD_FREE_TITLE_SET);
  return 1;
}
