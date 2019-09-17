#include <standard.h>
#include <cmd.h>
#include <properties.h>

inherit CMD_BASE;

void setup() 
{
        position = 0;
}

int stop_following(object me)
{
	me->add_static_property(STOP_FOLLOWING_PROP, 1);
	// El mensaje ya lo da la dote de seguir
	// tell_object(me, "Ok, detienes tu persecución...\n");
	return 1;
}

int really_stop(object me) 
{
    int i;
    object *obs;

    // if (!str) 
    // {
	obs = me->query_attacker_list() + me->query_call_outed();
	if (!sizeof(obs)) 
	{
	    notify_fail("No estás luchando con nadie.\n");
	    return 0;
	}
	
	for (i = 0; i < sizeof(obs); i++)
	    me->stop_fight(obs[i], 1); // 1 = silent mode

	tell_player(me, "Has dejado de luchar con " 
                     + query_multiple_short(obs)+".\n");
 	return 1;
    /*
    }

    obs = find_match(str, environment(me));
    if (find_living(str))
	obs += ({ find_living(str) });

    if (!sizeof(obs)) 
    {
	notify_fail("No puedo encontrar a "+str+"\n");
	return 0;
    }
    for (i=0; i < sizeof(obs); i++)
	me->stop_fight(obs[i]);
    // write("Ok, stopped hunting "+str+"\n");
    tell_object(me, "Ok, dejamos de perseguir a "+str+"\n");
    return 1;
    */
}

protected int cmd(string str, object me, string verb){

    object * obs = me->query_attacker_list() + me->query_call_outed();

    if ( !str || (str == "")) 
	{
      notify_fail("Sintaxis: detener combates\n"
                  "          (detendrás tus combates pendientes).\n" 
				  "          detener persecución\n"
   	              "          (dejarás de seguir a tus objetivos).\n" ); 
      return 0;
    }

	if ((str == "persecución") || (str == "persecucion"))
	{
		if (!me->query_property(FOLLOWING_PROP))
		{
			notify_fail("No estás siguiendo a nadie en estos momentos.\n");
	      	return 0;
		}
		
		return stop_following(me);
	}
	else if (str == "combates")
	{
	    if (!sizeof(obs))
		{
	      notify_fail("No estás combatiendo con nadie en estos momentos.\n");
	      return 0;
	    }

	    tell_object(me, "Parando peleas, tardará un poco...\n");
	    call_out("really_stop", 25, me);
	    return 1;		
	}
	
    notify_fail("Sintaxis: detener combates\n"
                "          (detendrás tus combates pendientes).\n" 
		        "          detener persecución\n"
 	            "          (dejarás de seguir a tus objetivos).\n" ); 
    return 0;	

}

