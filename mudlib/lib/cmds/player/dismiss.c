#include <mud/cmd.h>
#include <language.h>

inherit CMD_BASE;

void setup()
{
  set_aliases(_LANG_CMD_DISMISS_ALIAS);
  set_usage(_LANG_CMD_DISMISS_SYNTAX);
  set_help(_LANG_CMD_DISMISS_HELP);
}

/**
 * dismiss: sends away the mount that is out, wherever it is; it is stored.
 */
static int cmd(string str, object me, string verb)
{
  if ((string)handler("riding", me)->dismiss(me) == "none")
    tell_object(me, _LANG_CMD_DISMISS_NONE);

  return 1;
}
