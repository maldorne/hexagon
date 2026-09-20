
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
#define _LANG_MAILER_DISCARDED "Letter discarded.\n"
#define _LANG_MAILER_ASK_CC "Copy to (Enter for nobody): "

// editor.c

#define _LANG_EDITOR_START_MENU "Write the text. %^BOLD%^**%^RESET%^ on a line to go to the menu.\n"
#define _LANG_EDITOR_START_COMMAND "Write the text. %^BOLD%^**%^RESET%^ on a line to finish, " + \
  "~h to see the commands.\n"
#define _LANG_EDITOR_MENU_PROMPT "" + sizeof(lines) + (sizeof(lines) == 1 ? " line" : " lines") + \
  " - Choose from IDLMC" + (can_use_ed() ? "E" : "") + "SQ, H for help: "
#define _LANG_EDITOR_ASK_INSERT "Insert before line: "
#define _LANG_EDITOR_ASK_DELETE "Delete the line or lines (example: 3 or 1..3): "
#define _LANG_EDITOR_ASK_MODIFY "Modify the line or lines (example: 3 or 1..3): "
#define _LANG_EDITOR_ASK_MODIFY_FROM "Text to change: "
#define _LANG_EDITOR_ASK_MODIFY_TO "Change it to: "
#define _LANG_EDITOR_ASK_QUIT "Are you sure you want to quit without saving the text? (y/n): "
#define _LANG_EDITOR_YES ({ "y" })
#define _LANG_EDITOR_NO_LINES "There are no lines yet.\n"
#define _LANG_EDITOR_CONTINUE "You go on writing at the end. %^BOLD%^**%^RESET%^ on a line to stop.\n"
#define _LANG_EDITOR_INSERTING "You write before line " + number + ". %^BOLD%^**%^RESET%^ on a " + \
  "line to stop.\n"
#define _LANG_EDITOR_OUT_OF_RANGE "There are no lines with those numbers.\n"
#define _LANG_EDITOR_DELETED "Deleted.\n"
#define _LANG_EDITOR_ABORTED "Nothing changed.\n"
#define _LANG_EDITOR_DONE "Done.\n"
#define _LANG_EDITOR_BACK "Back to the editor.\n"
#define _LANG_EDITOR_SAVING "You save the text.\n"
#define _LANG_EDITOR_QUITTING "You quit without saving.\n"
#define _LANG_EDITOR_UNKNOWN_COMMAND "Unknown command. " + \
  (mode == EDITOR_MODE_COMMAND ? "~h" : "H") + " shows the help.\n"
#define _LANG_EDITOR_ED_START "You enter ed. %^BOLD%^x%^RESET%^ saves and quits, " + \
  "%^BOLD%^q%^RESET%^ quits, %^BOLD%^h%^RESET%^ shows the commands.\n"
#define _LANG_EDITOR_HELP \
  "  I - insert lines before a line number\n" + \
  "  D - delete lines: a number or a range like 1..3\n" + \
  "  L - show the lines written\n" + \
  "  M - change a text for another within some lines\n" + \
  "  C - go on writing at the end\n" + \
  (can_use_ed() ? "  E - hand the text to the ed editor\n" : "") + \
  "  S - quit and save the text\n" + \
  "  Q - quit without saving the text\n" + \
  "  H - show this help\n"

// board.c

#define _LANG_BOARD_NAME "notice board"
#define _LANG_BOARD_ALIASES ({ "board" })
#define _LANG_BOARD_SHORT "Notice Board"
#define _LANG_BOARD_PLURAL "Boards"
#define _LANG_BOARD_PLURALS ({ "boards" })
#define _LANG_BOARD_READ_CMDS ({ "read" })
#define _LANG_BOARD_POST_CMDS ({ "post" })
#define _LANG_BOARD_DELETE_CMDS ({ "eat" })
#define _LANG_BOARD_FOLLOWUP_CMDS ({ "followup" })
#define _LANG_BOARD_MUDMAIL_CMDS ({ "mudmail" })
#define _LANG_BOARD_SUBJECTS_CMDS ({ "subjects" })
#define _LANG_BOARD_EMPTY_TAG " [ Empty ]"
#define _LANG_BOARD_ONE_NOTE_TAG " [ 1 note ]"
#define _LANG_BOARD_NOTES_TAG " [ " + sizeof(stuff) + " notes ]"
#define _LANG_BOARD_NOTES_UNREAD_TAG " [ " + sizeof(stuff) + " notes (" + \
  ((pending == 1) ? "one" : query_num(pending, 20)) + " unread) ]"
#define _LANG_BOARD_ADMINISTRATOR "Administrator"
#define _LANG_BOARD_MARK_UNREAD "N"
#define _LANG_BOARD_READONLY "Only coders can write on this board.\n"
#define _LANG_BOARD_NO_GUESTS "Guests cannot write notes or letters.\n"
#define _LANG_BOARD_IS_EMPTY "The board is completely empty.\n"
#define _LANG_BOARD_SUBJECTS_PROMPT "Subjects"
#define _LANG_BOARD_COMMANDS "Commands (more in 'help boards'):\n" + \
  "  read [number]         read a note, or the first unread one\n" + \
  "  subjects              list every note\n" + \
  "  post <subject>        write a new note\n" + \
  "  followup <number>     answer a note on the board\n" + \
  "  mudmail <number>      answer whoever wrote it with a letter\n" + \
  "  eat <number>          tear down a note of yours\n\n"
