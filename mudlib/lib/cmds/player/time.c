
#include <mud/cmd.h>
#include <language.h>

inherit CMD_BASE;

void setup()
{
  set_aliases(_LANG_CMD_TIME_ALIAS);
  set_usage(_LANG_CMD_TIME_SYNTAX);
  set_help(_LANG_CMD_TIME_HELP);
}

static int cmd(string str, object me, string verb)
{
  object weather;
  string out;

  out = _LANG_CMD_TIME_REAL + ctime(time(), 1) + "\n";

  // The clock of the game the player is standing in, not this file's: a command
  // under /lib/cmds/ belongs to no game, so resolving the handler from itself
  // would answer with the lib one, which nothing advances.
  weather = handler("weather", me);
  if (weather)
    out += _LANG_CMD_TIME_GAME + (string)weather->date_string() + "\n";

  write(out);
  return 1;
}
