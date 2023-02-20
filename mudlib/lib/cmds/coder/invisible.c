// Comandos extraidos de global, Folken 4/2003
// Los lords tienen su propia action 'invisibilidad', para realizar
//  las comprobaciones de seguridad antes de pasar a invis 2.
// Folken 23/4/2003, cambiado el funcionamiento general del comando,
//  ahora deberia contemplar todas las posibilidades.
#include <standard.h>
#include <cmd.h>

inherit CMD_BASE;

protected int cmd(string str, object me, string verb)
{
  // Añadimos el mensaje de error de Sintaxis, Folken
  if (str && (str != "1")){
  	if (!me->query_administrator())
  	   notify_fail("Sintaxis: invisible.\n");
  	else
  	   notify_fail("Sintaxis: invisible.\n"+
  	               "          (admite el parametro opcional '1'\n"+
  	               "           para abandonar el estado de invisibilidad 2).\n");
  	return 0;
  }
	
  switch(me->query_invis()){
    /*
    case 2:
      // Flode, 120997 - Added the possibility to go from invis 2
      // to invis 1 the hard way
      if(str && str == "1"){
      	    tell_object(me, "Te haces un poco más visible.\n");
	    me->set_invis(1);
	    return 1;
	}
      notify_fail("Ya eres realmente invisible.\n");
      return 0;
    */
    case 1:
      if (!me->query_administrator()){
	notify_fail("Con tus actuales poderes no puedes hacerte más invisible de lo que ya eres.\n");
	return 0;
      }
      /*
      if(!str || (str == "")){
        tell_object(me, "Ahora eres realmente invisible.\n");
        me->set_invis(2);
        return 1;
      }
      */
      return 1;

    default:
    case 0:
	    tell_object(me, "Desapareces.\n");
	    tell_room(environment(me), me->query_cap_name()+" desaparece repentinamente.\n", me);
	    me->set_invis(1);
	    return 1;
  }
} /* invisible() */
