
// common

#define _LANG_CMD_FILE "Archivo"
#define _LANG_CMD_FILE_DOES_NOT_EXIST "No existe ese archivo.\n"
#define _LANG_CMD_CANNOT_READ_FILE "No se ha podido leer ese archivo.\n"
#define _LANG_CMD_NO_FILES "No hay archivos coincidentes.\n"

// clone

#define _LANG_CLONE_OK_ENV this_player()->query_cap_name() + " busca " + \
        (string)ob->query_numeral() + " " + \
        ((string)ob->query_short() ? (string)ob->query_short() : "objeto") + \
        " en otra dimensión.\n"

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

// peace

#define _LANG_PEACE_MESSAGES_MINE "Alzas tu mano y exclamas: " + \
      "%^YELLOW%^¡Dejad de luchar, mortales!%^RESET%^\n"
#define _LANG_PEACE_MESSAGES_OTHERS this_player()->query_cap_name() + \
        " alza su mano y exclama: %^YELLOW%^¡Dejad de luchar, mortales!" + \
        "%^RESET%^\n"
