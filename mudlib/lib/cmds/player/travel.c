
// travel, externalize from the mounts (/std/ride.c)
// to avoid some possible movement errors, now
// the mount moves after the player (with a call_out of
// zero time) if the action is in the mount, and the mounts moves 
// _after_ the player, the player can be in a different room they 
// try to execute the action. Externalized to avoid it, 
// neverbot 05/09

#include <mud/cmd.h>
#include <language.h>

inherit CMD_BASE;

void setup()
{
  set_aliases(_LANG_CMD_TRAVEL_ALIAS);
  set_usage(_LANG_CMD_TRAVEL_SYNTAX);
  set_help(_LANG_CMD_TRAVEL_HELP);
}

static int cmd (string arg, object me, string verb)
{
	if (!me->query_riding())
	{
		notify_fail(_LANG_CMD_TRAVEL_NO_MOUNT);
		return 0;
	}

  if (!strlen(arg))
  {
    if (me->query_traveling())
    {
      me->stop_travel();
      return 1;
    }
    else
    {
      notify_fail(_LANG_CMD_TRAVEL_WHERE);
      return 0;
    }
  }
    
  me->start_travel(arg);
	return 1;
}
