// Extracted from player.c to reduce complexity, neverbot 4/2003
// Here we perform all the necessary checks before removing the
// player, and delete the details that do not require security (the
// rest is done from the try_retire of /secure/master

#include <mud/cmd.h>
#include <mud/secure.h>
#include <user/player.h>
#include <language.h>

inherit CMD_BASE;

void setup()
{
  set_aliases(_LANG_CMD_RETIRE_ALIAS);
  set_usage(_LANG_CMD_RETIRE_SYNTAX);
  set_help(_LANG_CMD_RETIRE_HELP);
}

static int cmd(string str, object me, string verb) 
{ 
  // Added by neverbot, I can't believe this wasn't set :P
  if (me->query_property(GUEST_PROP))
  {
    notify_fail(_LANG_CMD_RETIRE_GUEST);
    return 0;
  }

  // should probably do a more secure check, but the retire has a
  // password.
  return SECURE->try_retire(me);
}

