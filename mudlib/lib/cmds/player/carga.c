
// encumbrance cmd from player.c, now external. Morgoth 2/Mar/95

#include <standard.h>
#include <cmd.h>

inherit CMD_BASE;

void setup()
{
	position = 0;
}

string query_usage()
{
	return "carga";
}

string query_short_help()
{
	return "Muestra tu carga actual y la máxima.";
}

protected int cmd (string arg, object me, string verb)
{
	int now;
	int max;
	now=me->query_loc_weight();
	max=me->query_max_weight();
	write("Tu inventario actual pesa "+now+" y puedes cargar "+
		"como mucho "+max+", gracias a tu fuerza.\n");
	return 1;
}
