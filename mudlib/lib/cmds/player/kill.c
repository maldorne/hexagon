/* 
 * kill command from player.c externalized by Baldrick.
 */

#include <mud/cmd.h>
#include <language.h>
#include <translations/common.h>
#include <common/properties.h>

inherit CMD_BASE;

void setup()
{
  set_aliases(_LANG_CMD_KILL_ALIAS);
  set_usage(_LANG_CMD_KILL_SYNTAX);
  set_help(_LANG_CMD_KILL_HELP);
}

static int cmd (string str, object me, string verb)
{
  int i;
  object *obs;

  // store targets 
  // list[string single] = ({ int how many, string plural, })
  mapping list;
  string *res;
  mixed mess;

  list = ([ ]);
  res = ({ });

  // cannot attack if doing any other thing
  if (mess = me->query_static_property("unable_to_combat"))
  {
    notify_fail(mess);
    return 0;
  }

  if (environment(me)->query_property(NOKILL_PROP))
  {
    notify_fail(_LANG_CMD_KILL_NO_FIGHT);
    return 0;
  }

  if (!strlen(str))
  {
    write(_LANG_SYNTAX + ": " +_LANG_CMD_KILL_SYNTAX + "\n");
    return 1;
  }

  str = lower_case(str);

  if (member_array(str, _LANG_CMD_KILL_NOT_ALLOWED_ARRAY) != -1)
  {
    notify_fail(_LANG_CMD_KILL_NOT_ALLOWED);
    return 0;
  }

  // Fix by Wonderflug. Ghosts shouldn't attack :)
  if (me->query_dead())
  {
    notify_fail(_LANG_CMD_KILL_NO_NEED_WHEN_DEAD);
    return 0;
  }

  obs = find_match(me->expand_nickname(str), environment(me));
  //obs = find_living(str, environment());
  
  if (!sizeof(obs) || obs[0]->query_hidden() == 1)
  {
    notify_fail(_LANG_CMD_KILL_NOT_FOUND);
    return 0;
  }

  // Notice that there is no check for living(). One can attack anything.
  // Should have...
  if (obs[0] == me)
  {
    notify_fail(_LANG_CMD_KILL_NOT_SUICIDE);
    return 0;
  }

  if (!obs[0]->query_alive())
  {
    notify_fail(_LANG_CMD_KILL_NOT_ALIVE);
    return 0;
  }

  for (i = 0; i < sizeof(obs); i++)
  {
    // Taniwha 1995, make sure the targets are alive
    if (obs[i]->query_alive())
    {
      me->attack_ob(obs[i]);

      // Mostramos lista de con quienes realmente comenzamos a combatir
      // list += ({ obs[i]->query_name() }); 
      if (member_array(obs[i]->query_name(), keys(list)) > -1)
      {
        list[obs[i]->query_name()][0] = list[obs[i]->query_name()][0] + 1;
      }
      else
      {
        list[obs[i]->query_name()] = ({ 1, 
            (sizeof(obs[i]->query_plurals())?obs[i]->query_plurals()[0]:obs[i]->query_main_plural()),
          });
      }
    }

    if (obs[i]->query_statue())
      log_file("link", "["+ctime(time())+"] "+me->query_cap_name()+" attacked "+
        obs[i]->query_cap_name()+" (query_statue were true).\n");
    
    if (interactive(obs[i]))
    {
      log_file("attacks", "["+ctime(time(), 4)+"] " + me->query_name() + 
        " with level: " + me->query_level() + 
        " attacked " + obs[i]->query_name() + 
        " with level: " + obs[i]->query_level() +"\n");      
    }
  }

  for (i = 0; i < sizeof(keys(list)); i++)
  {
    if (list[keys(list)[i]][0]!=1)
      res += ({ query_num(list[keys(list)[i]][0], 5)+" "+list[keys(list)[i]][1] });
    else
      res += ({ keys(list)[i] });
  } 

  write(_LANG_CMD_KILL_START);
  return 1;
}
