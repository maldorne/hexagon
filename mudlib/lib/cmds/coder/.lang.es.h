
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

#define _LANG_PEACE_ALIASES ({ "paz", "peace" })
#define _LANG_PEACE_MESSAGES_MINE "Alzas tu mano y exclamas: " + \
      "%^YELLOW%^¡Dejad de luchar, mortales!%^RESET%^\n"
#define _LANG_PEACE_MESSAGES_OTHERS this_player()->query_cap_name() + \
        " alza su mano y exclama: %^YELLOW%^¡Dejad de luchar, mortales!" + \
        "%^RESET%^\n"

// snoop

#define _LANG_SNOOP_WATCHED(who) (who) + " comienza a observar tu sesión.\n"

// uptime

#define _LANG_CMD_UPTIME_MESSAGE "El mud lleva funcionando " + \
  seconds_to_time_string(uptime()) + ".\n"

// free-title

#define _LANG_CMD_FREE_TITLE_ALIASES ({ "titulolibre", "títulolibre", "freetitle" })
#define _LANG_CMD_FREE_TITLE_SYNTAX "titulolibre <texto>\n" + \
                                    "          titulolibre -c"
#define _LANG_CMD_FREE_TITLE_HELP "Escribe a mano el título que luces, o bórralo con -c."
#define _LANG_CMD_FREE_TITLE_SHOW "Tu título escrito a mano es: " + me->query_manual_title() + "\n"
#define _LANG_CMD_FREE_TITLE_NONE "No tienes ningún título escrito a mano.\n"
#define _LANG_CMD_FREE_TITLE_CLEARED "Borrado tu título escrito a mano.\n"
#define _LANG_CMD_FREE_TITLE_SET "Ahora luces el título '" + me->query_title() + "'.\n"
