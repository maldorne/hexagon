
// finger.c

#define _LANG_FINGER_SYNTAX "finger [<nombre>]"
#define _LANG_FINGER_HELP "Muestra información acerca del personaje jugador indicado."
#define _LANG_FINGER_NAME "Nombre"
#define _LANG_FINGER_ACCOUNT "Cuenta"
#define _LANG_FINGER_REAL_NAME "Nombre real"
#define _LANG_FINGER_CITY "Ciudad"
#define _LANG_FINGER_BIRTHDAY "Cumpleaños"
#define _LANG_FINGER_NOBODY "Nadie con el nombre " + str + " ha visitado "+mud_name()+".\n"
#define _LANG_FINGER_FORBIDDEN "(tiene prohibido el acceso)"
#define _LANG_FINGER_FORBIDDEN_REASON "Prohibido por: '" + capitalize(ret) + "'\n"

// who

#define _LANG_WHO_SYNTAX "who\n" + \
                         "          who programadores\n" + \
                         "          who jugadores\n" + \
                         "          who <raza|cultura>"
#define _LANG_WHO_HELP "Muestra un listado de los personajes conectados."
#define _LANG_WHO_OPTION_CODERS "programadores"
#define _LANG_WHO_OPTION_PLAYERS "jugadores"
#define _LANG_WHO_REAL_WORLD_DATE "Hoy es " + ctime(time(), 5) + " en el mundo real"
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

// help

#define _LANG_CMD_HELP "ayuda"
#define _LANG_CMD_HELP_HELP "Muestra la ayuda disponible para un tema en particular."

// characters

#define _LANG_CMD_CHARACTERS "personajes"
#define _LANG_CMD_CHARACTERS_HELP "Muestra todos los personajes disponibles en tu cuenta."
#define _LANG_AVAILABLE_CHARACTERS_IN_ACCOUNT "Tienes disponibles en esta cuenta los siguientes personajes:\n"
#define _LANG_CMD_CHARACTERS_NO_LOGIN "Debes hacer login primero."

// games

#define _LANG_CMD_GAMES "juegos"
#define _LANG_CMD_GAMES_HELP "Lista todos los juegos disponibles en " + mud_name() + "."
#define _LANG_CMD_GAMES_AVAILABLE "Los siguientes juegos están disponibles:\n"
#define _LANG_CMD_GAMES_NO_LOGIN "Debes hacer login primero."
#define _LANG_CMD_GAMES_UNAVAILABLE_GAME "No jugable"
