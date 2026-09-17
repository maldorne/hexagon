
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

// diary.c

#define _LANG_DIARY_NAME "diary"
#define _LANG_DIARY_SHORT "Diary"
#define _LANG_DIARY_ALIAS ({ "diary", "book" })
#define _LANG_DIARY_PLURAL "Diaries"
#define _LANG_DIARY_PLURALS ({ "diaries" })
#define _LANG_DIARY_LONG "A worn book where you jot down your adventures and travels. " + \
    "Type '%^BOLD%^diary explorer%^RESET%^' to see the places you have discovered.\n"
#define _LANG_DIARY_VERBS ({ "diary" })
#define _LANG_DIARY_EXPLORER_SUBVERBS ({ "explorer", "exploration", "places" })
#define _LANG_DIARY_PENDING_SUBVERBS ({ "travels", "encyclopedia" })
#define _LANG_DIARY_HELP "The diary keeps track of your wanderings. Commands:\n" + \
    "\tdiary explorer  - lists the places you have discovered.\n"
#define _LANG_DIARY_DEAD "You cannot do that while dead.\n"
#define _LANG_DIARY_WHAT "What do you want to check in the diary? Try 'diary explorer'.\n"
#define _LANG_DIARY_NOT_YET "That section of the diary is still blank.\n"
#define _LANG_DIARY_EXPLORER_EMPTY "You have not discovered any place worth noting yet.\n"
#define _LANG_DIARY_EXPLORER_HEADER "Throughout your travels you have been to:\n\n"
#define _LANG_DIARY_EXPLORER_COUNT_PRE "\nIn total, you have visited "
#define _LANG_DIARY_PLACE_SG "place"
#define _LANG_DIARY_PLACE_PL "places"

// mailer.c

#define _LANG_MAILER_PROMPT "mail> "
#define _LANG_MAILER_EMPTY "You have no letters.\n"
#define _LANG_MAILER_HEADERS_TITLE "Your letters ('?' to see the commands):\n\n"
#define _LANG_MAILER_MARK_DELETED "D"
#define _LANG_MAILER_MARK_UNREAD "N"
#define _LANG_MAILER_NO_SUCH_LETTER "You have no letter with that number.\n"
#define _LANG_MAILER_WHICH_LETTERS "Which letters? Give their numbers: 3, 1-4 or 2,5.\n"
#define _LANG_MAILER_MARKED "Marked to be deleted when you quit.\n"
#define _LANG_MAILER_UNMARKED "They will not be deleted.\n"
#define _LANG_MAILER_LETTER_FROM "From: " + capitalize(letter["from"]) + "\n"
#define _LANG_MAILER_LETTER_TO "To: " + list_names(letter["to"]) + "\n"
#define _LANG_MAILER_LETTER_CC "Cc: " + list_names(letter["cc"]) + "\n"
#define _LANG_MAILER_LETTER_DATE "Date: " + ctime(letter["date"], 4) + "\n"
#define _LANG_MAILER_LETTER_SUBJECT "Subject: " + letter["subject"] + "\n"
#define _LANG_MAILER_NO_GROUPS "You have no groups.\n"
#define _LANG_MAILER_GROUPS_TITLE "Your groups:\n\n"
#define _LANG_MAILER_GROUP_SYNTAX "Type g <group> <names> to add, and -<name> to remove.\n"
#define _LANG_MAILER_GROUP_ADDED "Added to " + group + ": " + list_names(done) + ".\n"
#define _LANG_MAILER_GROUP_NOT_ADDED "Not added to " + group + " (they do not exist or were already there): " + \
  list_names(adding - done) + ".\n"
#define _LANG_MAILER_GROUP_REMOVED "Removed from " + group + ": " + list_names(done) + ".\n"
#define _LANG_MAILER_GROUP_NOT_REMOVED "None of those are in " + group + ".\n"
#define _LANG_MAILER_HELP \
  "  <number>                  read that letter\n" + \
  "  l                         list your letters\n" + \
  "  m <names>                 write a letter\n" + \
  "  r <number>                reply to whoever sent it\n" + \
  "  f <number> <names>        forward a letter\n" + \
  "  d <numbers>               mark letters to be deleted when you quit\n" + \
  "  u <numbers>               unmark them\n" + \
  "  g                         show your groups\n" + \
  "  g <group> <names>         add names to a group (-<name> removes it)\n" + \
  "  q                         quit, deleting the marked letters\n" + \
  "  ?                         show this help\n"
#define _LANG_MAILER_CMD_LIST ({ "l" })
#define _LANG_MAILER_CMD_WRITE ({ "m" })
#define _LANG_MAILER_CMD_REPLY ({ "r" })
#define _LANG_MAILER_CMD_FORWARD ({ "f" })
#define _LANG_MAILER_CMD_DELETE ({ "d" })
#define _LANG_MAILER_CMD_UNDELETE ({ "u" })
#define _LANG_MAILER_CMD_GROUPS ({ "g" })
#define _LANG_MAILER_CMD_QUIT ({ "q" })
#define _LANG_MAILER_CMD_HELP ({ "?" })
#define _LANG_MAILER_WRITE_TO_WHOM "To whom? Type m <names>.\n"
#define _LANG_MAILER_REPLY_PREFIX "Re: "
#define _LANG_MAILER_FORWARD_SYNTAX "Type f <number> <names>.\n"
#define _LANG_MAILER_FORWARD_PREFIX "Fwd: "
#define _LANG_MAILER_FORWARD_BODY "Letter forwarded by " + capitalize(owner_name) + ", written by " + \
  capitalize(letter["from"]) + " on " + ctime(letter["date"], 4) + ":\n\n"
#define _LANG_MAILER_UNKNOWN_RECIPIENTS "There is nobody to write to with those names: " + \
  list_names(recipients["unknown"]) + ".\n"
#define _LANG_MAILER_NOBODY_TO_WRITE "The letter has nobody to go to.\n"
#define _LANG_MAILER_SENT "Letter sent to " + list_names(delivered) + ".\n"
#define _LANG_MAILER_NOT_SENT "The letter did not reach anybody.\n"
#define _LANG_MAILER_DELETED (number == 1 ? "One letter deleted.\n" : "" + number + " letters deleted.\n")
#define _LANG_MAILER_CLOSED "You close your mailbox.\n"
#define _LANG_MAILER_UNKNOWN_COMMAND "Unknown command. Type '?' to see them.\n"
#define _LANG_MAILER_ASK_SUBJECT "Subject: "
#define _LANG_MAILER_NO_SUBJECT "(no subject)"
#define _LANG_MAILER_WRITE_BODY "Write the letter. A line with just a dot ends it; '~q' discards it.\n"
#define _LANG_MAILER_BODY_END "."
#define _LANG_MAILER_BODY_CANCEL "~q"
#define _LANG_MAILER_DISCARDED "Letter discarded.\n"
#define _LANG_MAILER_ASK_CC "Copy to (Enter for nobody): "
