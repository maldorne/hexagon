
// hearthstone.c

#define _LANG_HEARTHSTONE_NAME "hearthstone"
#define _LANG_HEARTHSTONE_SHORT "%^MAGENTA%^Hearthstone%^RESET%^"
#define _LANG_HEARTHSTONE_ALIAS ({ "stone" })
#define _LANG_HEARTHSTONE_PLURAL "%^MAGENTA%^Hearthstones%^RESET%^"
#define _LANG_HEARTHSTONE_PLURALS ({ "stones", "Hearthstones" })
#define _LANG_HEARTHSTONE_LONG "It is a small stone, purple in color, with a " + \
    "strange rune carved on one of its sides that you cannot decipher. A " + \
    "whitish aura surrounds it without emitting light, giving you the " + \
    "feeling that it is a beautiful object. It certainly seems to possess " + \
    "powers beyond your comprehension. Type 'help hearthstone' to see what " + \
    "you can do with it.\n"
#define _LANG_HEARTHSTONE_MARK_INFO (strlen(destination_name) ? "Your stone is marked in: " + \
    destination_name + ".\n" : "Your stone is not marked yet.\n")
#define _LANG_HEARTHSTONE_TRANSPORT_VERBS ({ "transport" })
#define _LANG_HEARTHSTONE_MARK_VERBS ({ "mark" })
#define _LANG_HEARTHSTONE_INVOKE_VERBS ({ "invoke" })

#define _LANG_HEARTHSTONE_ACTING "The stone is acting, wait a moment.\n"
#define _LANG_HEARTHSTONE_DEAD "You cannot do that while you are dead.\n"
#define _LANG_HEARTHSTONE_COMBATS "You must finish your combats first.\nType '%^BOLD%^stop combats%^RESET%^' to end them as soon as possible.\n"
#define _LANG_HEARTHSTONE_LOCKED "The stone has not accumulated enough energy since the last time.\n"
#define _LANG_HEARTHSTONE_FORGOTTEN "Your stone has forgotten the way home... you will have to mark a new place before trying again.\n"
#define _LANG_HEARTHSTONE_ORIGIN "Your stone has forgotten the way home... but it still remembers its initial origin.\n"
#define _LANG_HEARTHSTONE_MSG_ME "The hearthstone understands your desires and its powers activate.\n"
#define _LANG_HEARTHSTONE_MSG_ROOM "The hearthstone of "+this_player()->query_cap_name()+" begins to glow with purple tones.\n"

#define _LANG_HEARTHSTONE_MOVE "If you don't stop moving, it will be impossible for the stone " + \
    "to accumulate enough energy to transport you.\n"
#define _LANG_HEARTHSTONE_DEAD2 "When you die, your stone releases the accumulated energy...\n"
#define _LANG_HEARTHSTONE_ERROR "Something went wrong.\n"
#define _LANG_HEARTHSTONE_TRANSPORT_ME "You are blinded by a powerful beam of purple light " + \
    "coming from the stone, while you feel it warming to the touch.\n\nYou " + \
    "feel the heat of the stone disappear and as the beam of light fades, " + \
    "you can tell where you are.\n\n"
#define _LANG_HEARTHSTONE_TRANSPORT_ROOM " suddenly appears from within " + \
    "a strange purple smoke.\n"

#define _LANG_HEARTHSTONE_MARK_FAIL "Mark what? Maybe you want to 'mark hearthstone'.\n"
#define _LANG_HEARTHSTONE_NOT_INN_FAIL "You can only do that in a tavern.\n"
#define _LANG_HEARTHSTONE_DESTINATION "Ok, from now on the destination of your hearthstone will be: "
#define _LANG_HEARTHSTONE_HELP "You can use the following commands:\n\n" + \
          "\tmark            - in a tavern so your hearthstone remembers the place.\n" + \
          "\ttransport       - so the hearthstone transports you back to the marked place.\n" + \
          "\tinvoke mount    - to bring your mount to your side (only outdoors).\n"

#define _LANG_HEARTHSTONE_INVOKE_NAMES ({ "mount" })
#define _LANG_HEARTHSTONE_INVOKE_WHAT "Invoke what? Maybe you mean 'invoke mount'.\n"
#define _LANG_HEARTHSTONE_INVOKE_NO_MOUNT "You need to have a mount for that.\n"
#define _LANG_HEARTHSTONE_INVOKE_OUTSIDE "You can only do that outdoors.\n"
#define _LANG_HEARTHSTONE_INVOKE_NO_WATER "Doing that in the water is not recommended.\n"
#define _LANG_HEARTHSTONE_INVOKE_MOUNT_HERE "Your mount is already here.\n"
#define _LANG_HEARTHSTONE_INVOKE_MSG_ME "Your mount appears by your side.\n"
#define _LANG_HEARTHSTONE_INVOKE_MSG_ROOM "The mount of "+this_player()->query_cap_name()+" appears by their side.\n"

// corpse.c

#define _LANG_CORPSE_SOMEBODY "somebody"
#define _LANG_CORPSE_NAME "corpse"
#define _LANG_CORPSE_ALIASES ({ "corpse", "body", "remains" })
#define _LANG_CORPSE_PLURALS ({ "corpses", "bodies", "remains" })
#define _LANG_CORPSE_DESC "It is a corpse.\n"

#define _LANG_CORPSE_OF "corpse of "
#define _LANG_CORPSES_OF "corpses of "
#define _LANG_CORPSE_DESC_OF "This is the dead body of "+capitalize(n)+".\n"
#define _LANG_CORPSE_RAISE_MSG "You hear the howling of a tortured spirit.\n" + \
    "Suddenly, a " + cow->query_name() + " rises from the dead.\n"
#define _LANG_CORPSE_UNKNOWN_BEIGN "unknown beign"
#define _LANG_CORPSE_REMAINS "remains of "+query_name()
#define _LANG_CORPSE_ROTTEN_REMAINS "rotten remains of a "+query_name()
#define _LANG_CORPSE_REMAINS_DESC "This is the dead body of a "+race_name+".\n"
