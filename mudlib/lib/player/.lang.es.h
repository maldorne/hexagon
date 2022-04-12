
#define _LANG_YES_OPTIONS_ARRAY ({ 's', 'S' })
#define _LANG_NO_OPTIONS_ARRAY ({ 'n', 'N' })

// alias.c 

#define _LANG_ALIAS_LIST_HEADER "Lista de alias definidos:\n"
#define _LANG_ALIAS_SEMICOLON_NOT_ALLOWED "\";\" no permitido en los alias.\n"
#define _LANG_ALIAS_CONFIRM_FLUSH "Esto borrar� todos tus alias, �est�s seguro? (s/n)\n"
#define _LANG_ALIAS_FLUSHED "Ok, alias borrados.\n"
#define _LANG_ALIAS_OK "Ok.\n"
#define _LANG_ALIAS_NO_ALIAS "No has definido ning�n alias a�n.\n"
#define _LANG_ALIAS_NO_ALIAS_WITH_THAT "No has definido a�n ning�n alias con '"+str+"'.\n"
#define _LANG_ALIAS_TOO_MANY "Demasiados alias definidos, ya no puedes a�adir m�s.\n"
#define _LANG_ALIAS_TOO_LONG "Alias demasiado largo.\n"
#define _LANG_ALIAS_NOT_ALLOWED "Eso no est� permitido.\n"
#define _LANG_ALIAS_CHANGED "Ok, establecido el alias '" + s1 + "'.\n"
#define _LANG_ALIAS_SYNTAX "Sintaxis: " + query_verb() + " <alias>\n"
#define _LANG_ALIAS_REMOVED "Ok, borrado el alias '" + str + "'.\n"
#define _LANG_ALIAS_NO_RECURSIVE "�Alias recursivos? Eso no est� permitido.\n"

// events.c

#define _LANG_EVENTS_SOMEBODY_ARRIVES "Notas que alguien llega a tu posici�n.\n"
#define _LANG_EVENTS_SOMEBODY_LEAVES  "Notas marcharse a uno de los que estaban aqu�.\n"

// start.c


// quit.c

#define _LANG_QUIT_LOADING "Todav�a se esta cargando tu equipo, espera a que termine el proceso.\n"
#define _LANG_QUIT_EXITING "Saliendo...\n"
#define _LANG_QUIT_ENV_MSG this_object()->query_cap_name()+ \
            " busca un lugar c�modo para salir del juego.\n"
#define _LANG_QUIT_STOP_COMBAT_FIRST "Debes terminar primero tus combates.\n"+ \
        "Escribe '%^BOLD%^detener combates%^RESET%^' para terminarlos lo antes posible.\n"
#define _LANG_QUIT_STOP_MOVING_FIRST "Si no paras de moverte ser� imposible buscar un buen lugar "+ \
        "para salir del juego.\n"
#define _LANG_QUIT_BEEN_MOVED "En tu localizaci�n previa no es posible salir, "+ \
        "has sido movido a la m�s cercana (vuelve a intentarlo aqu� si a�n "+ \
        "deseas desconectar).\n"
#define _LANG_QUIT_DROP_ITEM "Dejas tu "+ob[i]->short()+".\n"
#define _LANG_QUIT_THANKS "Gracias por jugar, �hasta la pr�xima!\n"
#define _LANG_QUIT_INFORM_CODERS this_object()->query_cap_name()+" sale de "+mud_name()
#define _LANG_QUIT_INFORM_PLAYERS this_object()->query_cap_name()+" sale de "+mud_name()
#define _LANG_QUIT_ENV_QUIT this_object()->query_cap_name()+" sale de "+mud_name()+".\n"

// read.c

#define _LANG_READ "Lees:\n" + str +"\n"
#define _LANG_READ_IN_LANGUAGE "Lees un texto escrito en " + lang + ":\n\n" + str + "\n"
#define _LANG_READ_UNABLE_WITH_GARBLE "No eres capaz de entender el siguiente texto:\n" + \
        str + (lang ? "\nParece escrito en " + lang + "." : "") + "\n"
#define _LANG_READ_WHAT "Habr�as jurado que habia algo escrito aqu�...\n"; 
#define _LANG_READ_UNABLE_WITHOUT_GARBLE "No eres capaz de entender el texto" + \
        (lang ? ", parece escrito en " + lang : "") + ".\n"
#define _LANG_READ_IMPOSSIBLE "Todo " + mud_name() + " tiembla al encontrar una escritura m�gica que no " + \
        "tiene forma de ser interpretada.\n"
