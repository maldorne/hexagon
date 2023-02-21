
// login.c

#define _LANG_LPMUD_VERSION "LPmud version: "
#define _LANG_MUDLIB_VERSION "Mudlib version: "
#define _LANG_LANGUAGE_VERSION "english"

#define _LANG_WHO_COMMAND "who"
#define _LANG_CREATE_COMMAND "create"
#define _LANG_FINGER_COMMAND "finger"

#define _LANG_LOGIN_CMDS_OPTIONS "Options: "
#define _LANG_OPTIONS_ACCOUNT_LOGIN "%^BOLD%^GREEN%^<email>%^RESET%^ to login with your account."
#define _LANG_OPTIONS_CREATE_ACCOUNT "%^BOLD%^create%^RESET%^ to create a new account."
#define _LANG_OPTIONS_GUEST_LOGIN "%^BOLD%^guest%^RESET%^ to temporarily test the game."
#define _LANG_OPTIONS_CHARACTER_LOGIN "%^BOLD%^GREEN%^<character name>%^RESET%^ to play again."
#define _LANG_OPTIONS_CREATE_CHARACTER "%^BOLD%^create%^RESET%^ to create a new player character."
#define _LANG_OPTIONS_LIST_CHARACTERS "%^BOLD%^characters%^RESET%^ to list all your available characters."
#define _LANG_OPTIONS_CMD_GAMES "%^BOLD%^games%^RESET%^ to list all available games in " + mud_name() + "."
#define _LANG_OPTIONS_CMD_FINGER "%^BOLD%^finger <name>%^RESET%^ to get information about other player."
#define _LANG_OPTIONS_CMD_WHO "%^BOLD%^who%^RESET%^ to see the list of connected players."

#define _LANG_WORLD_STILL_BEING_CREATED "The world is still being created. Try again in a minute.\n"
#define _LANG_TOO_MANY_PLAYERS "Too many players connected. Try again in a minute.\n"
#define _LANG_ENTER_AN_OPTION "Enter the chosen option: "
#define _LANG_TIMED_OUT "Timed out.\n"
#define _LANG_COME_AGAIN_SOON "\nCome again soon!\n"
#define _LANG_OPTION_STRING_TOO_SHORT "Option selected is too short, the minimum are " + MIN_LEN + " characters.\n"
#define _LANG_INVALID_CHARACTER_OR_EMAIL "Invalid user name or email" + (tmp >= 0) ? " (wrong character '" + str[tmp..tmp] + "' in '" + str + "').\n" : ".\n"
#define _LANG_PLAYER_ACCESS_FORBIDDEN "Player access is not allowed at the moment.\n"
#define _LANG_ANSWER_YES_NO "You must answer yes or no (y/n): "
#define _LANG_YES_OPTIONS_ARRAY ({ 'y', 'Y' })
#define _LANG_NO_OPTIONS_ARRAY ({ 'n', 'N' })
#define _LANG_TRY_AGAIN "Ok, try again.\n"

#define _LANG_ENTER_EMAIL "Enter your email: "
#define _LANG_ENTER_VALID_EMAIL "\nEnter a valid email: "
#define _LANG_USED_EMAIL "Sorry, but that address is already in use.\n" + \
      "Please, enter a different email: "
#define _LANG_ACCOUNT_RECOMMEND "If your already have an account in " + mud_name() + ", we really " + \
      "recommend to create your " + \
      "different characters in the same account, because that has a lot of benefits when " + \
      "sharing items, resources, achievements, etc.\n"
#define _LANG_ACCOUNT_NAME_CHOSEN "You have chosen '" + user_name + "' as your account name, are you sure? (y/n): "
#define _LANG_DEFINE_ACCOUNT_PASSWORD "Enter the new password for your account: "
#define _LANG_DEFINE_ACCOUNT_PASSWORD_REPEAT "Repeat the password to check there are no errors: "
#define _LANG_ACCOUNT_REPEAT "Let's try again."
#define _LANG_ACCOUNT_PASSWORD_REMOVED "Ok, Password removed."
#define _LANG_ACCOUNT_DIFFERENT_PASSWORDS "The passwords are not the same."
#define _LANG_NEW_ACCOUNT_CREATED "\n   %^BOLD%^New account created with name '" + user_name + "'.%^RESET%^\n\n"

#define _LANG_NONEXISTANT_ACCOUNT "There is no account with that name.\n"
#define _LANG_NONEXISTANT_ACCOUNT_OR_CHARACTER "There is no account or character with that name.\n"
#define _LANG_TYPE_ACCOUNT_PASSWORD "Type your account password: "
#define _LANG_TYPE_CHARACTER_PASSWORD "Type your character password: "
#define _LANG_WRONG_PASSWORD "Wrong password.\n"
#define _LANG_TOO_MANY_RETRIES "Too many retries...\nWhat are you doing?\n"
#define _LANG_CHOOSE_ACCOUNT_CHARACTER "\nType the name (or number) to start playing.\n" + \
      "   ('new' to create a new character)\n> \n"
#define _LANG_CHOOSE_NEW "new"
#define _LANG_TYPE_THE_NEW_CHARACTER_NAME "Type the name of your new character: "
#define _LANG_UNKNOWN_CHARACTER "Unknown character name or number, please type it again: "
#define _LANG_CHARACTER_RESTORE_ERROR "Error, the character could not be restored.\n"
#define _LANG_ACCOUNT_RESTORE_ERROR "Error, your account could not be restored.\n"
#define _LANG_ALREADY_PLAYING "You are already playing, should I dump the other copy? (y/n): "
#define _LANG_CODERS_FORBIDDEN "Coder access is not allowed to this port.\n"

#define _LANG_INVALID_USER_NAME "Wrong character '"+str[tmp..tmp]+"' in ("+str+").\n\n" 
#define _LANG_CHARACTER_NAME_TOO_LONG "The name is too long, max "+MAX_LEN+" characters.\n"
#define _LANG_CHARACTER_NAME_TOO_SHORT "The name is too short, min "+MIN_LEN+" characters.\n"
#define _LANG_USED_CHARACTER_NAME "Sorry, but that character name is already in use.\n" + \
      "Please, enter a different name for your new character: "
#define _LANG_CHARACTER_NAME_CHOSEN "You have chosen '" + name + "' as your character name, are you sure? (y/n): "
#define _LANG_CHOOSE_CHARACTER_GENDER "Your character is a man or a woman? (m/w): "
#define _LANG_GENDER_MAN_OPTIONS_ARRAY ({ 'm', 'M' })
#define _LANG_GENDER_WOMAN_OPTIONS_ARRAY ({ 'w', 'W' })
#define _LANG_ANSWER_CHOOSE_AN_OPTION "You must answer one of the available options."


#define _LANG_WAIT_LOADING "Please wait... loading your character.\n"
#define _LANG_CONNECTED_WELCOME "%^BOLD%^You are connected! Welcome to "+mud_name()+".%^RESET%^\n"
#define _LANG_HAS_RECONNECTED _user->query_cap_name() + " has reconnected.\n"

// game.c

#define _LANG_GAME_DEFAULT_FINGER_INFO "One of the games in " + mud_name() + ".\n"
#define _LANG_GAME_NO_PROJECT "No assigned project"
