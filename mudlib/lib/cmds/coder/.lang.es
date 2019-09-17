
// common

#define _LANG_CMD_FILE "Archivo"
#define _LANG_CMD_FILE_DOES_NOT_EXIST "No existe ese archivo.\n"
#define _LANG_CMD_CANNOT_READ_FILE "No se ha podido leer ese archivo.\n"
#define _LANG_CMD_NO_FILES "No hay archivos coincidentes.\n"

// uptime

#define _LANG_UPTIME_SINCE "El mud está en funcionamiento desde hace:\n\t"
#define _LANG_UPTIME_DAY "día"
#define _LANG_UPTIME_HOUR "hora"
#define _LANG_UPTIME_MINUTE "minuto"
#define _LANG_UPTIME_SECOND "segundo"

// cat

#define _LANG_CAT_WHAT "¿Cat de qué archivo?\n"
#define _LANG_CAT_ASTERISK "¿cat *? ¡Olvídalo!\n"
#define _LANG_CAT_TRUNCATED "\n ----- TRUNCADO ----- \n\n"

// clone

#define _LANG_CLONE_WHAT "¿Clonar que?\n"
#define _LANG_CLONE_ERROR_IN "Error en "
#define _LANG_CLONE_ERROR_MAYBE_UNIQUE "Error, no se ha podido clonar el objeto (¿objeto único?).\n"
#define _LANG_CLONE_OK_YOU "Ok. Objeto " + file_name(ob) + " clonado en " + \
        (environment(ob) == this_player() ? "ti" : \
        (environment(ob) == environment(this_player()) ? "este lugar" : \
          this_player()->desc_object(ob))) + \
        ".\n"
#define _LANG_CLONE_OK_ENV this_player()->query_cap_name() + " busca " + \
        (string)ob->query_numeral() + " " + \
        ((string)ob->query_short() ? (string)ob->query_short() : "objeto") + \
        " en otra dimensión.\n"
#define _LANG_CLONE_ERROR "Error, no se ha podido clonar el objeto.\n"

// compare

#define _LANG_COMPARE_HELP "Permite comparar dos archivos. " + \
      "El comando devuelve las lineas de file1 que no se encuentran en " + \
      "file2 si which_file es igual a 1, y viceversa si which_file es igual a 2.\n\n" + \
      "Nota: ambas diferencias de file1 y file2 serán devueltas si which_file no " + \
      "se especifica."
#define _LANG_COMPARE_SYNTAX "Sintaxis: compare -<which_file> file1 file2\n"
#define _LANG_COMPARE_INVALID_WHICH "Valor inválido para -<which_file>.\n\nOpciones:\n" + \
      "-1 para file1\n-2 para file2\n-3 o ningún valor para ambos\n"
#define _LANG_COMPARE_NOT_A_FILE "No es un archivo: "
