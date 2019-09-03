// Comandos extraidos de global, Folken 4/2003
#include <standard.h>
#include <cmd.h>

inherit CMD_BASE;

protected int cmd (string str, object me, string verb)
{
    if(!me->query_invis()) {
    	tell_object(me, "Ya eres visible.\n");
	return 1;
    }
    tell_object(me, "Apareces.\n");
    tell_room(environment(me), me->query_cap_name()+" aparece repentinamente de la nada.\n", me);
    me->set_invis(0);
    return 1;
}

