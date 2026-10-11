// libraries. `book` is the book a player hands in or takes, `title` its title

#define _LANG_LIBRARY_ROOM_SHORT "Library"
#define _LANG_LIBRARY_ROOM_LONG "A room of shelves full of books.\n"

#define _LANG_LIBRARY_CMDS_LIST ({ "list" })
#define _LANG_LIBRARY_CMDS_TAKE ({ "borrow" })
#define _LANG_LIBRARY_CMDS_LEAVE ({ "deposit" })

#define _LANG_LIBRARY_SIGN_LONG "A small wooden sign hangs beside the shelves.\n"
#define _LANG_LIBRARY_SIGN_TEXT "Library\n\n" + \
        "  list                 the books on the shelves\n" + \
        "  read <number>        read one without taking it away\n" + \
        "  borrow <number>      take a copy with you\n" + \
        "  deposit <book>       leave one of yours on the shelves\n\n" + \
        "Every book you read to the end is noted down in your diary's " + \
        "encyclopedia."
#define _LANG_LIBRARY_SHELF_DESC "A wooden shelf full of books."
#define _LANG_LIBRARY_EMPTY "The shelves are empty.\n"
#define _LANG_LIBRARY_LIST_HEADER "On the shelves you see these books:\n\n"
#define _LANG_LIBRARY_LIST_FOOTER "\nUse 'read <number>' to read one here, or " + \
        "'borrow <number>' to take a copy with you.\n"
#define _LANG_LIBRARY_DAMAGED "A book in poor condition"
#define _LANG_LIBRARY_WHICH "Which book? Use 'list' to see the list and 'borrow <number>'.\n"
#define _LANG_LIBRARY_CANNOT_CARRY "You cannot carry anything else.\n"
#define _LANG_LIBRARY_TAKEN "You take a copy of \"" + book->query_title() + "\" from the shelves.\n"
#define _LANG_LIBRARY_TAKEN_OTHERS this_player()->query_cap_name() + " takes a book from the shelves.\n"
#define _LANG_LIBRARY_LEAVE_WHAT "Deposit which book?\n"
#define _LANG_LIBRARY_ONLY_BOOKS "Only books can be left on the shelves.\n"
#define _LANG_LIBRARY_ALREADY_THERE "That book is already on the shelves.\n"
#define _LANG_LIBRARY_LEFT "You leave \"" + book->query_title() + "\" on the shelves.\n"
#define _LANG_LIBRARY_LEFT_OTHERS this_player()->query_cap_name() + " leaves a book on the shelves.\n"
