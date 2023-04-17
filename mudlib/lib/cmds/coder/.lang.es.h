
// clone

#define _LANG_CLONE_OK_ENV this_player()->query_cap_name() + " busca " + \
        (string)ob->query_numeral() + " " + \
        ((string)ob->query_short() ? (string)ob->query_short() : "objeto") + \
        " en otra dimensión.\n"

// peace

#define _LANG_PEACE_MESSAGES_MINE "Alzas tu mano y exclamas: " + \
      "%^YELLOW%^¡Dejad de luchar, mortales!%^RESET%^\n"
#define _LANG_PEACE_MESSAGES_OTHERS this_player()->query_cap_name() + \
        " alza su mano y exclama: %^YELLOW%^¡Dejad de luchar, mortales!" + \
        "%^RESET%^\n"
