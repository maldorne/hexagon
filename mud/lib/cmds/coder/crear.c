
#include <cmd.h>

#define PATH_CREATOR "/std/areas/path_creator.c"
#define AREA_CREATOR "/std/areas/area_creator.c"

inherit CMD_BASE;

string master_file = "";
int mirror_x = 0;
int mirror_y = 0;
string creator = "";
int iterations = 1;
int exits = 8;
int city_mode = 0;


void setup()
{
	position = 0;
}

string query_usage()
{
	return "crear";
}

string query_short_help()
{
	return "Inicia el sistema de creación de zonas, caminos y ciudades.";
}

int choose_creator(string str, object me);
int choose_mirror_x(string str, object me);
int choose_mirror_y(string str, object me);
int choose_iterations(string str, object me);
int choose_exits(string str, object me);
int choose_city_mode(string str, object me);
int finish_creation(object me);

protected int cmd (string arg, object me, string verb)
{
	string str = arg;
	object ob;
	
	mirror_x = 0;
	mirror_y = 0;
	creator = "";
	iterations = 1;
	exits = 8;
	city_mode = 0;
	master_file = "";
	
	if (!str || str == "")
	{
		notify_fail("Sintaxis: crear <path del archivo maestro>\n");
		return 0;		
	}
	
	// Comprobamos si el argumento es una ruta completa
	if (file_size(str) == -1)
	{
		str = me->query_path() + "/" + arg;
		if (file_size(str) == -1)
		{
			notify_fail("No se puede encontrar el archivo '"+arg+"'.\n");
			return 0;
		}
	}
	
	catch(ob = load_object(str));
	
	if (!ob)
	{
		notify_fail("El archivo '"+arg+"' no carga.\n");
		return 0;	
	}
	
	master_file = str;
	
	tell_object(me, "¿Quieres utilizar el creador de caminos o de áreas?\n"
		"\t(c) caminos\n"
		"\t(a) áreas\n"
		"\t(q) cancelar\n>"
		);
	input_to( (:choose_creator:), 2, me);
	return 1;
}

int choose_creator(string str, object me)
{
	switch(str)
	{
		case "c":
		case "camino":
		case "caminos":
			creator = PATH_CREATOR;
			break;
		case "a":
		case "area":
		case "areas":
		case "área":
		case "áreas":	
			creator = AREA_CREATOR;
			break;	
		default:
			tell_object(me, "Ok. Cancelando.\n");
			return 1;
	}
	
	if (!load_object(creator))
	{
		notify_fail("El creador no carga... eso no debería suceder.\n");
		return 0;	
	}	

	// Solo damos las opciones de espejado en caminos, en areas no tiene mucho sentido
	if (creator == PATH_CREATOR)
	{
		tell_object(me, "¿Quieres que se espeje el resultado sobre el eje X? (s/n) > ");
		input_to( (:choose_mirror_x:), 2, me);
	}
	else
	{
		tell_object(me, "¿Número de iteraciones del algoritmo? > ");
		input_to( (:choose_iterations:), 2, me);
	}
	return 1;	
}

int choose_mirror_x(string str, object me)
{
	switch(str)
	{
		case "s":
			mirror_x = 1;
			break;
		case "n":	
			mirror_x = 0;
			break;	
		default:
			tell_object(me, "Ok. Cancelando.\n");
			return 1;
	}
	
	tell_object(me, "¿Quieres que se espeje el resultado sobre el eje Y? (s/n) > ");
	input_to( (:choose_mirror_y:), 2, me);
	return 1;	
}

int choose_mirror_y(string str, object me)
{
	switch(str)
	{
		case "s":
			mirror_y = 1;
			break;
		case "n":	
			mirror_y = 0;
			break;	
		default:
			tell_object(me, "Ok. Cancelando.\n");
			return 1;
	}
	
	if (creator == PATH_CREATOR)
	{	
		finish_creation(me);
		return 1;
	}
		
	tell_object(me, "¿Número de iteraciones del algoritmo? > ");
	input_to( (:choose_iterations:), 2, me);
	return 1;	
}

int choose_iterations(string str, object me)
{
	iterations = atoi(str);

	tell_object(me, "¿Número de salidas por room? (4/8) > ");
	input_to( (:choose_exits:), 2, me);
	return 1;	
}

int choose_exits(string str, object me)
{
	exits = atoi(str);
	
	if ((exits != 4) && (exits != 8))
	{
		notify_fail("Sólo pueden ser cuatro u ocho salidas... Cancelando.\n");
		return 0;	
	}

	tell_object(me, "¿Quieres activar el modo ciudad (calles más rectas)? (s/n) > ");
	input_to( (:choose_city_mode:), 2, me);
	return 1;	
}

int choose_city_mode(string str, object me)
{
	switch(str)
	{
		case "s":
			city_mode = 1;
			break;
		case "n":	
			city_mode = 0;
			break;	
		default:
			tell_object(me, "Ok. Cancelando.\n");
			return 1;
	}
	
	finish_creation(me);
	return 1;
}

int finish_creation(object me)
{
	object ob = load_object(creator);
	
	if (!ob)
	{
		notify_fail("El creador no carga... eso no debería suceder.\n");
		return 0;	
	}	
	
	switch(creator)
	{
		case AREA_CREATOR:
			ob->create_area(master_file, mirror_x, mirror_y, iterations, exits, city_mode);
			break;
		case PATH_CREATOR:
			ob->create_path(master_file, mirror_x, mirror_y);
			break;
		default:
			tell_object(me, "Ok. Cancelando.\n");
			return 1;		
	}
	return 1;
}