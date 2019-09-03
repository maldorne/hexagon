// qatt del antiguo domain_communicator. 
// Hecho comando por Folken 2001

#include <standard.h>
#include <cmd.h>
inherit CMD_BASE;

protected int cmd(string str, object me, string verb) 
{
    object *ob;
    int i;
    int yep = 0;
    object *fig;
    int f;
    
    if( !this_player()->query_coordinator() )
    {
       tell_object(this_player(),"Solo Thanes y superiores pueden usar este comando.\n");
       return 1;
    }
    
    ob = users();
    for (i = 0;i < sizeof(ob);i++)
    {
	if (sizeof(ob[i]->query_attacker_list())) 
	{
	    tell_object(this_player(),"\n"+ ob[i]->query_cap_name()+ " esta en lucha con:\n");
	    fig = ob[i]->query_attacker_list();
	    for (f = 0; f < sizeof(fig); f++) 
	    {
		tell_object(this_player(),"\t"+ fig[f]->query_short()+ "\t("+ file_name(fig[f])+ ")\n");
		yep = 1;
	    }
        }
    }
    if (yep == 0) 
    {
	tell_object(this_player(),"Nadie esta en luchas en este momento.\n");
	return 1;
    }
    return 1;

}