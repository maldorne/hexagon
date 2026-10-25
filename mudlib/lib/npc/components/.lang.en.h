// Strings for the NPC components under this directory.

// raise.c -- the service of returning a ghost to its body

#define _LANG_RAISE_INFO_VERBS ({ "info", "information" })
#define _LANG_RAISE_VERBS ({ "raise", "raiseme" })

#define _LANG_RAISE_INFO "Uhmm, if you truly wish to return to your mortal form, " + \
    "you have only to ask: write 'raise'."
#define _LANG_RAISE_NOT_DEAD "Why do you want me to raise you if you don't need it?\n"
#define _LANG_RAISE_REFUSED "Your soul is not pure enough for me to bring you back to life.\n"
#define _LANG_RAISE_TOO_SOON "You were raised recently, you will have to wait a bit more.\n"
#define _LANG_RAISE_GESTURE "Hands are raised calling on the powers that bind " + \
    "soul to body.\n"

// toll
#define _LANG_TOLL_VERBS ({ "pay" })
#define _LANG_TOLL_BLOCKED "Hey, " + who->query_cap_name() + ", if you want to cross, pay!"
#define _LANG_TOLL_EXEMPT "You do not have to pay anything, go through whenever you like."
#define _LANG_TOLL_ALREADY_PAID "Thank you, but you have already paid."
#define _LANG_TOLL_NO_MONEY "Hey, you! Are you trying to make a fool of me? You do not have enough money."
#define _LANG_TOLL_PAID "Thank you very much, the way is open for you."

// guard
#define _LANG_GUARD_WANTED_BLOCKED "Halt! You are wanted by the law, you will not pass."
#define _LANG_GUARD_WANTED_SHOUT "Halt, in the name of the law!"

// gift
#define _LANG_GIFT_BUSY "$npc$ is in the middle of a fight and cannot attend to you right now."
