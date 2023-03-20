
// door.c

#define _LANG_DOOR_LONG "Es una puerta.\n"
#define _LANG_DOOR_OPEN_ACTIONS ({ "abrir" })
#define _LANG_DOOR_CLOSE_ACTIONS ({ "cerrar" })
#define _LANG_DOOR_LOCK_ACTIONS ({ })
#define _LANG_DOOR_UNLOCK_ACTIONS ({ })
#define _LANG_DOOR_NOT_DEAD "Estando muerto tienes poco control sobre el mundo material.\n"

#define _LANG_DOOR_IS_LOCKED "La puerta hacia " + dest  + " está cerrada con llave.\n"
#define _LANG_DOOR_IS_LOCKED_KNOWN "La puerta " + dest  + " está cerrada con llave.\n"
#define _LANG_DOOR_IS_LOCKED_CUSTOM capitalize((!number) ? query_article() : query_article_plural()) + " " + \
                                    dest + ((!number) ? " está cerrad" + query_vowel() + " con llave" : \
                                    " están cerrad" + query_vowel() + "s con llave") + ".\n"

#define _LANG_DOOR_USE_TO_UNLOCK "Utilizas tu " + obs[i]->short() + ".\n"

#define _LANG_DOOR_NOT_LOCKED "No está cerrad" + query_vowel() + " con llave.\n"
#define _LANG_DOOR_CANNOT_BE_LOCKED "No puede cerrarse con llave.\n"

#define _LANG_DOOR_OPEN_ALREADY "La puerta hacia " + dest + " ya está abierta.\n"
#define _LANG_DOOR_OPEN_ALREADY_KNOWN "La puerta " + dest + " ya está abierta.\n"
#define _LANG_DOOR_OPEN_ALREADY_CUSTOM capitalize((!number) ? query_article() : query_article_plural()) + \
                                  " " + dest + ((!number) ? (" ya está abiert" + query_vowel()) : \
                                                            (" ya están abiert" + query_vowel() + "s"))

#define _LANG_DOOR_YOU_OPEN "Abres la puerta hacia " + door + ".\n"
#define _LANG_DOOR_YOU_OPEN_KNOWN "Abres la puerta " + door + ".\n"
#define _LANG_DOOR_YOU_OPEN_CUSTOM "Abres " + ((!number) ? query_article() : query_article_plural()) + \
                                   " " + door + ".\n"
#define _LANG_DOOR_PLAYER_OPENS ob->query_cap_name() + " abre la puerta hacia " + door + ".\n"
#define _LANG_DOOR_PLAYER_OPENS_KNOWN ob->query_cap_name() + " abre la puerta " + door + ".\n"
#define _LANG_DOOR_PLAYER_OPENS_CUSTOM ob->query_cap_name() + " abre " + \
                                   ((!number) ? query_article() : query_article_plural()) + \
                                   " " + door + ".\n"
#define _LANG_DOOR_SOMEBODY_OPENS "Alguien abre la puerta hacia " + door + " desde el otro lado.\n"
#define _LANG_DOOR_SOMEBODY_OPENS_KNOWN "Alguien abre la puerta " + door + " desde el otro lado.\n"
#define _LANG_DOOR_SOMEBODY_OPENS_CUSTOM "Alguien abre " + \
                                   ((!number) ? query_article() : query_article_plural()) + \
                                   " " + door + " desde el otro lado.\n"

#define _LANG_DOOR_BROKEN "La puerta hacia " + dest + " está rota.\n"
#define _LANG_DOOR_BROKEN_KNOWN "La puerta " + dest + " está rota.\n"
#define _LANG_DOOR_BROKEN_CUSTOM capitalize((!number) ? query_article() : query_article_plural()) + \
                                   " " + dest + ((!number) ? "está rot" + query_vowel() : \
                                   "están rot" + query_vowel() + "s") + ".\n"

#define _LANG_DOOR_VERB_TO_LOCK_USED ((sizeof(list) >= 3) && (list[1] == "con") && (list[2] == "llave"))

#define _LANG_DOOR_LOCK_ALREADY "La puerta hacia " + dest + " ya está cerrada con llave.\n"
#define _LANG_DOOR_LOCK_ALREADY_KNOWN "La puerta " + dest + " ya está cerrada con llave.\n"
#define _LANG_DOOR_LOCK_ALREADY_CUSTOM capitalize((!number) ? query_article() : query_article_plural()) + \
                                   " " + dest + ((!number) ? (" ya está cerrad" + query_vowel()) : \
                                   (" ya están cerrad" + query_vowel() + "s")) + " con llave.\n"

#define _LANG_DOOR_NEED_A_KEY "No tienes la llave necesaria.\n"
#define _LANG_DOOR_USE_TO_LOCK "Utilizas tu " + obs[i]->short() + ".\n"

