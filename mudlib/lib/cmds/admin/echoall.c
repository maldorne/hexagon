#include <standard.h>
#include <cmd.h>
inherit CMD_BASE;
  
void setup()
{
	position = 1;
}
 
string query_short_help()
{
	return "Envía un mensaje anónimo a todos los usuarios.";
}

string query_usage()
{
	return "command";
}

protected int cmd (string str,object me, string verb)
{
/*	efun::tell_object(me, (string)me->query_short()+
 *		(me->query_gtitle()? " "+me->query_gtitle() : "")+
 *		(me->query_title()? " "+me->query_title() : "")+
 *		".\n");
 */		
    object *players;
    int i;
    
    if(!str)
    {
        notify_fail("Tienes que poner un mensaje!!!\n");
        return 0;
    }
    players=users();
    for (i=0; i<sizeof(players); i++)
    {
         tell_object(players[i], "\n"+str+".\n");
    }
    return 1;

}
