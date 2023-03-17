
#define _LANG_YES_OPTIONS_ARRAY ({ 's', 'S' })
#define _LANG_NO_OPTIONS_ARRAY ({ 'n', 'N' })

// alias.c 

#define _LANG_ALIAS_LIST_HEADER "Lista de alias definidos:\n"
#define _LANG_ALIAS_SEMICOLON_NOT_ALLOWED "\";\" no permitido en los alias.\n"
#define _LANG_ALIAS_CONFIRM_FLUSH "Esto borrará todos tus alias, ¿estás seguro? (s/n)\n"
#define _LANG_ALIAS_FLUSHED "Ok, alias borrados.\n"
#define _LANG_ALIAS_OK "Ok.\n"
#define _LANG_ALIAS_NO_ALIAS "No has definido ningún alias aún.\n"
#define _LANG_ALIAS_NO_ALIAS_WITH_THAT "No has definido aún ningún alias con '"+str+"'.\n"
#define _LANG_ALIAS_TOO_MANY "Demasiados alias definidos, ya no puedes añadir más.\n"
#define _LANG_ALIAS_TOO_LONG "Alias demasiado largo.\n"
#define _LANG_ALIAS_NOT_ALLOWED "Eso no está permitido.\n"
#define _LANG_ALIAS_CHANGED "Ok, establecido el alias '" + s1 + "'.\n"
#define _LANG_ALIAS_SYNTAX "Sintaxis: " + query_verb() + " <alias>\n"
#define _LANG_ALIAS_REMOVED "Ok, borrado el alias '" + str + "'.\n"
#define _LANG_ALIAS_NO_RECURSIVE "¿Alias recursivos? Eso no está permitido.\n"

// events.c

#define _LANG_EVENTS_SOMEBODY_ARRIVES "Notas que alguien llega a tu posición.\n"
#define _LANG_EVENTS_SOMEBODY_LEAVES  "Notas marcharse a uno de los que estaban aquí.\n"

// start.c

#define _LANG_START_INFORM_CODER query_cap_name() + \
                (strlen(query_gtitle()) ? " " + query_gtitle() : "") + \
                " entra en " + mud_name()
#define _LANG_START_INFORM_PLAYER query_cap_name() + " entra " + \
                (query_property(GUEST_PROP) ? ("como invitad" + G_CHAR + " ") : "") + \
                "en " + mud_name() + \
                (is_new_player ? " (%^GREEN%^BOLD%^Nuev" + G_CHAR + " jugador" + \
                  (query_gender() == 2 ? "a" : "") + "%^RESET%^)" : "")
#define _LANG_NEW_MANDATORY_ITEM "Por algún error has debido perder tu " + ob->query_name() + ". " + \
              "Otr" + ob->query_vowel() + " nuev" + ob->query_vowel() + " te es concedid" + ob->query_vowel() + ".\n"

// quit.c

#define _LANG_QUIT_LOADING "Todavía se esta cargando tu equipo, espera a que termine el proceso.\n"
#define _LANG_QUIT_EXITING "Saliendo...\n"
#define _LANG_QUIT_ENV_MSG this_object()->query_cap_name()+ \
            " busca un lugar cómodo para salir del juego.\n"
#define _LANG_QUIT_STOP_COMBAT_FIRST "Debes terminar primero tus combates.\n"+ \
        "Escribe '%^BOLD%^detener combates%^RESET%^' para terminarlos lo antes posible.\n"
#define _LANG_QUIT_STOP_MOVING_FIRST "Si no paras de moverte será imposible buscar un buen lugar "+ \
        "para salir del juego.\n"
#define _LANG_QUIT_BEEN_MOVED "En tu localización previa no es posible salir, "+ \
        "has sido movido a la más cercana (vuelve a intentarlo aquí si aún "+ \
        "deseas desconectar).\n"
#define _LANG_QUIT_DROP_ITEM "Dejas tu "+ob[i]->short()+".\n"
#define _LANG_QUIT_THANKS "Gracias por jugar, ¡hasta la próxima!\n"
#define _LANG_QUIT_INFORM_CODERS this_object()->query_cap_name()+" sale de "+mud_name()
#define _LANG_QUIT_INFORM_PLAYERS this_object()->query_cap_name()+" sale de "+mud_name()
#define _LANG_QUIT_ENV_QUIT this_object()->query_cap_name()+" sale de "+mud_name()+".\n"

// read.c

#define _LANG_READ "Lees:\n" + str +"\n"
#define _LANG_READ_IN_LANGUAGE "Lees un texto escrito en " + lang + ":\n\n" + str + "\n"
#define _LANG_READ_UNABLE_WITH_GARBLE "No eres capaz de entender el siguiente texto:\n" + \
        str + (lang ? "\nParece escrito en " + lang + "." : "") + "\n"
#define _LANG_READ_WHAT "Habrías jurado que habia algo escrito aquí...\n"; 
#define _LANG_READ_UNABLE_WITHOUT_GARBLE "No eres capaz de entender el texto" + \
        (lang ? ", parece escrito en " + lang : "") + ".\n"
#define _LANG_READ_IMPOSSIBLE "Todo " + mud_name() + " tiembla al encontrar una escritura mágica que no " + \
        "tiene forma de ser interpretada.\n"

// death

#define _LANG_DEATH_KILLED_BY query_cap_name() + " matad"+G_CHAR+" por "
#define _LANG_DEATH_KILLED_HIMSELF "(sobre sí mismo, quizá con un call)"
#define _LANG_DEATH_KILLED query_cap_name() + " ha muerto.\n"
#define _LANG_DEATH_CON_TOO_LOW "Algo va mal... tu constitución es demasiado baja.\n"
#define _LANG_DEATH_RAISED_ME "Recuperas tu forma mortal.\n"
#define _LANG_DEATH_RAISED_ROOM query_cap_name() + " recupera su forma mortal.\n"

#define _LANG_DEATH_MESSAGE_NO_ROOM "Por algún oscuro deseo de los poderes divinos, tu alma debe continuar en este plano de existencia.\n"
#define _LANG_DEATH_MESSAGE_ROOM " da un grito desgarrador mientras su alma abandona este mundo.\n"
#define _LANG_DEATH_MESSAGE_YOU "Tu espíritu se eleva sobre tu cuerpo y abandona este mundo.\n"
#define _LANG_DEATH_MESSAGE_ROOM_AFTER " llega al infierno para ser castigado por sus pecados en vida.\n"
