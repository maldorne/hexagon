/*
 * jump: from a bridge cell into the water below (see /lib/room/bridge.c).
 */

#include <mud/cmd.h>
#include <language.h>

inherit CMD_BASE;

void setup()
{
  set_aliases(_LANG_CMD_JUMP_ALIAS);
  set_usage(_LANG_CMD_JUMP_SYNTAX);
  set_help(_LANG_CMD_JUMP_HELP);
}

static int cmd(string str, object me, string verb)
{
  object env;

  if (me->query_dead())
  {
    notify_fail(_LANG_CMD_JUMP_DEAD);
    return 0;
  }

  env = environment(me);
  if (!env || !env->query_bridge())
  {
    notify_fail(_LANG_CMD_JUMP_NOWHERE);
    return 0;
  }

  if (str && strlen(trim(str)) &&
      member_array(lower_case(trim(str)), _LANG_CMD_JUMP_TARGETS) == -1)
  {
    notify_fail(_LANG_CMD_JUMP_WHERE);
    return 0;
  }

  env->jump_into_water(me);
  return 1;
}
