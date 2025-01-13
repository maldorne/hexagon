
// user.c

#define _LANG_DISCONNECTED "Desconectado.\n"
#define _LANG_LAST_CONNECTION "Tu última conexión fue desde '"+last_on_from+"'.\n"
#define _LANG_USER_COMMAND_TOO_LONG "Comando demasiado largo - procesando de todas formas.\n"
#define _LANG_USER_PLAYER_RESTORE_ERROR "Tu avatar ha sido destruido y no ha podido ser restaurado.\n" + \
                        "Tu conexión será terminada para que puedas iniciar sesión de nuevo.\n"

// monster.c

#define _LANG_MONSTER_CANNOT_CLONE query_cap_name() + " intenta sostener su " + ob->query_name() + \
              " pero finalmente tiene que soltarlo.\n"

// room.c

#define _LANG_DEFAULT_SIGN_NAME "cartel"

#define _LANG_ROOM_TOO_DARK "Está demasiado oscuro para poder ver"
#define _LANG_ROOM_LIGHT_DEF "Hmm, no ves bien. " + dark_mess + "\n"
#define _LANG_ROOM_LIGHT_2 "No puedes ver mucho.\n"+ exit_string
#define _LANG_ROOM_LIGHT_3 "Es demasiado difícil ver en esta oscuridad.\n"+::short(1)+".\n"+exit_string
#define _LANG_ROOM_LIGHT_4 "Estás deslumbrado por la luz.\n"+::short(0)+".\n"+exit_string
#define _LANG_ROOM_LIGHT_5 "La luz es realmente cegadora.\n"+ exit_string
#define _LANG_ROOM_LIGHT_6 "Estás cegado por la luz.\n"

// language.c

#define _LANG_LANGUAGE_DEFAULT_DESC "Es el lenguaje habitual, hablado por la mayoría.\n"

// player.c

#define _LANG_PLAYER_SAVING "Salvando...\n"
#define _LANG_PLAYER_NO_GUEST_SAVE "Ups, l"+G_CHAR+"s invitad"+G_CHAR+"s no pueden salvar...\n"
#define _LANG_PLAYER_SAVE_ACTIONS ({ "salvar", "grabar" })
#define _LANG_PLAYER_PLURAL "personas que se parecen a " + str

// item.c

#define _LANG_ITEM_BREAKS "¡Tu " + query_name() + " se rompe en mil pedazos!\n"

// armour.c

#define _LANG_DESC_IS_SET_PIECE "   Este objeto forma parte del conjunto: " + \
              main_piece->query_piece_set_name() + ".\n"
#define _LANG_DESC_IS_MAIN_SET_PIECE "   Este objeto es la pieza central del conjunto '" + \
              main_piece->query_piece_set_name() + "', compuesto de "

// weapon.c

#define _LANG_WEAPON_FUMBLE_MSG "¡Oh, qué torpeza!\n"
#define _LANG_WEAPON_CRITICAL_MSG "¡Oh, qué habilidad!\n"

#define _LANG_WEAPON_DODGE_MSG_ATTACKER " logra esquivar tu ataque.\n"
#define _LANG_WEAPON_DODGE_MSG_DEFENDER " intenta golpearte pero logras esquivar su ataque.\n"
#define _LANG_WEAPON_DODGE_MSG_ROOM " trata de golpear a "
#define _LANG_WEAPON_DODGE_MSG_ROOM_END " pero "+defender->query_demonstrative()+" logra esquivar su ataque.\n"
#define _LANG_WEAPON_FUMBLE_MSG_ATTACKER "Te haces un lío con tu arma y no consigues golpear a "
#define _LANG_WEAPON_FUMBLE_MSG_DEFENDER " se hace un lío con su arma y no consigue golpearte.\n"
#define _LANG_WEAPON_FUMBLE_MSG_ROOM " se hace un lío con su arma y no consigue golpear a "
#define _LANG_WEAPON_PARRY_MSG_ATTACKER " bloquea tu ataque con su "
#define _LANG_WEAPON_PARRY_MSG_DEFENDER " intenta golpearte pero logras bloquear su ataque con tu "
#define _LANG_WEAPON_PARRY_MSG_ROOM " trata de golpear a "
#define _LANG_WEAPON_PARRY_MSG_ROOM_END " pero "+defender->query_demonstrative()+" bloquea el ataque con su "
#define _LANG_WEAPON_NO_HIT "No consigues golpear a "
#define _LANG_WEAPON_NOT_HIT " no consigue golpearte."
#define _LANG_WEAPON_NO_HIT_ROOM " no consigue golpear a "
