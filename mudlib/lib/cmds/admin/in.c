// In <domain> del antiguo domain_communicator. 
// Hecho comando por Folken 2001

#include <standard.h>
#include <cmd.h>
inherit CMD_BASE;


protected int cmd(string str, object me, string verb) 
{
    string *doms;
    object *ob;
    int i;
    string fname;
    int yep = 0;
    string *bass;

    if( !this_player()->query_coordinator() )
    {
       tell_object(this_player(),"Solo Thanes y superiores pueden utilizar este comando.\n");
       return 1;
    }

    ob = users();
    for (i = 0; i<sizeof(ob); i++) 
    {
      fname = file_name(environment(ob[i]));
      bass = explode (fname, "/");
      doms = get_dir("/d/");
      if (member_array(str, doms) == -1) { return 0; }
      if (bass[1] == str) 
	    {
		yep = 1;
		tell_object(this_player(),ob[i]->query_cap_name()+ " en "+
	                                  environment(ob[i])->query_short()+ "   ("+
		                          file_name(environment(ob[i]))+ ")\n");
	    }
    }
    if (yep == 0) 
    {
	tell_object(this_player(),"No hay nadie en el dominio '"+str+"' en este momento.\n");
	return 1;
    }
    return 1;
}