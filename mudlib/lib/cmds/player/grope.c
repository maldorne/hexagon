// Grope (around in the dark)
// For blind players to find their way.
// Quark, April 97
// Translated by neverbot@RL

#include <mud/cmd.h>
#include <common/properties.h>

inherit CMD_BASE;

void setup()
{
	position = 0;
}

string query_usage()
{
	return "palpar";
}

string query_short_help()
{
	return "Para buscar alguna salida a tu alrededor cuando no puedas ver.";
}

static int cmd(string arg, object me, string verb)
{
	mixed *exits, exit;
	object *contents;
	int i,j, dark;

	i = random(10);
	dark = me->check_dark((int)environment(me)->query_light());

	if(!me->query_property(BLIND_PROP) && !dark) 
	{
		notify_fail("Palpar los alrededores únicamente te servirá cuando estés ciego o esté muy oscuro.\n");
		return 0;
	}   

	if(me->query_property(BUSCAR_PROP))
	{
		notify_fail("Debes esperar un rato antes de volver a palpar de nuevo.\n");

		return 0;
	} 
	
	me->add_timed_property(BUSCAR_PROP,1,1);

	if(i<3)
	{
		contents = all_inventory(environment(me)); 
		j = random(sizeof(contents));
		
		if (contents[j] == me)
		{
			tell_object(me, "Buscas a tu alrededor pero no encuentras nada.\n");
			return 1;
		}
		
		if(interactive(contents[j]) )
		{
			tell_object(me, "¡Te encuentras en medio a "+
				contents[j]->query_cap_name()+"!\n");
			tell_object(contents[j],"¡"+me->query_cap_name()+" choca contigo.\n");

			tell_room(environment(me),"¡"+me->query_cap_name()+" choca con "
				+contents[j]->query_cap_name()+".\n",({ me, contents[j]}));
		}
		else
			if(contents[j]->query_npc())
		{
			tell_object(me, "¡Parece que te has chocado con"+
				" "+contents[j]->query_cap_name()+"!\n");
			tell_room(environment(me),"¡"+me->query_cap_name()+" se choca con "+
				+contents[j]->query_cap_name()+"!\n",({ me, }));

			// contents[j]->attack_ob(me);
		}
		else // neverbot, 21/02/04
			if(contents[j]->query_door())
			{
				tell_object(me, "Palpas a tu alrededor y encuentras una puerta hacia "+
					+contents[j]->query_dest()+".\n");       	
			}
			else
			{
				tell_object(me,"Te golpeas contra "+contents[j]->query_cap_name()+
					".\n");
				tell_room(environment(me),me->query_cap_name()+" se golpea "+
					"contra "+contents[j]->query_cap_name()+".\n");
			}      
		return 1;
	}
	else
	{
		exits = environment(me)->query_dest_dir();  
		
		if(sizeof(exits))
		{
			exit = exits[(random(sizeof(exits)/2))*2];
			tell_object(me, "Palpas a tu alrededor y encuentras una salida hacia "+
				+exit+".\n");
			return 1;
		}
	}
	notify_fail("No encuentras nada.\n");
	return 0;
}
