// door.c

// What a door is called when it is given no name of its own, and its gender.
#define _LANG_DOOR_DEFAULT_NAME "puerta"
#define _LANG_DOOR_DEFAULT_GENDER 2

// The door named with its way, without article; `dir` in scope. Up, down, in and
// out read "hacia arriba", the compass "este".
#define _LANG_DOOR_NOUN door_name + (reset_msg ? " hacia " : " ") + dir
#define _LANG_DOOR_NOUN_FOR(d) (door_name + " " + d)

// with an article, capitalised, and how it agrees with "está" in number and gender
#define _LANG_DOOR_THE(d) capitalize(query_door_phrase(d))
#define _LANG_DOOR_IS ((!number) ? "está" : "están")
#define _LANG_DOOR_AGREE (query_vowel() + ((!number) ? "" : "s"))
#define _LANG_DOOR_N ((!number) ? "" : "n")
#define _LANG_DOOR_OF(d) ((query_door_phrase(d)[0..2] == "el ") ? \
                          "del " + query_door_phrase(d)[3..] : "de " + query_door_phrase(d))

#define _LANG_DOOR_LONG "Es una puerta.\n"
#define _LANG_DOOR_OPEN_ACTIONS ({ "abrir" })
#define _LANG_DOOR_CLOSE_ACTIONS ({ "cerrar" })
#define _LANG_DOOR_LOCK_ACTIONS ({ })
#define _LANG_DOOR_UNLOCK_ACTIONS ({ })
#define _LANG_DOOR_NOT_DEAD "Estando muerto tienes poco control sobre el mundo material.\n"

#define _LANG_DOOR_IS_CLOSED _LANG_DOOR_THE(dest) + " " + _LANG_DOOR_IS + " cerrad" + _LANG_DOOR_AGREE + ".\n"
#define _LANG_DOOR_IS_LOCKED _LANG_DOOR_THE(dest) + " " + _LANG_DOOR_IS + " cerrad" + _LANG_DOOR_AGREE + " con llave.\n"

#define _LANG_DOOR_USE_TO_UNLOCK "Utilizas tu " + obs[i]->short() + ".\n"

#define _LANG_DOOR_NOT_LOCKED "No está cerrad" + query_vowel() + " con llave.\n"
#define _LANG_DOOR_CANNOT_BE_LOCKED "No puede cerrarse con llave.\n"

#define _LANG_DOOR_OPEN_ALREADY _LANG_DOOR_THE(dest) + " ya " + _LANG_DOOR_IS + " abiert" + _LANG_DOOR_AGREE + ".\n"

#define _LANG_DOOR_YOU_OPEN "Abres " + query_door_phrase(door) + ".\n"
#define _LANG_DOOR_PLAYER_OPENS ob->query_cap_name() + " abre " + query_door_phrase(door) + ".\n"
#define _LANG_DOOR_SOMEBODY_OPENS "Alguien abre " + query_door_phrase(door) + " desde el otro lado.\n"

#define _LANG_DOOR_BROKEN _LANG_DOOR_THE(dest) + " " + _LANG_DOOR_IS + " rot" + _LANG_DOOR_AGREE + ".\n"

#define _LANG_DOOR_VERB_TO_LOCK_USED ((sizeof(list) >= 3) && (list[1] == "con") && (list[2] == "llave"))

#define _LANG_DOOR_LOCK_ALREADY _LANG_DOOR_THE(dest) + " ya " + _LANG_DOOR_IS + " cerrad" + _LANG_DOOR_AGREE + " con llave.\n"

#define _LANG_DOOR_NEED_A_KEY "No tienes la llave necesaria.\n"
#define _LANG_DOOR_USE_TO_LOCK "Utilizas tu " + obs[i]->short() + ".\n"

#define _LANG_DOOR_OTHERS_LOCK "Escuchas un chasquido en la cerradura " + _LANG_DOOR_OF(dest) + ".\n"
#define _LANG_DOOR_OTHER_SIDE_LOCK "Escuchas un chasquido en la cerradura " + _LANG_DOOR_OF(dir_other_side) + ".\n"

#define _LANG_DOOR_CLOSED_ALREADY _LANG_DOOR_THE(dest) + " ya " + _LANG_DOOR_IS + " cerrad" + _LANG_DOOR_AGREE + ".\n"

#define _LANG_DOOR_YOU_CLOSE "Cierras " + query_door_phrase(door) + ".\n"
#define _LANG_DOOR_PLAYER_CLOSES ob->query_cap_name() + " cierra " + query_door_phrase(door) + ".\n"
#define _LANG_DOOR_SOMEBODY_CLOSES "Alguien cierra " + query_door_phrase(door) + " desde el otro lado.\n"

#define _LANG_DOOR_BREAKS _LANG_DOOR_THE(dest) + " se rompe" + _LANG_DOOR_N + " en mil pedazos.\n"

#define _LANG_DOOR_HEALTH_STATUSES ({ "En perfecto estado", "Un poco estropeada", \
                                   "No está en buen estado", "En mal estado", \
                                   "Casi no se tiene en pie", "Está rota", })

#define _LANG_DOOR_OPENS _LANG_DOOR_THE(dest) + " se abre" + _LANG_DOOR_N + " lentamente.\n"
#define _LANG_DOOR_CLOSES _LANG_DOOR_THE(dest) + " se cierra" + _LANG_DOOR_N + " lentamente.\n"

// item.c

#define _LANG_ITEM_NOTHING_IMPORTANT "No ves nada destacable.\n"
#define _LANG_ITEM_ERROR "Error en el objeto, comunícaselo a un programador.\n"

#define _LANG_DOOR_OPEN_AS_FAMILY "Eres de la casa: la puerta te reconoce.\n"
