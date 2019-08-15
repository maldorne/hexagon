#include <standard.h>
#include <cmd.h>


inherit CMD_BASE;

void setup()
{
        position = 0;
}

protected int cmd(string str, object me, string verb) 
{
    if (str == "abreviado" || str == "corto" || str == "breve") 
      {
      tell_object(me, "Ok, estás en modo abreviado.\n");
      me->set_verbose(0);
      }
    else if (str == "detallado" || str == "largo" || str == "extendido")  
      {
      tell_object(me, "Ok, estás en modo detallado.\n");
      me->set_verbose(1);
      }
    else if (str) {
	notify_fail("Sintaxis: modo [detallado|abreviado]\n"+
                    "          (Sin parámetros para ver el estado actual).\n"); 	
        return 0;
    }
    
    if (!str && me->query_verbose())
      tell_object(me,"Estás en modo detallado.\n");
    else if(!str)
      tell_object(me, "Estás en modo abreviado.\n");

    me->set_trivial_action();
    return 1;
}

