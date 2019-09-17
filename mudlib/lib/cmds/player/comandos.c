#include <standard.h>
#include <cmd.h>


#include <combat.h>

inherit CMD_BASE;

void setup() 
{
        position = 0;
}

protected int cmd(string str, object me, string verb)
{
	object handler = load_object("/cmds/handlers/cmd_handler.c");
	object command;
	string filename;
	string help;
	string * aux;
	mapping cmds;
	string result = "", result_aux = "";
	int i, j;
	string * categories = ({ });
	int list_mode = 0;
	
	if (!handler)
	{
		notify_fail("No se ha podido obtener la lista de comandos disponibles.\n");
		return 0;
	}
	
	if (str && str != "-l")
	{
		tell_object(me, "Sintaxis: comandos (para listar todos los comandos)\n"
		                "          comandos -l (para una lista con más información)\n\n");
		return 1;
	}
	
	if (str == "-l")
		list_mode = 1;
	
	cmds = handler->query_available_cmds_by_category(me);
	
	categories = keys(cmds);
	
	for (i = 0; i < sizeof(categories); i++)
	{
		result += "\n%^GREEN%^Comandos de " + categories[i] + ":%^RESET%^\n";
		
		for (j = 0; j < sizeof(cmds[categories[i]]); j++)
		{
			aux = explode(cmds[categories[i]][j], "/");
			filename = aux[sizeof(aux) - 1];
			filename = explode(filename, ".")[0];

			if (list_mode)
			{
				catch(command = load_object(cmds[categories[i]][j]));
			
				if (!command)
					continue;
				
				if (command->query_short_help())
					// help = command->query_short_help()[0..60];
					help = command->query_short_help();
				else 
					help = "";
			
				if (help != "")
					result += sprintf("  %13-=s: %s\n", filename, help);
				else
					result += sprintf("  %13-=s\n", filename);
			}
			else
			{
				result_aux += filename + "\n";
			}
		}
		
		if (!list_mode)
		{
			result += sprintf("%-#*s\n", me->query_cols(), result_aux);
			result_aux = "";
		}
	}
	
	// tell_object(me, result);
	me->more_string(result + "\n", "[Comandos]");
	
    return 1;
}

