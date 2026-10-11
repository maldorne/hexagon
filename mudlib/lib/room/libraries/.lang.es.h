// libraries. `book` is the book a player hands in or takes, `title` its title

#define _LANG_LIBRARY_ROOM_SHORT "Biblioteca"
#define _LANG_LIBRARY_ROOM_LONG "Una sala de estanterías llenas de libros.\n"

#define _LANG_LIBRARY_CMDS_LIST ({ "listar" })
#define _LANG_LIBRARY_CMDS_TAKE ({ "recuperar" })
#define _LANG_LIBRARY_CMDS_LEAVE ({ "depositar" })

#define _LANG_LIBRARY_SIGN_LONG "Es un pequeño cartel de madera colgado junto a las estanterías.\n"
#define _LANG_LIBRARY_SIGN_TEXT "Biblioteca\n\n" + \
        "  listar               los libros de las estanterías\n" + \
        "  leer <número>        lee uno sin sacarlo de aquí\n" + \
        "  recuperar <número>   te llevas una copia\n" + \
        "  depositar <libro>    dejas uno tuyo en las estanterías\n\n" + \
        "Cada libro que leas hasta el final queda anotado en la enciclopedia " + \
        "de tu diario."
#define _LANG_LIBRARY_SHELF_DESC "Una estantería de madera llena de libros."
#define _LANG_LIBRARY_EMPTY "Las estanterías están vacías.\n"
#define _LANG_LIBRARY_LIST_HEADER "En las estanterías ves estos libros:\n\n"
#define _LANG_LIBRARY_LIST_FOOTER "\nUsa 'leer <número>' para leer uno aquí, o " + \
        "'recuperar <número>' para llevarte una copia.\n"
#define _LANG_LIBRARY_DAMAGED "Un libro en mal estado"
#define _LANG_LIBRARY_WHICH "¿Qué libro? Usa 'listar' para ver la lista y 'recuperar <número>'.\n"
#define _LANG_LIBRARY_CANNOT_CARRY "No puedes llevar nada más.\n"
#define _LANG_LIBRARY_TAKEN "Coges de las estanterías una copia de " + book->query_short() + ".\n"
#define _LANG_LIBRARY_TAKEN_OTHERS this_player()->query_cap_name() + " coge un libro de las estanterías.\n"
#define _LANG_LIBRARY_LEAVE_WHAT "¿Depositar qué libro?\n"
#define _LANG_LIBRARY_ONLY_BOOKS "En las estanterías solo se pueden dejar libros.\n"
#define _LANG_LIBRARY_ALREADY_THERE "Ese libro ya está en las estanterías.\n"
#define _LANG_LIBRARY_LEFT "Dejas " + book->query_short() + " en las estanterías.\n"
#define _LANG_LIBRARY_LEFT_OTHERS this_player()->query_cap_name() + " deja un libro en las estanterías.\n"
