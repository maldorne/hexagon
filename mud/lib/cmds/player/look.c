// look cmd from player.c, now external. Morgoth 2/Mar/95
// mirar.c reviewed for ccmud, neverbot 4/03

#include <mud/cmd.h>

#include <common/properties.h>
#include <areas/weather.h>

#define DEFAULT_TIME 10

inherit CMD_BASE;
// inherit "/global/player/more_string.c";

void setup()
{
	position = 0;
}

string query_usage()
{
	return "mirar [a] [<objeto>]";
}

string query_short_help()
{
	return "Devuelve la descripción larga de un objeto o (por defecto) la "+
		"habitación donde te encuentras.";
}

static int cmd(string arg, object me, string verb)
{
	object here, *ob;
	int i, dark;
	string ret;

	ret = "";
	here = environment(me);

	if (!here)
	{
		write("Estás en el limbo... perdona pero no puedes ver nada.\n");
		return 1;
	}

	if (me->query_timed_property(BLIND_PROP) && me->query_coder() == 0)
	{
		write("No puedes ver nada. ¡Estás cegado!\n");
		return 1;
	}

	// Antes incluido en /global/pweath.c, ahora aqui, Folken 11/03
	/*
	if ((arg == "sol") || (arg == "luna") || (arg == "lunas") || (arg == "clima") ||
	(arg == "tiempo")){
		// if ((string)environment()->query_property("location") != "outside")
		if (!here->query_outside())
		{
			notify_fail("Necesitas estar al aire libre para poder fijarte en el tiempo.\n");
			return 0;
		}

		if (arg == "sol")
			if (WEATHER->query_day())
			{
				write("¡Ouch, el sol es brillante!\n");
				return 1;
			}
			else
			{
				write("El sol no ha salido todavía, tendrás que esperar a que amanezca.\n");
				return 1;
			}
		if ((arg == "luna") || (arg == "lunas"))
			if (WEATHER->query_day())
			{
				write("Las lunas no han aparecido todavía, prueba más tarde.\n");
				return 1;
			}
			else if (WEATHER->moon_string() != "")
			{
				write(WEATHER->moon_string());
				return 1;
			}
			else
			{
				write("Las lunas no han aparecido todavía, prueba más tarde.\n");
				return 1;
			}
		write(WEATHER->weather_string(here));
		return 1;
	}
	*/

	// Fin del codigo de /global/pweath.c

	dark = me->check_dark((int)here->query_light());

	if (!arg || !stringp(arg) || arg == "")
	{
		if (me->query_coder())
			ret = file_name(here)+"\n";

		if(!dark)
				ret += here->short(dark)+".\n";

		ret += here->long(arg, dark); //+"\n";
		
		write(ret);

		// me->adjust_time_left(-LOOK_TIME);
		return 1;
	}

	sscanf(arg, "a %s", arg);
	arg = me->expand_nickname(arg);
	// if (arg == "todo") arg = "all";
	ob = find_match(arg, ({ me, here }),1 );
	ret = "";
	// me->adjust_time_left(-DEFAULT_TIME);

	if (!sizeof(ob))
	{
		notify_fail("Nada con el nombre "+arg+" está aquí.\n");
		return 0;
	}
	//for (i=sizeof(ob)-1;i>=0;i--) if (ob[i]->query_hide_shadow() && ob[i]->query_name()!=me->query_name()) ob=delete(ob,i,1);
	if (ob[0]->query_coder() && ob[0]->query_invis() && !me->query_coder())
	{
		notify_fail("Nada con el nombre "+arg+" está aquí.\n");
		return 0;
	}
	if (sizeof(ob))
	{
		if (pointerp(ob))
		{
			for (i=0; i < sizeof(ob); i++)
			{
				if (!ob[i]->query_player())
					ret += capitalize(ob[i]->short())+".\n";

				ret += ob[i]->long(arg, dark); //+"\n";
				// me->adjust_time_left(-EXAMINE_ITEM_TIME);
			}
		}
		else
		{
			ret += ob->long(arg, dark);
			// me->adjust_time_left(-EXAMINE_ITEM_TIME);
		}

		// me->do_efun_write(ret);
		me->more_string(ret, "Mirar");
		return 1;
	}

	notify_fail("Nada con el nombre "+arg+" está aquí.\n");
	return 0;
}
