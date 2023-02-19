#include <standard.h>
#include <cmd.h>


inherit CMD_BASE;

void setup()
{
        position = 0;
}

protected int cmd(string str, object me, string verb) 
{
    string ret;
    object * obs;
    int i;

    if (me->query_dead())
    {
        notify_fail("Estando muerto no puedes hacer eso.\n");
        return 0;
    }
    
    if (!str || (str == "") || (str == me->query_name()) || 
        (me->expand_nickname(str) == me->query_name()))
	{
      tell_object(me, me->health_string(1) + ".\n");
      return 1;
    }

    str = me->expand_nickname(str);

    if (str == "todos") str = "todo";

    // El 1 es para filtrar los hidden y los invis
    obs = find_match(str, environment(me), 1);

    if (!sizeof(obs)){
      notify_fail("Sintaxis: 'estado' para comprobar tu estado de salud, o\n"+
                  "          'estado <nombre>' para ver el estado de otro, o\n"+
                  "          'estado todos' para comprobar a todos los de tu entorno.\n");
      return 0;
    }

    ret = "";

    for(i = 0; i < sizeof(obs); i++)
	{
	  if (obs[i]->query_dead())
        ret += sprintf("%17s: %s\n",
                       capitalize(obs[i]->query_name()), "Está muert"+obs[i]->query_vowel()+".");
      else if (/* (obs[i] != me) && */ function_exists("health_string", obs[i]) && 
           (obs[i]->query_player() || obs[i]->query_npc() ) )
        ret += sprintf("%17s: %s\n",
                       capitalize(obs[i]->query_name()), capitalize(obs[i]->health_string(0)) + ".");
    }

    if (ret == ""){
      notify_fail("Sintaxis: 'estado' para comprobar tu estado de salud, o\n"+
                  "          'estado <nombre>' para ver el estado de otro, o\n"+
                  "          'estado todos' para comprobar a todos los de tu entorno.\n");
      return 0;
    }
    else
      ret = "Estado en que se encuentran:\n" + ret;

    tell_object(me, ret);
    
    me->set_trivial_action();
    return 1;
}

