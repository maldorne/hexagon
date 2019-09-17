
// encumbrance cmd from player.c, now external. Morgoth 2/Mar/95

#include <standard.h>
#include <cmd.h>

#define MONEY_HAND "/obj/handlers/money_handler.c"

inherit CMD_BASE;

void setup()
{
	position = 0;
}

string query_usage()
{
	return "dinero";
}

string query_short_help()
{
	return "Muestra la cantidad de dinero total que llevas en efectivo y que tienes guardada en los bancos.";
}

protected int cmd (string arg, object me, string verb)
{
	string str = "";
	string * props = keys(me->query_properties());
	string * pieces;
	object bank;
	int i, value, total = 0;
	
	value = me->query_value();
	if (value)
	{
		str = "Llevas encima monedas por valor de:\n\t" + MONEY_HAND->money_value_string(value) + "\n\n";
		total += value;
	}
	
	for (i = 0; i < sizeof(props); i++)
	{
		pieces = explode(props[i], " ");
		if (sizeof(pieces) == 2 && pieces[0] == "bank")
		{
			// Comprobamos que el banco sigue existiendo
			bank = load_object(pieces[1]);
			if (bank && bank->query_bank())
			{
				value = me->query_property(props[i]);
				if (value)
				{
					str += "En ["+bank->query_short()+"] tienes almacenado:\n\t";
					str += MONEY_HAND->money_value_string(value) + ".\n";
					total += value;
				}
				else
				{
					str += "En ["+bank->query_short()+"] tienes una cuenta abierta sin fondos.\n";
				}
			}
		}
	}
	
	str += "\nEn total, tienes acumulado:\n\t"+MONEY_HAND->money_value_string(total) + ".\n";

	// str = sprintf("%-=*s\n", me->query_cols(), str);

	tell_object(me, str);

	return 1;
}
