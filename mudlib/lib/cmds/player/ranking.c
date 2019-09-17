#include <standard.h>
#include <cmd.h>

#include <user/player.h>

inherit CMD_BASE;


void setup()
{
        position = 0;
}

int compare_ints(int a, int b)
{
    if(a == b) return 0;
    if(!a || !b) return -1;
    if(a > b) return -1;
    return 1;
}

private string get_group_ranking(mapping rank)
{
	string ret = "";
	int * levels = keys(rank);
	int i; //, j;
	string * names = ({ });
	
	levels = sort_array(levels, "compare_ints");
	
	for ( i = 0; i < sizeof(levels); i++)
	{
		names = rank[levels[i]];

		// si la lista esta vacia (por algun error), eliminamos la entrada del mapping
		if (sizeof(names) == 0)
			map_delete(rank, levels[i]);
		else
		{
			ret += "  Nivel " + (levels[i] < 10 ? " " : "") + levels[i] + ": ";
			ret += query_multiple_short(names) + "\n";
		}
	}
	
	return ret;
}

private void remove_name(string who, mapping rank)
{
	string * rank_groups = keys(rank);
	mapping levels;
	int i, j;
	int * indexes;
	
	for (i = 0; i < sizeof(rank_groups); i++)
	{
		levels = rank[rank_groups[i]]; // almacenamos el subranking completo de cada grupo social
		indexes = keys(levels); // almacenamos todas las entradas (niveles) de ese subranking
		
		for (j = 0; j < sizeof(indexes); j++)
		{
			tell_object(find_living("folken"), "Eliminando "+who+" de "+rank_groups[i]+" nivel "+indexes[j]+"\n");
			rank[rank_groups[i]][indexes[j]] -= ({ who });

			// si hemos vaciado la lista, la eliminamos del mapping
			if (rank[rank_groups[i]][indexes[j]] == ({ }) )
				map_delete(rank[rank_groups[i]], indexes[j]);
		}
	}
}

protected int cmd(string str, object me, string verb) 
{
    string ret = " %^GREEN%^Ranking de niveles de tus grupos:%^RESET%^\n";
	object ob;
	mapping rank;
	string * rank_groups = ({ });
	string * my_groups = ({ });
	int i, complete = 0;
	string group_name, aux = "";

	ob = load_object(RANKING_OB);

	if (!ob)
	{
		notify_fail("Hay algún error con la clasificación.\n");
		return 0;
	}
	
	rank = ob->query_ranking();
	rank_groups = keys(rank);
	
	if (str && me->query_coder() && (sscanf(str, "eliminar %s", aux) == 1))
	{
		remove_name(aux, rank);
		ob->save_handler();
		return 1;
	}
	
	if ((str == "todos") && me->query_coder())
	{
		my_groups = keys(rank);
		complete = 1;
	}
	else
		my_groups = me->query_object_list();

	for (i = 0; i < sizeof(my_groups); i++)
	{
		if (my_groups[i])
		{
			// Obtenemos el grupo al que pertenece el jugador
			// (solo si carga y su nombre de archivo real para evitar problemas
			//  de nombres "parecidos" pero no iguales)
			ob = load_object(my_groups[i]);
			if (!ob) 
				continue;
			group_name = real_file_name(ob);
			
			if (undefinedp(rank[group_name]))
				continue;
			
			ret += " %^GREEN%^" + ob->query_short() + "%^RESET%^\n";
			ret += get_group_ranking(rank[group_name]) + "\n";
			
		}
	}
	
	if (me->query_coder())
	{
		ret += "  Usa 'ranking eliminar <nombre>' para quitar un nombre de la lista.\n";
		
		if (!complete)
			ret += "  Usa 'ranking todos' para ver todos los grupos existentes.\n";
			
		ret += "    (sólo programadores)\n";
	}

    tell_object(me, ret);
    
    return 1;
}

