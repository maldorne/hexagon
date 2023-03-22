
#include <mud/cmd.h>
#include <translations/common.h>
#include <language.h>

inherit CMD_BASE;

void setup()
{
  set_aliases(_LANG_CMD_PAST_ALIAS);
  set_usage(_LANG_CMD_PAST_SYNTAX);
  set_help(_LANG_CMD_PAST_HELP);
}

static int cmd(string name, object me, string verb)
{
  me->print_past_g();
  me->set_trivial_action();
  return 1;
}
