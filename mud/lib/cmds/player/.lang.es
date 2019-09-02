
// common

#define _LANG_CMD_NOTHING_HERE "No hay nada con el nombre '"+arg+"' aquí.\n"

// look.c

#define _LANG_CMD_LOOK "mirar"
#define _LANG_CMD_LOOK_SYNTAX "mirar [a] [<objeto>]"
#define _LANG_CMD_LOOK_HELP "Devuelve la descripción de un objeto o (por defecto) " + \
                    "la localización en la que te encuentras."
#define _LANG_CMD_LOOK_NO_ENVIRONMENT "No estás en ningún sitio... no puedes ver nada.\n"
#define _LANG_CMD_LOOK_BLINDED "No puedes ver nada. ¡Estás cegado!\n"

// finger.c

#define _LANG_FINGER_SYNTAX "finger <nombre>"
#define _LANG_FINGER_NAME "Nombre"
#define _LANG_FINGER_REAL_NAME "Nombre real"
#define _LANG_FINGER_CITY "Ciudad"
#define _LANG_FINGER_BIRTHDAY "Cumpleaños"

// whoami

#define _LANG_WHOAMI_HELP "Muestra onformación sobre tu propio personaje."
#define _LANG_WHOAMI_CLASS_FIRST "   Clase:\n"
#define _LANG_WHOAMI_CLASS_EACH "     " + (me->query_class_ob())->query_short() + \
      " de nivel " + me->query_level() + ".\n"
#define _LANG_WHOAMI_GUILD_FIRST "   Gremios:\n"
#define _LANG_WHOAMI_GUILD_EACH "     " + capitalize(g_names[i]->query_short()) + \
      " de nivel " + guilds[g_names[i]][0] + ".\n";
#define _LANG_WHOAMI_JOB_FIRST "   Oficios:\n"
#define _LANG_WHOAMI_JOB_EACH "     " + capitalize(j_names[i]->query_short()) + \
      " de nivel " + jobs[j_names[i]][0] + ".\n";

// who

#define _LANG_WHO_HELP "Sintaxis: who\n" + \
                       "          who programadores\n" + \
                       "          who jugadores\n" + \
                       "          who <raza|cultura>\n"
#define _LANG_WHO_OPTION_CODERS "programadores"
#define _LANG_WHO_OPTION_PLAYERS "jugadores"
#define _LANG_WHO_REAL_WORLD_DATE "Hoy es " + ctime(time()) + " en el mundo real"
#define _LANG_WHO_EDITING_MSG " %^GREEN%^(Editando: " + (string)tmp + ")%^RESET%^"
#define _LANG_WHO_AWAY_MSG " (%^WHITE%^%^BOLD%^Ausente%^RESET%^)"
#define _LANG_WHO_IDLE_MSG " (%^CYAN%^%^BOLD%^Inactivo: " + (user->query_idle() / 60) + "%^RESET%^)"
#define _LANG_WHO_GUEST_MSG " invitad" + ((arr[i]->query_gender() == 2) ? "a" : "o") + " en " + mud_name()
#define _LANG_WHO_NO_CODERS_MSG "> %^GREEN%^No hay programadores conectados%^RESET%^ <"
#define _LANG_WHO_NO_PLAYERS_MSG "> %^GREEN%^No hay jugadores conectados%^RESET%^ <"
#define _LANG_WHO_IS_THE_ONLY_ONE "> %^GREEN%^Es el único que está conectado en " + mud_name() + "%^RESET%^ <"
#define _LANG_WHO_YOU_ARE_THE_ONLY_ONE "> %^GREEN%^Eres el único que está conectado en " + mud_name() + "%^RESET%^ <"
#define _LANG_WHO_ONLY_CODERS_MSG "> %^GREEN%^Hay " + query_num(creators, 100) + " programador" + \
        (creators < 2 ? "" : "es") + " en " + mud_name() + "%^RESET%^ <"
#define _LANG_WHO_ONLY_PLAYERS_MSG "> %^GREEN%^Hay " + query_num(num_people, 100) + " jugador" + \
        (num_people < 2 ? "" : "es") + " en " + mud_name() + "%^RESET%^ <"
#define _LANG_WHO_MULTIPLE_MSG "> %^GREEN%^Hay " + query_num(creators, 100) + " programador" + \
      (creators < 2 ? "" : "es") + " y " + query_num(num_people, 100) + " jugador" + \
      (num_people < 2 ? "" : "es") + " en " + mud_name() + "%^RESET%^ <"
#define _LANG_WHO_DISCONNECTED_MSG "> %^GREEN%^" + capitalize(number_as_string(num_disconnected_people)) + \
      " usuario"+(num_disconnected_people < 2 ? "" : "s")+" con la conexión caída%^RESET%^ <"
