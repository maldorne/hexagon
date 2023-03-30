
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
  write(ctime(time(), 1) + "\n");

  // write(ctime(time()-3600)+" Islas Canarias.\n");
  // write(ctime(time()-14400)+" América Central.\n");

  // mud time
  // TO DO, return the calendar from the game the character is in
  //  write(ctime(time(), 3) + ".\n");

  return 1;
}
