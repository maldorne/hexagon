
// common

#define _LANG_CMD_SYNTAX "Sintaxis: "

// communicate.c

#define _LANG_COMM_ROWS "filas"
#define _LANG_COMM_COLS "columnas"
#define _LANG_COMM_ROWS_SYNTAX "Filas actualmente ajustadas a " + rows + ".\n"+query_verb() + \
        " <número> para cambiarlas.\n"
#define _LANG_COMM_ROWS_WRONG "Número incorrecto de filas (debe ser mayor o igual a 10).\n"
#define _LANG_COMM_ROWS_SET "Filas ajustadas a " + val + ".\n"
#define _LANG_COMM_COLS_SYNTAX "Columnas actualmente ajustadas a " + cols + ".\n"+query_verb() + \
        " <número> para cambiarlas.\n"
#define _LANG_COMM_COLS_WRONG "Número incorrecto de columnas (debe estar entre 35 y 200).\n"
#define _LANG_COMM_COLS_SET "Columnas ajustadas a " + val + ".\n"

#define _LANG_COMM_EARMUFFS "tapones"
#define _LANG_COMM_TERMINAL "terminal"
#define _LANG_COMM_BLOCK ({ "bloquear", "block" })
#define _LANG_COMM_UNBLOCK ({ "desbloquear", "unblock" })

// help.c

#define _LANG_HELP_CMD "Ayuda sobre el comando '" + str + "':\n"
#define _LANG_HELP_CMD_NO_HELP "No hay ningún texto de ayuda concreto."
#define _LANG_HELP_SYNTAX "Sintaxis: ayuda [<tema | comando>]\n"
#define _LANG_HELP_NAME "ayuda"
#define _LANG_HELP_NO_HELP_ABOUT "Lo siento, no hay ayuda sobre "+str+".\n"
#define _LANG_HELP_HELP_ABOUT "Ayuda sobre " + objs[loop]->query_name() + ":\n" + text + "\n\n"
#define _LANG_HELP_NO_HELP_FOR_ITEM "No hay ayuda sobre "+((sizeof(objs) > 1) ? "estos objetos":"este objeto")+".\n"

// more_string.c

#define _LANG_MORE_STRING_MORE "Sigue"
#define _LANG_MORE_STRING_STAT_LINE "$N > desde $T hasta $B de $S ($%%) - h para ayuda. "
#define _LANG_MORE_STRING_NOT_FOUND "Lo siento, "+s1+" no encontrado.\n"
#define _LANG_MORE_STRING_HELP_NOT_FOUND "El documento de ayuda " + \
                             "no está. Busca a alguien que lo arregle.\n"
#define _LANG_MORE_STRING_EMPTY_STRING "Cadena vacia.\n"

// notifications.c

#define _LANG_NO_NOTIFICATIONS "No has recibido notificaciones.\n"
#define _LANG_PAST_NOTIFICATIONS "Notificaciones anteriores"

// commands.c

#define _LANG_ACCOUNT_COMMANDS "" + \
        "Comandos de cuenta:\n" + \
        "   %^BOLD%^GREEN%^<nombre de personaje>%^RESET%^ para seguir jugando.\n" + \
        "   %^BOLD%^personajes%^RESET%^ para listar todos tus personajes.\n" + \
        "   %^BOLD%^crear%^RESET%^ para crear un nuevo personaje.\n" + \
        "   %^BOLD%^juegos%^RESET%^ para listar todos los juegos disponibles.\n" + \
        "   %^BOLD%^password%^RESET%^ para cambiar tu contraseña.\n"

// security

#define _LANG_COMM_PASSWORD ({ "password" })
#define _LANG_OLD_PASSWORD "Por favor, introduce tu antigua contraseña: "
#define _LANG_CANCELLING "Cancelando...\n"
#define _LANG_WRONG_PASSWORD "Contraseña incorrecta."
#define _LANG_UNKNOWN_PASSWORD_ERROR "Hay algún problema con la contraseña de tu cuenta.\n"
#define _LANG_NEW_PASSWORD "Introduce la nueva contraseña: "
#define _LANG_PASSWORD_CONDITIONS "La contraseña debe tener al menos seis caracteres."
#define _LANG_REPEAT_PASSWORD "Por favor, introdúcela de nuevo: "

// ui.c

#define _LANG_UI_SIDEBAR_SYNTAX "Usa sidebar <on|off> para activarla o desactivarla.\n"
#define _LANG_UI_MAP "Mapa"
#define _LANG_UI_STATS "Puntos"
