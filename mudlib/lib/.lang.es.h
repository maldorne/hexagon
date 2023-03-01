
// user.c

#define _LANG_DISCONNECTED "Desconectado.\n"
#define _LANG_LAST_CONNECTION "Tu última conexión fue desde '"+last_on_from+"'.\n"
#define _LANG_USER_COMMAND_TOO_LONG "Comando demasiado largo - procesando de todas formas.\n"

// monster.c

#define _LANG_MONSTER_CANNOT_CLONE query_cap_name() + " intenta sostener su " + ob->query_name() + \
              " pero finalmente tiene que soltarlo.\n"

// room.c

#define _LANG_DEFAULT_SIGN_NAME "cartel"

// language.c

#define _LANG_LANGUAGE_DEFAULT_DESC "Es el lenguaje habitual, hablado por la mayoría.\n"

// player.c

#define _LANG_PLAYER_SAVING "Salvando...\n"
#define _LANG_PLAYER_NO_GUEST_SAVE "Ups, l"+G_CHAR+"s invitad"+G_CHAR+"s no pueden salvar...\n"
#define _LANG_PLAYER_SAVE_ACTIONS ({ "salvar", "grabar" })
#define _LANG_PLAYER_NOT_IN_CONDITIONS "No estás en condiciones de salir corriendo.\n"
#define _LANG_PLAYER_SEE_FEET_RUNNING "Ves a tus pies correr ante ti.\n"
#define _LANG_PLAYER_FAILED_ESCAPE "Intentaste escapar, pero no has encontrado ninguna salida.\n"

// item.c

#define _LANG_ITEM_BREAKS "¡Tu " + query_name() + " se rompe en mil pedazos!\n"

// armour.c

#define _LANG_DESC_IS_SET_PIECE "   Este objeto forma parte del conjunto: " + \
              main_piece->query_piece_set_name() + ".\n"
#define _LANG_DESC_IS_MAIN_SET_PIECE "   Este objeto es la pieza central del conjunto '" + \
              main_piece->query_piece_set_name() + "', compuesto de "
