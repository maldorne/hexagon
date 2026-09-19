
// user.c

#define _LANG_DISCONNECTED "Desconectado.\n"
#define _LANG_IDLE_KICKED "Llevas demasiado tiempo inactivo; desconectando.\n"
#define _LANG_LAST_CONNECTION "Tu última conexión fue desde '"+last_on_from+"'.\n"
#define _LANG_USER_COMMAND_TOO_LONG "Comando demasiado largo - procesando de todas formas.\n"
#define _LANG_USER_PLAYER_RESTORE_ERROR "Tu avatar ha sido destruido y no ha podido ser restaurado.\n" + \
                        "Tu conexión será terminada para que puedas iniciar sesión de nuevo.\n"

// monster.c

#define _LANG_MONSTER_CANNOT_CLONE query_cap_name() + " intenta sostener su " + ob->query_name() + \
              " pero finalmente tiene que soltarlo.\n"

// room.c

// location.c

// Cabecera del banner (solo visible a coders) que lista los
// componentes atachados a la location. Seguida por los tipos de
// componente separados por comas.
#define _LANG_LOCATION_COMPONENTS_HEADER "Componentes"

// language.c

#define _LANG_LANGUAGE_DEFAULT_DESC "Es el lenguaje habitual, hablado por la mayoría.\n"

// player.c

#define _LANG_PLAYER_SAVING "Salvando...\n"
#define _LANG_PLAYER_SAVE_ACTIONS ({ "salvar", "grabar" })
#define _LANG_PLAYER_PLURAL "personas que se parecen a " + str

// item.c

#define _LANG_ITEM_BREAKS "¡Tu " + query_name() + " se rompe en mil pedazos!\n"
#define _LANG_ITEM_HAS_HELP "Este objeto tiene un mensaje de ayuda disponible.\n"

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

// food.c

#define _LANG_FOOD_EAT_VERBS ({ "comer" })
#define _LANG_FOOD_DRINK_VERBS ({ "beber" })
#define _LANG_FOOD_CONSUME_WHAT "¿" + capitalize(query_verb()) + " el qué?\n"
#define _LANG_FOOD_DEFAULT_EAT_ME "Comes un poco de " + query_name() + ".\n"
#define _LANG_FOOD_DEFAULT_EAT_OTHERS "come un poco de " + query_name() + ".\n"
#define _LANG_FOOD_DEFAULT_DRINK_ME "Bebes un trago de " + query_name() + ".\n"
#define _LANG_FOOD_DEFAULT_DRINK_OTHERS "bebe un trago de " + query_name() + ".\n"
#define _LANG_FOOD_SPOILED_ME "Está en mal estado, y el estómago se te revuelve.\n"
#define _LANG_FOOD_FINISHED "No queda nada de " + query_name() + ".\n"
#define _LANG_FOOD_ROTTED_HOLDER "Lo que queda de " + query_name() + " se deshace entre tus dedos.\n"
#define _LANG_FOOD_ROTTED_HOLDER_ROOM "tira lo que queda de " + query_name() + ", ya podrido.\n"
#define _LANG_FOOD_ROTTED_ROOM "Lo que queda de " + query_name() + " acaba de pudrirse del todo.\n"
#define _LANG_FOOD_COND_FRESH "Está recién hech" + query_vowel() + ".\n"
#define _LANG_FOOD_COND_PASSABLE "Todavía está en buen estado.\n"
#define _LANG_FOOD_COND_STALE "Está a punto de echarse a perder.\n"
#define _LANG_FOOD_COND_SPOILED "Está echad" + query_vowel() + " a perder.\n"
#define _LANG_FOOD_LONG_CONDITION "   " + cond_string()
#define _LANG_FOOD_LONG_BITES "   Da para " + number_as_string(query_bites_left()) + \
  ((query_bites_left() == 1) ? " bocado más" : " bocados más") + ".\n"
