#include <mud/cmd.h>
#include <language.h>

inherit CMD_BASE;

void setup()
{
  set_aliases(({ "uptime" }));
  set_usage("uptime");
  set_help("Shows for how long the mud server has been functioning.");
}

static int cmd(string str, object me, string verb)
{
  tell_object(me, _LANG_CMD_UPTIME_MESSAGE);
  return 1;
}
