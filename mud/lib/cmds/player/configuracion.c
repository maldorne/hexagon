/* ********************************************************************
* Folken 04
* Comando para mostrar y cambiar todos los parametros configurables de un jugador
* ********************************************************************
*/

#include <standard.h>
#include <cmd.h>
#include <term.h>

inherit CMD_BASE;

#include <configuration.h>

int config(string str, object me, string verb);
int config_value(object me, string category, string subcategory, string value);  

protected int cmd (string str, object me, string verb){

	mapping config = CONFIG_TABLE->query_config_data();
	string * categories = keys(config);
	string * subcategories;
	int i = 0;
	int j = 0;
	string ret = "";
	mapping current_map;
	string func_name;
	mixed result;

	if (str && (str != ""))
	{
		return config(str, me, verb);
	}

	// Si llegamos aqui damos el listado de configuracion completa
	ret =  "--------------------------------------------------------------------------\n";
	ret += "  Configuración del jugador: %^BOLD%^" + me->query_cap_name()+"%^RESET%^\n";
	ret += "--------------------------------------------------------------------------\n";

	for(i = 0; i < sizeof(categories); i++)
	{
		ret += "  %^BOLD%^" + capitalize(categories[i]) + "%^RESET%^:\n";

		current_map = config[categories[i]];
		subcategories = keys(current_map);

		for(j = 0; j < sizeof(subcategories); j++)
		{
			func_name = current_map[subcategories[j]][1];
			
			if (function_exists(func_name, me))
			{
				if (current_map[subcategories[j]][3] != "")
					result = call_other(me, func_name, current_map[subcategories[j]][3] );
				else
					result = call_other(me, func_name);
				
				if (current_map[subcategories[j]][0] == "bool")
					result = (result?"sí":"no");
					
				// ret += "    " + (string)subcategories[j] + ": " + result + "\n";
				ret += sprintf("    %18-s:%15|O     %%^GREEN%%^(%s)%%^RESET%%^\n", (string)subcategories[j], result, current_map[subcategories[j]][4]);
			}
			else{
				// ret += "    " + (string)subcategories[j] + ": Desconocido\n";
				ret += sprintf("    %18-s:%15|s\n", (string)subcategories[j], "Desconocido");
			}
		}
	}

	ret += "--------------------------------------------------------------------------\n";
	ret += "  Utiliza 'configuracion <categoría> <subcategoría> <valor>' para\n";
	ret += "    cambiar los valores.\n";
	ret += "  Utiliza el comando independiente indicado en color verde para obtener\n";
	ret += "    más información sobre cualquiera de los temas.\n";
	ret += "--------------------------------------------------------------------------\n";

	tell_object(me, ret);

	return 1;
}

