#include <standard.h>
#include <cmd.h>

inherit CMD_BASE;

void setup() 
{
        position = 0;
}

protected int cmd(string str, object me, string verb)
{
    object *obs = me->query_attacker_list() + me->query_call_outed();

	if (!sizeof(obs)) 
	{
	    notify_fail("No estás luchando con nadie.\n");
	    return 0;
	}
	
	tell_object(me, "Estás luchando con " + query_multiple_short(obs)+".\n");

    return 1;
}

