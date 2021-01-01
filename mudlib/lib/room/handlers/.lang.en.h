
// exit_handler

#define _LANG_EXITS_CANNOT_SEE_EXITS "There are no obvious exits"
#define _LANG_EXITS_ONE_EXIT "There is one obvious exit: "
#define _LANG_EXITS_AND " and "
#define _LANG_EXITS_MULTIPLE_EXITS "There are " + query_num(sizeof(dirs)+1, 0) + \
                                   " obvious exits: "

#define _LANG_EXITS_IS_CLOSED "The " + dest_direc[i] + " door is closed.\n"
#define _LANG_EXITS_IS_CLOSED_KNOWN "The " + dest_direc[i] + " door is closed.\n"
#define _LANG_EXITS_IS_CLOSED_CUSTOM "The " + dest_direc[i] + " " + \
                          ((!door->query_number()) ? "is" : "are") + " closed.\n"
