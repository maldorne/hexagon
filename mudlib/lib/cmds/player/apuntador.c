#include <standard.h>
#include <cmd.h>
#include <hud.h>

inherit CMD_BASE;

string query_short_help()
{
	return "Cambia la forma en que se perciben el resto de personajes del entorno.";
}

protected int cmd(string str, object me, string verb) 
{
    if (!str || (member_array(str, HUD_TYPES) == -1))
	{
		tell_object(me, "Sintaxis: apuntador { "+implode( HUD_TYPES, " | ")+" }\n" + 
						"    Tu apuntador actual es: "+me->query_hud()+"\n");
		return 1;
	}
	
	me->set_hud(str);
	tell_object(me, "Ok, tipo de apuntador cambiado a '"+str+"'.\n");

	me->set_trivial_action();
    return 1;
}

