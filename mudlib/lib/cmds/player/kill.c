
/* 
 * kill command from player.c externalized by Baldrick.
 */

#include <mud/cmd.h>

inherit CMD_BASE;

void setup()
{
  position = 0;
}

string query_usage()
{
  return "atacar <objetivos>";
}

string query_short_help()
{
  return "Comienza a atacar a los objetivos especificados.";
}

static int cmd (string str, object me, string verb)
{
  int i;
  object *obs;

  // Almacenamos con quienes hemos comenzado combates:
  // list[string "singular"] = ({ int cuantos, string "plural", })
  mapping list;
  string *res;
  mixed mess;

  list = ([ ]);
  res = ({ });

  // si esta haciendo otra cosa no puede iniciar combates
  mess = me->query_static_property("unable_to_combat");

  if (mess)
  {
    notify_fail(mess);
    return 0;
  }

  if (!strlen(str))
  {
    notify_fail("Sintaxis: atacar <objetivos>\n");
    return 0;
  }

  str = lower_case(str);

  if (str == "todo" || str[0..0] == "0")
  {
    notify_fail("No está permitido.\n");
    return 0;
  }

  // Fix by Wonderflug.  Ghosts shouldn't attack :)
  if ( me->query_dead() )
  {
    notify_fail("Tu estado etéreo tiene poco efecto en el mundo material.\n ");
    return 0;
  }

  obs = find_match(me->expand_nickname(str), environment(me));
  //obs = find_living(str, environment());
  
  if (!sizeof(obs) || obs[0]->query_hidden() == 1)
  {
    notify_fail("No consigues encontrar a "+me->expand_nickname(str)+".\n");
    return 0;
  }

  /* Notice that there is no check for living().  One can attack anything.
   * Should have..
   */

  if (obs[0] == me)
  {
    notify_fail("¿Te quieres suicidar?\n");
    return 0;
  }

  if (!obs[0]->query_alive())
  {
    notify_fail("Comienzas a golpear a "+str+", pero "+str+" no "+
      "parece darse cuenta.\n");
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
      log_file("link", "["+ctime(time())+"] "+me->query_cap_name()+" atacó a "+
        obs[i]->query_cap_name()+" (respondia a query_statue).\n");
    
    if (interactive(obs[i]))
    {
      log_file("attacks", "["+ctime(time(), 4)+"] " + me->query_name() + 
        " with level: " + me->query_level() + 
        " attacked " + obs[i]->query_name() + 
        " with level: " + obs[i]->query_level() +"\n");      
      // log_attack(me->query_name(), me->query_level(), obs[i]->query_name(),
      //   obs[i]->query_level());
    }
  }

  for( i = 0; i < sizeof(keys(list)); i ++)
  {
    if (list[keys(list)[i]][0]!=1)
    {
      res += ({ query_num(list[keys(list)[i]][0], 5)+" "+list[keys(list)[i]][1] });
    }
    else
    {
      res += ({ keys(list)[i] });
    }
  } 

  // write("Ok. Comienzas a pelear con "+str+".\n");
  write("Ok. Comienzas a pelear con "+query_multiple_short(res, 0)+".\n");
  return 1;
}
