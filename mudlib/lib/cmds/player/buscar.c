// Hecho externo por Folken 6/03
// para vaciar un poco de codigo el room.c

#include <standard.h>
#include <cmd.h>


inherit CMD_BASE;

void setup()
{
	position = 0;
}

int is_hidden(object ob)
{
	return ob->query_hide_shadow();
}

protected int cmd(string str, object me, string verb) 
{
	/* 
	object ob;
	object * hidden_objects;
	object * aux;
	*/
	
	// Fix by wonderflug.  ghosts shouldn't search.
	if ( this_player()->query_dead() )
	{
		notify_fail("Un fantasma no puede hacer eso.\n");
		return 0;
	}
	
	if ( this_player()->query_hide_shadow() )
	{
		notify_fail("Estando escondido no puedes ponerte a buscar.\n");
		return 0;
	}
	
	// A bit of gp to do this -- Wf, oct 95
	if ( this_player()->adjust_gp(-1) < 0 )
	{
		notify_fail("Estás demasiado cansado como para ponerte a buscar ahora.\n");
		return 0;
	}

	if (!environment(me)) 
		return 0;
	
	/*	
	hidden_objects = environment(me)->query_hidden_objects();
	hidden_objects -= ({ nil });
	
	// Añadimos lo que se haya escondido sin informar a la room
	aux = all_inventory(environment(me));
	aux = filter_array(aux, (:is_hidden:));
	hidden_objects += aux;

	if(sizeof(hidden_objects) )
	{
		// Si queremos hacer algo con los objetos que están escondidos, lo podemos hacer aqui,
		// Folken 10/2009
	}
	*/
	
	write( ({
			"Buscas atentamente a tu alrededor, tratando de encontrar algo oculto.\n",
			"Remueves todo a tu alrededor buscando algo interesante.\n",
			"Buscas esforzadamente algo por los alrededores.\n",
			"Examinas toda la zona buscando algo que no hubieses advertido.\n",
			"Cuidadosamente rastreas el suelo buscando algo de interés.\n" })[random(5)]);
	

	tell_room(environment(me), me->query_cap_name()+" busca algo por los alrededores.\n",
		({ me }));
	event(environment(me), "player_search");
	return 1;
}

