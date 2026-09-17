
// clone

#define _LANG_CLONE_OK_ENV this_player()->query_cap_name() + " busca " + \
        (string)ob->query_numeral() + " " + \
        ((string)ob->query_short() ? (string)ob->query_short() : "objeto") + \
        " en otra dimensión.\n"

#define _LANG_CMD_CLONE_IN ({ "en", "in" })

// heal

#define _LANG_HEAL_ALIASES ({ "vida" })
#define _LANG_HEAL_MESSAGE_MINE "%^YELLOW%^Tu vida y tu energía se recuperan.%^RESET%^\n"
#define _LANG_HEAL_MESSAGE_OTHERS "%^YELLOW%^La vida y la energía de " + me->query_cap_name() + \
  " se recuperan.%^RESET%^\n"

// peace

#define _LANG_PEACE_MESSAGES_MINE "Alzas tu mano y exclamas: " + \
      "%^YELLOW%^¡Dejad de luchar, mortales!%^RESET%^\n"
#define _LANG_PEACE_MESSAGES_OTHERS this_player()->query_cap_name() + \
        " alza su mano y exclama: %^YELLOW%^¡Dejad de luchar, mortales!" + \
        "%^RESET%^\n"

// snoop

#define _LANG_SNOOP_WATCHED(who) (who) + " comienza a observar tu sesión.\n"