int config(string str, object me, string verb){

	string * words = explode(str, " ");

	mapping config = CONFIG_TABLE->query_config_data();
	string * categories = keys(config);
	string * subcategories;
	int position, position2, j;
	mapping current_map;
	string func_name;
	mixed result = 1;
	mixed value, value_check;
	mixed result_check;
	int number, number_check;
	int check = 0;

	position = member_array(words[0], categories);
	// Si no es una categoria valida
	if ( position == -1 ){

		tell_object(me, "--------------------------------------------------------------------------\n");
		tell_object(me, "  '"+words[0]+"' no es una categoría válida a configurar.\n"
			"  Utiliza 'configuracion' sin ningún parámetro para ver\n  los valores que puedes ajustar.\n");
		tell_object(me, "--------------------------------------------------------------------------\n");
		return 1;
	}

	current_map = config[words[0]];
	subcategories = keys(current_map);

	if (sizeof(words) < 2){
		tell_object(me, "--------------------------------------------------------------------------\n");
		tell_object(me, "  Valores configurables dentro de "+words[0]+":\n");
		for(j = 0; j < sizeof(subcategories); j++){
			func_name = current_map[subcategories[j]][1];
			if (function_exists(func_name, me)){
				if (current_map[subcategories[j]][3] != "")
					result = call_other(me, func_name, current_map[subcategories[j]][3] );
				else{
					result = call_other(me, func_name);
				}
				if (current_map[subcategories[j]][0] == "bool")
					result = (result?"si":"no");
				// tell_object(me, "    " + (string)subcategories[j] + ": " + result + "\n");
				tell_object(me, sprintf("    %18-s:%15|O\n", (string)subcategories[j], result));
			}
			else{
				// tell_object(me, "    " + (string)subcategories[j] + ": Desconocido\n");
				tell_object(me, sprintf("    %18-s:%15|s\n", (string)subcategories[j], "Desconocido"));
			}  		
		}
		tell_object(me, "--------------------------------------------------------------------------\n");
		return 1;
	}

	position2 = member_array(words[1], subcategories);

	// No es una subcategoria valida
	if (position2 == -1){
		tell_object(me, "--------------------------------------------------------------------------\n");
		tell_object(me, "  '"+words[1]+"' no es una subcategoría válida dentro de '"+words[0]+"'.\n"
			"  Utiliza 'configuracion <categoría> <subcategoría>' sin ningún\n"+
			"  parámetro para ver los valores que puedes ajustar.\n");
		tell_object(me, "--------------------------------------------------------------------------\n");  	                
		return 1;
	}

	if (sizeof(words) < 3) {
		func_name = current_map[words[1]][1];
		if (function_exists(func_name, me)){
			if (current_map[words[1]][3] != "")
				result = call_other(me, func_name, current_map[words[1]][3] );
			else{
				result = call_other(me, func_name);
			}
			if (current_map[words[1]][0] == "bool")
				result = (result?"si":"no");
			tell_object(me, "Configuración para "+words[0] + "/" + words[1] + " actual: [" + result + "]\n");
		}
		else{
			tell_object(me, "Configuración para "+words[0] + "/" + words[1] + " actual: Desconocido\n");
		}
		check = 1;
	}  
	else{
		func_name = current_map[words[1]][2];
		if (function_exists(func_name, me)){
			if (current_map[words[1]][0] == "bool"){
				if ((words[2] == "si") || (words[2] == "s")){
					if (current_map[words[1]][3] != "")
						result = call_other(me, func_name, current_map[words[1]][3], 1);
					else{
						result = call_other(me, func_name, 1);
					}
					value = "si";
				}
				else{
					if (current_map[words[1]][3] != "")
						result = call_other(me, func_name, current_map[words[1]][3], 0);
					else{
						result = call_other(me, func_name, 0);
					}
					value = "no";
				}
			}
			else if (current_map[words[1]][0] == "int"){
				if (number = atoi(words[2])){
					value = words[2];
					if (current_map[words[1]][3] != "")
						result = call_other(me, func_name, current_map[words[1]][3], number);
					else{
						result = call_other(me, func_name, number);
					}
				}
				else
					return 0;
			}
			else{
				value = words[2];
				if (current_map[words[1]][3] != "")
					result = call_other(me, func_name, current_map[words[1]][3], value);
				else{
					result = call_other(me, func_name, value);
				}
			}
		}
		else{
			return 0;
		}

		// Comprobacion final: para dar el mensaje final comprobamos que el nuevo valor introducido
		// ha sido almacenado... no podemos hacerlo de otra forma, por ejemplo confiando en los return 1/0, 
		// ya que muchas funciones son void y los tipos se confunden...

		func_name = current_map[words[1]][1];
		if (function_exists(func_name, me)){
			if (current_map[words[1]][0] == "bool"){
				if (current_map[words[1]][3] != "")
					result_check = call_other(me, func_name, current_map[words[1]][3]);
				else{
					result_check = call_other(me, func_name);
				}
				if ((words[2] == "si") || (words[2] == "s")){
					if (1 == result_check)
						check = 1;
				}
				else{
					if (0 == result_check)
						check = 1;
				}
			}
			else if (current_map[words[1]][0] == "int"){
				if (number = atoi(words[2])){
					if (current_map[words[1]][3] != "")
						number_check = call_other(me, func_name, current_map[words[1]][3]);
					else{
						number_check = call_other(me, func_name);
					}
					if (number == number_check)
						check = 1;
				}
			}
			else{
				if (current_map[words[1]][3] != "")
					value_check = call_other(me, func_name, current_map[words[1]][3]);
				else{
					value_check = call_other(me, func_name);
				}
				if (value == value_check)
					check = 1;
			}
		}

		// Eliminado ya que muchas funciones devuelven void (que es considerado 0)
		// if (result) 
		if (check)
			tell_object(me, "Configuración para "+words[0] + "/" + words[1] + " establecida como: [" + value + "]\n");
	}

	// Eliminado ya que muchas funciones devuelven void (que es considerado 0)
	// return result;
	return check;
}

/*
int config_value(object me, string category, string subcategory, string value){

string func_name;

// Si no introducimos ningun nuevo valor
if ( value == "" ){
func_name = "show_" + category + "_" + subcategory;
if (function_exists(func_name, this_object() ))
return call_other(this_object(), func_name, me);
}
else{
func_name = "set_" + category + "_" + subcategory;
if (function_exists(func_name, this_object() ))
return call_other(this_object(), func_name, me, value);
}
}

*/
