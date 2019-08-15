// cabalgar, hecho externo desde las monturas (/std/ride.c)
// Para evitar algunos posibles errores de movimiento, ahora 
// la montura se mueve despues del player (con un call_out de 
// tiempo cero), y la diferencia de tiempo es suficiente para que,
// cuando el jugador quiere parar su viaje escribiendo cabalgar,
// la montura pueda no estar aun en su room actual, y el comando
// haga un return cero.
// Hecho externo para evitarlo, Folken 05/09

#include <standard.h>
#include <cmd.h>

inherit CMD_BASE;

void setup() { position = 0; }

protected int cmd (string arg, object me, string verb)
{
	if (!me->query_riding())
	{
		notify_fail("Sólo funciona si estás sobre una montura.\n");
		return 0;
	}

    if (!arg || arg == "")
    {
        if (me->query_traveling())
        {
            me->stop_travel();
            return 1;
        }
        else
        {
            notify_fail("¿"+capitalize(verb)+" hacia dónde?\n");
            return 0;
        }
    }
    
    me->start_travel(arg);
	return 1;
}
