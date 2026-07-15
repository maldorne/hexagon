
// finger.c

#define _LANG_FINGER_SYNTAX "finger [<name>]"
#define _LANG_FINGER_HELP "Shows information about a player character."
#define _LANG_FINGER_NAME "Name"
#define _LANG_FINGER_ACCOUNT "Account"
#define _LANG_FINGER_REAL_NAME "Real name"
#define _LANG_FINGER_CITY "City"
#define _LANG_FINGER_BIRTHDAY "Birthday"
#define _LANG_FINGER_NOBODY "Nobody with the name " + str + " has visited "+mud_name()+".\n"
#define _LANG_FINGER_FORBIDDEN "(access forbidden)"
#define _LANG_FINGER_FORBIDDEN_REASON "Forbidden because of: '" + capitalize(ret) + "'\n"

// who

#define _LANG_WHO_SYNTAX "who\n" + \
                       "        who coders\n" + \
                       "        who players\n" + \
                       "        who <race or culture>"
#define _LANG_WHO_HELP "Shows a list of the connected characters."
#define _LANG_WHO_OPTION_CODERS "coders"
#define _LANG_WHO_OPTION_PLAYERS "players"
#define _LANG_WHO_REAL_WORLD_DATE "Today is " + ctime(time(), 5) + " in the real world"
#define _LANG_WHO_EDITING_MSG " %^GREEN%^(Editing: " + (string)tmp + ")%^RESET%^"
#define _LANG_WHO_AWAY_MSG " (%^WHITE%^%^BOLD%^Away%^RESET%^)"
#define _LANG_WHO_IDLE_MSG " (%^CYAN%^%^BOLD%^Idle: " + (user->query_idle() / 60) + "%^RESET%^)"
#define _LANG_WHO_LINKDEAD_MSG " (%^YELLOW%^%^BOLD%^Linkdead: " + ((time() - user->query_linkdead_at()) / 60) + " min%^RESET%^)"
#define _LANG_WHO_GUEST_MSG " guest in " + mud_name()
#define _LANG_WHO_NO_CODERS_MSG "> %^GREEN%^No coders connected%^RESET%^ <"
#define _LANG_WHO_NO_PLAYERS_MSG "> %^GREEN%^No players connected%^RESET%^ <"
#define _LANG_WHO_IS_THE_ONLY_ONE "> %^GREEN%^Is the only one connected to " + mud_name() + "%^RESET%^ <"
#define _LANG_WHO_YOU_ARE_THE_ONLY_ONE "> %^GREEN%^You are the only one connected to " + mud_name() + "%^RESET%^ <"
#define _LANG_WHO_ONLY_CODERS_MSG "> %^GREEN%^" + capitalize(query_num(creators, 100)) + " coder" + \
        (creators < 2 ? "" : "s") + " connected to " + mud_name() + "%^RESET%^ <"
#define _LANG_WHO_ONLY_PLAYERS_MSG "> %^GREEN%^ " + capitalize(query_num(num_people, 100)) + " player" + \
        (num_people < 2 ? "" : "s") + " connected to " + mud_name() + "%^RESET%^ <"
#define _LANG_WHO_MULTIPLE_MSG "> %^GREEN%^ " + capitalize(query_num(creators, 100)) + " coder" + \
      (creators < 2 ? "" : "s") + " and " + query_num(num_people, 100) + " player" + \
      (num_people < 2 ? "" : "s") + " connected to " + mud_name() + "%^RESET%^ <"

// help

#define _LANG_CMD_HELP "help [< topic | command >]"
#define _LANG_CMD_HELP_HELP "Shows the available help for a particular topic or command, or a list "+ \
      "of every topic if none given."

// characters

#define _LANG_CMD_CHARACTERS "characters"
#define _LANG_CMD_CHARACTERS_HELP "Show every character available in your account."
#define _LANG_AVAILABLE_CHARACTERS_IN_ACCOUNT "You have the following characters available in your account:\n"
#define _LANG_CMD_CHARACTERS_NO_LOGIN "You must login first."

// games

#define _LANG_CMD_GAMES "games"
#define _LANG_CMD_GAMES_SYNTAX "games"
#define _LANG_CMD_GAMES_HELP "List every available game in " + mud_name() + "."
#define _LANG_CMD_GAMES_AVAILABLE "The following games are available:\n"
#define _LANG_CMD_GAMES_NO_LOGIN "You must login first."
#define _LANG_CMD_GAMES_UNAVAILABLE_GAME "Non playable"

// idle

#define _LANG_IDLE_SYNTAX "idle [<seconds>]"
#define _LANG_IDLE_HELP "How long, in real seconds, the mud keeps your character in the world\n" + \
                        "after your connection drops (so combat cannot be dodged by pulling the plug).\n" + \
                        "Running `idle` alone shows the current value and the allowed range.\n" + \
                        "Running `idle <seconds>` sets a new value; anything outside the range is clamped."
#define _LANG_IDLE_SHOW "Linkdead grace is %d seconds (allowed range %d – %d).\n"
#define _LANG_IDLE_SET "Linkdead grace set to %d seconds.\n"
