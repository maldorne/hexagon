
// user.c

#define _LANG_DISCONNECTED "\nDesconectado.\n"
#define _LANG_LAST_CONNECTION "Tu última conexión fue desde '"+last_on_from+"'.\n"
#define _LANG_USER_COMMAND_TOO_LONG "Comando demasiado largo - procesando de todas formas.\n"

// monster.c

#define _LANG_MONSTER_CANNOT_CLONE query_cap_name() + " intenta sostener su " + ob->query_name() + \
              " pero finalmente tiene que soltarlo.\n"
