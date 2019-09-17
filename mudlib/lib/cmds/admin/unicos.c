#include <standard.h>
#include <cmd.h>

#include <unique_objects.h>

inherit CMD_BASE;

void setup()
{
        position = 0;
}

protected int cmd(string str, object me, string verb) 
{
    int i;
    string ret;
    string *names;
    mixed *data;
    mapping obs = HANDLER->query_objects();
    
    names = keys(obs);
    
    ret =  "-------------------------------------------------------\n";
    ret += "  Lista de objetos únicos del juego:\n";
    ret += "-------------------------------------------------------\n";
    
    for (i = 0; i < sizeof(names); i++){
    	data = obs[names[i]];
    	ret += "    %^BOLD%^"+names[i]+"%^RESET%^ está en:\n";
    	ret += "    "+((data[0]==UNASIGNED)?("%^BOLD%^RED%^sin asignar%^RESET%^"):(data[1]))+" "+
                      ((data[0]==VAULT)    ?("(%^BOLD%^RED%^baul%^RESET%^)"):"") + "\n";
    	ret += "    desde: "+ctime(data[2],4)+"\n";
        ret += "-------------------------------------------------------\n";
    }

    tell_object(me, ret);    
    me->set_trivial_action();
    return 1;
}

