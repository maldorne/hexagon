#include <mud/cmd.h>
#include <mud/mudos.h>
#include <common/properties.h>
#include <translations/common.h>
#include <language.h>

inherit CMD_BASE;

#define STOPPING_FIGHTS_PROP "stopping fights"
#define SECONDS_TO_STOP 12

void setup()
{
  set_aliases(_LANG_CMD_STOP_ALIAS);
  set_usage(_LANG_CMD_STOP_SYNTAX);
  set_help(_LANG_CMD_STOP_HELP);
}

int stop_following(object me)
{
  me->add_static_property(STOP_FOLLOWING_PROP, 1);
  // the message is shown by the follow skill
  // tell_object(me, "Ok, you stop following...\n");
  return 1;
}

int really_stop(object me) 
{
  int i;
  object * obs;

  // if (!str) 
  // {
  obs = me->query_attacker_list() + me->query_call_outed();
  
  if (!sizeof(obs)) 
  {
    notify_fail(_LANG_CMD_STOP_NOT_FIGHTING);
    return 0;
  }
  
  for (i = 0; i < sizeof(obs); i++)
    me->stop_fight(obs[i], 1); // 1 = silent mode

  me->user()->add_notification("player", _LANG_CMD_STOP_MSG);
  return 1;

  /*
  }

  obs = find_match(str, environment(me));
  if (find_living(str))
   obs += ({ find_living(str) });

  if (!sizeof(obs)) 
  {
    notify_fail("Cannot find "+str+"\n");
    return 0;
  }

  for (i=0; i < sizeof(obs); i++)
    me->stop_fight(obs[i]);

  // write("Ok, stopped hunting "+str+"\n");
  tell_object(me, "Ok, you stop following "+str+"\n");
  return 1;
  */
}

static int cmd(string str, object me, string verb)
{
  object * obs;

  if (!strlen(str))
  {
    write(_LANG_SYNTAX + ": " +_LANG_CMD_STOP_SYNTAX + "\n");
    return 1;
  }

  if (member_array(str, _LANG_CMD_STOP_FOLLOW_OPTIONS) != -1)
  {
    if (!me->query_property(FOLLOWING_PROP))
    {
      notify_fail(_LANG_CMD_STOP_NOT_FOLLOWING);
      return 0;
    }
    
    return stop_following(me);
  }
  else if (member_array(str, _LANG_CMD_STOP_FIGHT_OPTIONS) != -1)
  {
    if (me->query_timed_property(STOPPING_FIGHTS_PROP))
    {
      notify_fail(_LANG_CMD_STOP_ALREADY_STOPPING);
      return 0;
    }

    obs = me->query_attacker_list() + me->query_call_outed();

    if (!sizeof(obs))
    {
      notify_fail(_LANG_CMD_STOP_NOT_FIGHTING);
      return 0;
    }

    tell_object(me, _LANG_CMD_STOP_FIGHTS_OK);
    me->add_timed_property(STOPPING_FIGHTS_PROP, 1, SECONDS_TO_STOP);
    call_out("really_stop", SECONDS_TO_STOP * HEART_BEAT_TIME, me);
    return 1;  
  }
  
  write(_LANG_SYNTAX + ": " +_LANG_CMD_STOP_SYNTAX + "\n");
  return 1;
}
