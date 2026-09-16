
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
#define _LANG_HELP_EMOTIONS ({ "emocion", "emoción", "emociones", "soul", "souls" })
#define _LANG_HELP_MORE_EMOTIONS "Emociones"

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
#define _LANG_ERROR_IN_PASSWORD "Hay un error con tu contraseña, contacta con un administrador.\n"

// ui.c

#define _LANG_UI_SIDEBAR_SYNTAX "Usa sidebar <on|off> para activarla o desactivarla.\n"
#define _LANG_UI_MAP "Mapa"
#define _LANG_UI_STATS "Puntos"

// help topics

#define _LANG_HELP_REINDEX ({ "reindex", "reindexar" })
#define _LANG_HELP_REINDEXED(n) "Indexados " + (n) + " temas de ayuda.\n"
#define _LANG_HELP_SECTIONS_FOOTER \
        "  ayuda <sección>    los temas de una sección\n" + \
        "  ayuda <tema>       leer uno\n" + \
        "  ayuda *palabra*    buscar\n" + \
        "  comandos           todo lo que puedes teclear\n"
#define _LANG_HELP_SECTIONS_STAFF "  para programadores"
#define _LANG_HELP_KIND_INDEX "índice"
#define _LANG_HELP_SEE_ALSO "Ver también: " + implode(words, ", ") + ".\n"
#define _LANG_HELP_MATCHES "\nCoinciden:\n\n"
#define _LANG_HELP_NO_MATCHES "No hay ningún tema que coincida.\n"
#define _LANG_HELP_TITLE(name, kind) "%^BOLD%^WHITE%^" + (name) + "%^RESET%^ (" + (kind) + ")"
#define _LANG_HELP_KIND_SEPARATOR "·"
#define _LANG_HELP_KIND_COMMAND "comando"
#define _LANG_HELP_KIND_SKILL "habilidad"
#define _LANG_HELP_KIND_TOPIC "tema"
#define _LANG_HELP_KIND_CODER_TOPIC "tema para creadores"
#define _LANG_HELP_KIND_DRIVER "manual del driver"
#define _LANG_HELP_KIND_SOUL "emoción"
#define _LANG_HELP_KIND_SECTION "sección"

// account details. `str` is what the player just typed
#define _LANG_ACCOUNT_DETAILS_VERBS ({ "datos", "chfn" })
#define _LANG_ACCOUNT_NONE_WORDS ({ "ninguno", "ninguna" })

#define _LANG_ACCOUNT_DETAILS_HEADER "Cambiando los datos personales de tu cuenta.\n" + \
  "Si no introduces algún dato, se tomará la opción por defecto (entre corchetes).\n"

#define _LANG_ACCOUNT_ASK_REAL_NAME "\nIntroduce tu nombre real [" + \
  this_object()->query_real_name() + "]\n('" + _LANG_ACCOUNT_NONE_WORDS[0] + "' para borrarlo): "
#define _LANG_ACCOUNT_REAL_NAME_CLEARED "Ok, nombre real borrado.\n"
#define _LANG_ACCOUNT_REAL_NAME_SET "Ok, nombre real establecido como " + str + ".\n"
#define _LANG_ACCOUNT_REAL_NAME_KEPT "Ok, nombre real sin modificar.\n"

#define _LANG_ACCOUNT_ASK_LOCATION "\nIntroduce tu lugar de residencia (ej: Madrid, Estocolmo... ) [" + \
  this_object()->query_location() + "]\n('" + _LANG_ACCOUNT_NONE_WORDS[1] + "' para borrarla): "
#define _LANG_ACCOUNT_LOCATION_CLEARED "Ok, lugar de residencia borrado.\n"
#define _LANG_ACCOUNT_LOCATION_SET "Ok, lugar de residencia establecido como " + str + ".\n"
#define _LANG_ACCOUNT_LOCATION_KEPT "Ok, lugar de residencia sin modificar.\n"

#define _LANG_ACCOUNT_ASK_BIRTHDAY "\nIntroduce tu fecha de cumpleaños (ddmm) [" + \
  this_object()->query_birthday() + "]\n('" + _LANG_ACCOUNT_NONE_WORDS[1] + "' para borrarla): "
#define _LANG_ACCOUNT_BIRTHDAY_CLEARED "Ok, fecha de cumpleaños borrada.\n"
#define _LANG_ACCOUNT_BIRTHDAY_SET "Ok, fecha de cumpleaños establecida como " + str + ".\n"
#define _LANG_ACCOUNT_BIRTHDAY_KEPT "Ok, fecha de cumpleaños sin modificar.\n"
#define _LANG_ACCOUNT_BIRTHDAY_INVALID "La fecha no es válida, vuelve a introducirla (ddmm): "