#define _LANG_DOOR_OTHERS_LOCK "Escuchas un chasquido en la cerradura de la puerta hacia " + dest + ".\n"
#define _LANG_DOOR_OTHERS_LOCK_KNOWN "Escuchas un chasquido en la cerradura de la puerta " + dest + ".\n"
#define _LANG_DOOR_OTHERS_LOCK_CUSTOM "Escuchas un chasquido en la cerradura de " + \
                                   ((!number) ? query_article() : query_article_plural()) + " " + dest + ".\n"

#define _LANG_DOOR_OTHER_SIDE_LOCK "Escuchas un chasquido en la cerradura de la puerta hacia " + dir_other_side + ".\n"
#define _LANG_DOOR_OTHER_SIDE_LOCK_KNOWN "Escuchas un chasquido en la cerradura de la puerta " + dir_other_side + ".\n"
#define _LANG_DOOR_OTHER_SIDE_LOCK_CUSTOM "Escuchas un chasquido en la cerradura de " + \
                                   ((!number)? query_article() : query_article_plural()) + \
                                   " " + dir_other_side + ".\n"

#define _LANG_DOOR_CLOSED_ALREADY "La puerta hacia " + dest + " ya está cerrada.\n"
#define _LANG_DOOR_CLOSED_ALREADY_KNOWN "La puerta " + dest + " ya está cerrada.\n"
#define _LANG_DOOR_CLOSED_ALREADY_CUSTOM capitalize((!number) ? query_article() : query_article_plural()) + \
                                  " " + dest + ((!number) ? (" ya está cerrad" + query_vowel()) : \
                                                            (" ya están cerrad" + query_vowel() + "s"))

#define _LANG_DOOR_YOU_CLOSE "Cierras la puerta hacia " + door + ".\n"
#define _LANG_DOOR_YOU_CLOSE_KNOWN "Cierras la puerta " + door + ".\n"
#define _LANG_DOOR_YOU_CLOSE_CUSTOM "Cierras " + ((!number) ? query_article() : query_article_plural()) + \
                                   " " + door + ".\n"
#define _LANG_DOOR_PLAYER_CLOSES ob->query_cap_name() + " cierra la puerta hacia " + door + ".\n"
#define _LANG_DOOR_PLAYER_CLOSES_KNOWN ob->query_cap_name() + " cierra la puerta " + door + ".\n"
#define _LANG_DOOR_PLAYER_CLOSES_CUSTOM ob->query_cap_name() + " cierra " + \
                                   ((!number) ? query_article() : query_article_plural()) + \
                                   " " + door + ".\n"
#define _LANG_DOOR_SOMEBODY_CLOSES "Alguien cierra la puerta hacia " + door + " desde el otro lado.\n"
#define _LANG_DOOR_SOMEBODY_CLOSES_KNOWN "Alguien cierra la puerta " + door + " desde el otro lado.\n"
#define _LANG_DOOR_SOMEBODY_CLOSES_CUSTOM "Alguien cierra " + \
                                   ((!number) ? query_article() : query_article_plural()) + \
                                   " " + door + " desde el otro lado.\n"

#define _LANG_DOOR_BREAKS "La puerta hacia " + dest + " se rompe en mil pedazos.\n"
#define _LANG_DOOR_BREAKS_KNOWN "La puerta " + dest + " se rompe en mil pedazos.\n"
#define _LANG_DOOR_BREAKS_CUSTOM capitalize((!number) ? query_article() : query_article_plural()) + \
                                   " " + dest + " se rompe" + ((!number) ? "" : "n")+" en mil pedazos.\n"

#define _LANG_DOOR_HEALTH_STATUSES ({ "En perfecto estado", "Un poco estropeada", \
                                   "No está en buen estado", "En mal estado", \
                                   "Casi no se tiene en pie", "Está rota", })

#define _LANG_DOOR_OPENS "La puerta hacia " + dest + " se abre lentamente.\n"
#define _LANG_DOOR_OPENS_KNOWN "La puerta " + dest + " se abre lentamente.\n"
#define _LANG_DOOR_OPENS_CUSTOM capitalize((!number) ? query_article() : query_article_plural()) +  " " + \
                        dest + " se abre" + ((!number) ? "" : "n") + " lentamente.\n"
#define _LANG_DOOR_CLOSES "La puerta hacia " + dest + " se cierra lentamente.\n"
#define _LANG_DOOR_CLOSES_KNOWN "La puerta " + dest + " se cierra lentamente.\n"
#define _LANG_DOOR_CLOSES_CUSTOM capitalize((!number) ? query_article() : query_article_plural()) +  " " + \
                        dest + " se cierra" + ((!number) ? "" : "n") + " lentamente.\n"

// item.c

#define _LANG_ITEM_NOTHING_IMPORTANT "No ves nada destacable.\n"
#define _LANG_ITEM_ERROR "Error en el objeto, comunícaselo a un programador.\n"
