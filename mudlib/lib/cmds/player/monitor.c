#include <standard.h>
#include <cmd.h>


inherit CMD_BASE;

void setup()
{
        position = 0;
}

protected int cmd(string str, object me, string verb) 
{
	if ((str == "si") || (str == "sí"))
	{
		tell_object(me, "Ok, tu monitor de puntos de vida ahora está activado.\n");
		me->set_monitor(1);
	}
	else if (str == "no")
	{
		tell_object(me, "Ok, tu monitor de puntos de vida ahora está desactivado.\n");
		me->set_monitor(0);
	}
	else 
	{
		if (me->query_monitor())
			tell_object(me, "Tu monitor de puntos de vida está activado.\n");
		else
			tell_object(me, "Tu monitor de puntos de vida está desactivado.\n"); 

		tell_object(me, "   Utiliza 'monitor { sí | no }' para cambiarlo\n");
	}
		
	me->set_trivial_action();
	return 1;
}

