
// basic help cmd, from the player_role, neverbot 09/2019

#include <mud/cmd.h>
#include <language.h>

inherit CMD_BASE;

void setup()
{
  position = 0;
}

string query_help()
{
  return _LANG_CMD_HELP + "\n\n" + _LANG_CMD_HELP_HELP;
}

static int cmd(string arg, object me, string verb)
{
  return me->user()->do_help(arg);
}
