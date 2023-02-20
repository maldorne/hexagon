
// exit_handler

#define _LANG_EXITS_CANNOT_SEE_EXITS "No puedes ver ninguna salida"
#define _LANG_EXITS_ONE_EXIT "Puedes ver una salida: "
#define _LANG_EXITS_AND " y "
#define _LANG_EXITS_MULTIPLE_EXITS "Puedes ver " + query_num(sizeof(dirs)+1, 0) + \
                                   " salidas: "

#define _LANG_EXITS_IS_CLOSED "La puerta hacia " + dest_direc[i] + " está cerrada.\n"
#define _LANG_EXITS_IS_CLOSED_KNOWN "La puerta " + dest_direc[i] + " se cierra lentamente.\n"
#define _LANG_EXITS_IS_CLOSED_CUSTOM capitalize((!door->query_number()) ? door->query_article() : \
              door->query_article_plural()) + " " + dest_direc[i] + \
              " está"+((!door->query_number()) ? "" : "n") + \
              (!door->query_number()) ? (" cerrad"+door->query_vowel()+".\n") : \
              (" cerrad"+door->query_vowel()+"s.\n")
