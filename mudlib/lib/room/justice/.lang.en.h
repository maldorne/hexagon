// Strings for the justice rooms and the location components that share their actions.

// jail-actions.c -- a cell

#define _LANG_JAIL_CMDS_BRIBE ({ "bribe" })
#define _LANG_JAIL_LOCKED "The cell door is still locked: you have $time$ of your sentence left.\n"
#define _LANG_JAIL_NOT_PRISONER "Nobody locked you up here, you have nobody to bribe.\n"
#define _LANG_JAIL_PASSED_OUT "You are unconscious, you cannot do anything.\n"
#define _LANG_JAIL_BRIBE_NO_MONEY "You do not carry what the jailer expects: $cost$ coins.\n"
#define _LANG_JAIL_BRIBE_OK "You slip $cost$ coins to the jailer through the bars. He looks away and the door is left open.\n"
#define _LANG_JAIL_BRIBE_FAILED "The jailer pockets the coins, smiles and shouts that a prisoner tried to bribe him. Your sentence grows longer.\n"

// fines-actions.c -- the office where fines are paid

#define _LANG_FINES_CMDS_OWED ({ "owe", "fines" })
#define _LANG_FINES_CMDS_PAY ({ "pay" })
#define _LANG_FINES_CMDS_BAIL ({ "bail" })
#define _LANG_FINES_NO_LAW "No law rules here, nobody will charge you anything.\n"
#define _LANG_FINES_OWE_NOTHING "The clerk checks his books: you owe nothing.\n"
#define _LANG_FINES_OWE "The clerk checks his books: you have $points$ points of crime, and the fine is $fine$ coins. You can settle it with 'pay'.\n"
#define _LANG_FINES_NO_MONEY "You do not carry enough money: the fine is $fine$ coins.\n"
#define _LANG_FINES_PAID "You pay $fine$ coins. The clerk strikes your name from the book: you are clean.\n"
#define _LANG_FINES_BAIL_USAGE "Usage: bail <player>\n"
#define _LANG_FINES_BAIL_NOBODY "You cannot find anybody by that name.\n"
#define _LANG_FINES_BAIL_NOT_JAILED "That person is not serving a sentence here.\n"
#define _LANG_FINES_BAIL_NOTHING "That person owed nothing any more: they are free.\n"
#define _LANG_FINES_BAIL_NO_MONEY "You do not carry enough money: the bail is $fine$ coins.\n"
#define _LANG_FINES_BAIL_PAID "You pay $fine$ coins of bail for $who$. They will be free at once.\n"

#define _LANG_JAIL_ROOM_SHORT "Cell"
#define _LANG_JAIL_ROOM_LONG "A narrow stone cell, with iron bars instead of a door. Whoever serves a sentence here does not leave until the time is up.\n"

#define _LANG_FINES_ROOM_SHORT "Fines office"
#define _LANG_FINES_ROOM_LONG "An office with a counter and a clerk surrounded by account books. Fines are settled here, and bail is paid for prisoners.\n"
#define _LANG_FINES_SIGN_LONG "A wooden sign nailed above the counter.\n"
