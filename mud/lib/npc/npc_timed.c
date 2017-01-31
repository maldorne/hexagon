
/* 
 * Codigo extraido de monster.c, Folken 04/2009
 *
 * Incluido nuevo sistema para npcs controlados segun el tiempo del
 *  weather.c (por la noche desaparecen, por ejemplo).
 * set_only_one ->
 *  solo se clonara si no existe ningun otro npc igual en el mud
 */

#include <areas/weather.h>

static int is_timed,   /* el npc funciona mediante el tiempo */
    is_night_timed,    /* el npc funciona mediante tiempo pero por la noche
					      (su hora de inicio es mayor que la de fin!!!) */
    init_hour,         /* hora a partir de la cual debe clonarse */
    end_hour;          /* hora a la que debe desaparecer */

static string end_hour_mess; /* mensaje que dará al desaparecer
								query_cap_name() + " " + end_hour_mess + .\n"
								No hay mensaje de hora de inicio... si la hora de
								inicio no concuerda el npc no se clona!! */
static string * end_hour_comms;
							 /* Lista de comandos a realizar cuando tenga que irse
								(si la lista no es vacia, el npc no se destruye, solo
								hace los comandos) */

void create()
{
	// Npcs con control horario, Folken 7/03
	is_timed = 0;
	is_night_timed = 0;
	init_hour = 0;
	end_hour = 0;
	end_hour_mess = "abandona el lugar";
	end_hour_comms = ({ });
	
	// Importante: para añadir el npc al control horario del weather.c, 
	// ya tiene que haberse hecho el setup,
	// por lo que el resto del create relativo a esto se realiza 
	// al final del create() de /std/monster.c
}

int query_timed_npc(){ return (is_timed || is_night_timed); }

// Nuevo sistema de control de npcs segun el weather.c, Folken 7/03
void event_weather(object who, varargs int flag, int * values)
{
	int time, i;
	object env;

	if ((!is_timed) && (!is_night_timed))
	    return;

	// estos flags informan de los cambios horarios, los demas no interesan
	if ((flag != FLAG_CHECK) &&
	    (flag != FLAG_AMANECER) &&
	    (flag != FLAG_ANOCHECER) )
	return;

	time = WEATHER->query_date_data()[0];

	if (is_timed && (time > end_hour))
	{
		if(!sizeof(this_object()->query_attacker_list()))
		{
			if ((env = environment(this_object())) && (end_hour_mess != ""))
			    tell_room(env, this_object()->query_cap_name() + " " + end_hour_mess + ".\n", ({ this_object() }) );
			if (!sizeof(end_hour_comms))
			    this_object()->dest_me();
			else
			{
				for(i = 0; i < sizeof(end_hour_comms); i++)
				{
					this_object()->queue_action(end_hour_comms[i]);
				}
				is_timed = 0; // No repetimos mas veces este proceso
				WEATHER->unnotify_me(this_object());
			}
			return;
		}
	}

	if (is_night_timed && (time > end_hour) && (time < init_hour) )
	{
		if(!sizeof(this_object()->query_attacker_list()))
		{
			if ((env = environment(this_object())) && (end_hour_mess != ""))
			    tell_room(env, this_object()->query_cap_name() + " " + end_hour_mess + ".\n", ({ this_object() }) );
			if (!sizeof(end_hour_comms))
			    this_object()->dest_me();
			else
			{
				for(i = 0; i < sizeof(end_hour_comms); i++)
				{
					this_object()->queue_action(end_hour_comms[i]);
				}
				is_night_timed = 0; // No repetimos mas veces este proceso
				WEATHER->unnotify_me(this_object());
			}
			return;
		}
	}
	return;
}

private void set_night_time_range(int start, int end);
// Si no queremos que de ningun mensaje, debemos hacer un set_time_mess("");
void set_time_mess(string str) { end_hour_mess = str; }
int * query_time_range() { return ({ init_hour, end_hour }); }
void set_time_range(int start, int end, varargs string * comms)
{
	// int aux;
	// No pueden ser iguales
	if (start == end)
	 return;

	// Pequeño seguro... dejamos minimos y maximos
	if (start < 0) start = 0;
	if (end < 1) end = 1;
	if (start > 22) start = 22;
	if (end > 23) end = 23;

	// Lista de comandos a hacer cuando tenga que desaparecer
	if (comms && (sizeof(comms) > 0))
  {
		end_hour_comms = comms;
	}

	// start debe ser el menor
	if (start > end)
  {
		/* Cambiado, si start es mayor, hacemos un set_night_time_range
		aux = start;
		start = end;
		end = aux;
		*/
		set_night_time_range(start, end);
	}

	is_night_timed = 0;
	is_timed = 1;
	init_hour = start;
	end_hour = end;
	// Solo lo hacemos con los objetos clonados
	if (strsrch(file_name(this_object()), "#") != -1)
	WEATHER->notify_me(this_object());
}

// Ummm... el metodo anterior no nos sirve si queremos clonar npcs nocturnos
//  (la hora de inicio es mayor que la de final), ahora mismo no tengo ganas
//  de pensar un metodo mejor, Folken 1/7/03
private void set_night_time_range(int start, int end)
{
	int aux;
	// No pueden ser iguales
	if (start == end)
	 return;

	// Pequeño seguro... dejamos minimos y maximos
	if (start < 1) start = 1;
	if (end < 0) end = 0;
	if (start > 23) start = 23;
	if (end > 22) end = 22;

	// start debe ser el mayor
	if (start < end)
	{
		aux = start;
		start = end;
		end = aux;
	}

	is_timed = 0;
	is_night_timed = 1;
	init_hour = start;
	end_hour = end;
	// Solo lo hacemos con los objetos clonados
	if (strsrch(file_name(this_object()), "#") != -1)
	    WEATHER->notify_me(this_object());
}

mixed *stats() 
{
	return ({
		({ "Is timed (nosave)", is_timed }),
		({ "Is night timed (nosave)", is_night_timed }),
		({ "Init hour", init_hour }),
		({ "End hour", end_hour }),
		({ "End hour mess", end_hour_mess }),
		({ "End hour comms", end_hour_comms }),
	        });
}
