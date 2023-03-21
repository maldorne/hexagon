/*
 * bury converted from std/command so it uses less memory, Errante
 * adapted to Cc, neverbot 20/04/2003
 */

#include <mud/cmd.h>
#include <language.h>

inherit CMD_BASE;

void setup()
{
  set_aliases(_LANG_CMD_BURY_ALIAS);
  set_usage(_LANG_CMD_BURY_SYNTAX);
  set_help(_LANG_CMD_BURY_HELP);
}

static int cmd (string str, object me, string verb)
{
  object * target;
  int counter, i;
 
  // if (!strlen(str))
  //   str = "corpse";

  if (me->query_dead())
  {
    tell_object(me, _LANG_CMD_BURY_DEAD);
    return 1;
  }

  if (environment(me) && environment(me)->query_water_environment())
  {
    tell_object(me, _LANG_CMD_BURY_WATER);
    return 1;
  }

  target = all_inventory(environment(me));

  counter = 0;
  for (i = 0 ; i < sizeof(target); i++)
    if (target[i]->query_corpse())
    {
      target[i]->dest_me(); // allow them to clean up themselves
      counter++;
    }

  if (counter)
    tell_object(me, _LANG_CMD_BURY_MSG);
  else
    tell_object(me, _LANG_CMD_BURY_NO);

  return 1;
}