#define _LANG_BOARD_NO_NEW_NOTES "\nThere are no new notes.\n"
#define _LANG_BOARD_READ_SYNTAX "Syntax: read <note number>\n"
#define _LANG_BOARD_NOTHING_TO_READ "You have no notes left to read.\n"
#define _LANG_BOARD_NO_SUCH_NOTE "There is no note with that number.\n"
#define _LANG_BOARD_NOTE_HEADER "Note #" + (num + 1) + " written by %^GREEN%^" + \
  author_of(stuff[num]) + "%^RESET%^ on " + ctime(stuff[num][B_TIME], 4) + "\n" + \
  "Subject: '%^GREEN%^" + stuff[num][B_SUBJECT] + "%^RESET%^'\n\n"
#define _LANG_BOARD_NOTE_PROMPT "Note " + (num + 1)
#define _LANG_BOARD_POST_SYNTAX "Syntax: post <subject>\n"
#define _LANG_BOARD_WRITE_ERROR "The note could not be written.\n"
#define _LANG_BOARD_WRITTEN "You pin your note on the board.\n"
#define _LANG_BOARD_NOT_SAVED "The note was not kept.\n"
#define _LANG_BOARD_DELETE_SYNTAX "Syntax: eat [until] <note number>\n"
#define _LANG_BOARD_DELETE_UNTIL "until"
#define _LANG_BOARD_DELETE_FAILED "You cannot delete that note.\n"
#define _LANG_BOARD_DELETED_MANY_ME "You tear down and destroy " + query_num(eaten, 0) + " notes.\n"
#define _LANG_BOARD_DELETED_MANY_ROOM this_player()->query_cap_name() + " tears down and destroys " + \
  query_num(eaten, 0) + " notes.\n"
#define _LANG_BOARD_DELETED_ONE_ME "You tear down and destroy note number " + num + ".\n"
#define _LANG_BOARD_DELETED_ONE_ROOM this_player()->query_cap_name() + \
  " tears down and destroys note number " + num + ".\n"
#define _LANG_BOARD_FOLLOWUP_SYNTAX "Syntax: followup <note number>\n"
#define _LANG_BOARD_MUDMAIL_SYNTAX "Syntax: mudmail <note number>\n"

// globe
#define _LANG_GLOBE_NAME "globe"
#define _LANG_GLOBE_SHORT "Chat Globe"
#define _LANG_GLOBE_PLURAL "Chat Globes"
#define _LANG_GLOBE_ALIASES ({ "chat globe" })
#define _LANG_GLOBE_PLURALS ({ "globes", "chat globes" })
#define _LANG_GLOBE_LONG "This is one of the most curious objects you have come across in " + \
  "all your travels. It is a bluish globe resting on a strange golden stand, " + \
  "with curious metal arcs circling the sphere. It surely has something " + \
  "magical about it.\n"
#define _LANG_GLOBE_READ "This globe was made by some powerful wizard so that whoever is " + \
  "in any pub of the world can talk to each other.\n" + \
  "\tUse 'globe chat <message>' to speak through every globe.\n" + \
  "\tUse 'globe who' to find out who is standing by a globe.\n"
#define _LANG_GLOBE_VERB "globe"
#define _LANG_GLOBE_CHAT_OPTIONS ({ "chat" })
#define _LANG_GLOBE_WHO_OPTIONS ({ "who" })
#define _LANG_GLOBE_USAGE "Syntax: globe chat <message>, globe who\n"
#define _LANG_GLOBE_NO_MESSAGE "You should say something too.\n"
#define _LANG_GLOBE_HEARD "[Globe] " + this_player()->query_cap_name() + ": " + str + "\n"
#define _LANG_GLOBE_SAID "You say through the globe: " + str + "\n"
#define _LANG_GLOBE_WHO_PLACE "In " + place + (strlen(game) ? " (" + game + ")" : "") + ":\n"
#define _LANG_GLOBE_DRUNK_REPLACEMENTS ({ "s", "sh", "r", "rr", "ing", "in'", "x", "xsh", "S", "SH", "R", "RR" })

#define _LANG_TORCH_NAME "torch"
#define _LANG_TORCH_LONG "A small torch to light the way, little more than a stick " + \
      "of old wood with strips of cloth wound around one end. Use \"light torch\" and " + \
      "\"extinguish torch\" to work it.\n"

#define _LANG_TORCH_AS_NEW "It is as good as new.\n"
#define _LANG_TORCH_BURNT_OUT "It is burnt out.\n"
#define _LANG_TORCH_LEFT "It has " + percentage + "% of its use left.\n"
#define _LANG_TORCH_LIT " (%^YELLOW%^lit%^RESET%^)"
#define _LANG_TORCH_LIT_PLURAL " (%^YELLOW%^lit%^RESET%^)"

#define _LANG_TORCH_VERBS_LIGHT ({ "light" })
#define _LANG_TORCH_VERBS_EXTINGUISH ({ "extinguish", "quench" })

#define _LANG_TORCH_WHAT capitalize(query_verb()) + " what?\n"
#define _LANG_TORCH_HOLD_FIRST "You have to hold the torch first.\n"
#define _LANG_TORCH_ALREADY_LIT "The torch is already lit.\n"
#define _LANG_TORCH_ALREADY_OUT "The torch is not lit.\n"

#define _LANG_TORCH_YOU_LIGHT "%^YELLOW%^You light the torch.%^RESET%^\n"
#define _LANG_TORCH_SOMEBODY_LIGHTS "%^YELLOW%^" + who->query_cap_name() + \
      " lights a torch.%^RESET%^\n"
#define _LANG_TORCH_YOU_EXTINGUISH "You put the torch out.\n"
#define _LANG_TORCH_SOMEBODY_EXTINGUISHES who->query_cap_name() + \
      " puts a torch out.\n"
#define _LANG_TORCH_GOES_OUT "Your torch goes out.\n"
#define _LANG_TORCH_SOMEBODYS_GOES_OUT "The torch of " + who->query_cap_name() + \
      " goes out.\n"
