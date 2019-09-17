#include <standard.h>
#include <cmd.h>
inherit CMD_BASE;
#define PEOPLER "/obj/handlers/peopler"

void setup()
{
    position=1;
}

protected int cmd(string str, object me, string verb){
    if(!this_player()->query_coordinator()){
	write("No tienes suficiente rango para ejecutar este comando.\n");
	return 1;
    }
    return (int)PEOPLER->do_dirs(str);
}
