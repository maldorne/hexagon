
// common

#define _LANG_CMD_SYNTAX "Syntax: "

// communicate.c

#define _LANG_COMM_ROWS "rows"
#define _LANG_COMM_COLS "columns"
#define _LANG_COMM_ROWS_SYNTAX "Rows currently set to  " + rows + ".\n"+query_verb() + \
        " <number> to change it.\n"
#define _LANG_COMM_ROWS_WRONG "Wrong number of rows (must be 10 or more).\n"
#define _LANG_COMM_ROWS_SET "Rows set to " + val + ".\n"
#define _LANG_COMM_COLS_SYNTAX "Columns currently set to " + cols + ".\n"+query_verb() + \
        " <number> to change it.\n"
#define _LANG_COMM_COLS_WRONG "Wrong number of columns (must be from 35 to 200).\n"
#define _LANG_COMM_COLS_SET "Columns set to " + val + ".\n"

#define _LANG_COMM_EARMUFFS "earmuffs"
#define _LANG_COMM_TERMINAL "terminal"
#define _LANG_COMM_BLOCK "block"
#define _LANG_COMM_UNBLOCK "unblock"

// help.c

#define _LANG_HELP_CMD "Help about the command '" + str + "':\n"
#define _LANG_HELP_CMD_NO_HELP "There is no help text about that."
#define _LANG_HELP_SYNTAX "Syntax: help [<topic | command>]\n"
#define _LANG_HELP_NAME "help"
#define _LANG_HELP_NO_HELP_ABOUT "Sorry, there is no help about "+str+".\n"
#define _LANG_HELP_HELP_ABOUT "Help about " + objs[loop]->query_name() + ":\n" + text + "\n\n"
#define _LANG_HELP_NO_HELP_FOR_ITEM "There is no help about "+((sizeof(objs) > 1) ? "those items":"that item")+".\n"
#define _LANG_HELP_EMOTIONS ({ "emotion", "emotions", "soul", "souls" })
#define _LANG_HELP_MORE_EMOTIONS "Emotions"

// more_string.c

#define _LANG_MORE_STRING_MORE "More"
#define _LANG_MORE_STRING_STAT_LINE "$N > from $T to $B of $S ($%%) - h for help. "
#define _LANG_MORE_STRING_NOT_FOUND "Sorry, "+s1+" not found.\n"
#define _LANG_MORE_STRING_HELP_NOT_FOUND "Cannot find the help document. " + \
                             "Please tell somebody to fix this.\n"
#define _LANG_MORE_STRING_EMPTY_STRING "Empty string.\n"

// notifications.c

#define _LANG_NO_NOTIFICATIONS "You haven't received any notification yet.\n"
#define _LANG_PAST_NOTIFICATIONS "Past notifications"

// commands.c

#define _LANG_ACCOUNT_COMMANDS "" + \
        "Account commands:\n" + \
        "   %^BOLD%^GREEN%^<character name>%^RESET%^ to continue playing.\n" + \
        "   %^BOLD%^players%^RESET%^ to list all your characters.\n" + \
        "   %^BOLD%^create%^RESET%^ to create a new player character.\n" + \
        "   %^BOLD%^games%^RESET%^ to list available games.\n" + \
        "   %^BOLD%^password%^RESET%^ to change your password.\n"

// security

#define _LANG_COMM_PASSWORD ({ "password" })
#define _LANG_OLD_PASSWORD "Please, type your old password: "
#define _LANG_CANCELLING "Cancelling...\n"
#define _LANG_WRONG_PASSWORD "Wrong password."
#define _LANG_UNKNOWN_PASSWORD_ERROR "There is some problem with your account password.\n"
#define _LANG_NEW_PASSWORD "Please, type your new password: "
#define _LANG_PASSWORD_CONDITIONS "Your password must have at least six characters."
#define _LANG_REPEAT_PASSWORD "Please, type the password again: "
#define _LANG_ERROR_IN_PASSWORD "There is an error with your password, contact an administrator.\n"

// ui.c

#define _LANG_UI_SIDEBAR_SYNTAX "Use sidebar <on|off> to activate or deactivate it.\n"
#define _LANG_UI_MAP "Map"
#define _LANG_UI_STATS "Stats"

// help topics

#define _LANG_HELP_REINDEX ({ "reindex" })
#define _LANG_HELP_REINDEXED(n) "Indexed " + (n) + " help topics.\n"
#define _LANG_HELP_SECTIONS_FOOTER \
        "  help <section>     the topics of a section\n" + \
        "  help <topic>       read one\n" + \
        "  help *word*        search\n" + \
        "  commands           everything you can type\n"
#define _LANG_HELP_SECTIONS_STAFF "  for creators"
#define _LANG_HELP_KIND_INDEX "index"
#define _LANG_HELP_SEE_ALSO "See also: " + implode(words, ", ") + ".\n"
#define _LANG_HELP_MATCHES "\nMatching:\n\n"
#define _LANG_HELP_NO_MATCHES "No topic matches that.\n"
#define _LANG_HELP_TITLE(name, kind) "%^BOLD%^WHITE%^" + (name) + "%^RESET%^ (" + (kind) + ")"
#define _LANG_HELP_KIND_SEPARATOR "·"
#define _LANG_HELP_KIND_COMMAND "command"
#define _LANG_HELP_KIND_SKILL "skill"
#define _LANG_HELP_KIND_TOPIC "topic"
#define _LANG_HELP_KIND_CODER_TOPIC "topic for creators"
#define _LANG_HELP_KIND_DRIVER "driver manual"
#define _LANG_HELP_KIND_SOUL "emote"
#define _LANG_HELP_KIND_SECTION "section"

// account details. `str` is what the player just typed
#define _LANG_ACCOUNT_DETAILS_VERBS ({ "details", "chfn" })
#define _LANG_ACCOUNT_NONE_WORDS ({ "none" })

#define _LANG_ACCOUNT_DETAILS_HEADER "Changing the personal details of your account.\n" + \
  "Leave a line blank to keep what is there (shown in brackets).\n"

#define _LANG_ACCOUNT_ASK_REAL_NAME "\nType your real name [" + \
  this_object()->query_real_name() + "]\n('" + _LANG_ACCOUNT_NONE_WORDS[0] + "' to clear it): "
#define _LANG_ACCOUNT_REAL_NAME_CLEARED "Ok, real name cleared.\n"
#define _LANG_ACCOUNT_REAL_NAME_SET "Ok, real name set to " + str + ".\n"
#define _LANG_ACCOUNT_REAL_NAME_KEPT "Ok, real name unchanged.\n"

#define _LANG_ACCOUNT_ASK_LOCATION "\nType where you live (e.g. Madrid, Stockholm... ) [" + \
  this_object()->query_location() + "]\n('" + _LANG_ACCOUNT_NONE_WORDS[0] + "' to clear it): "
#define _LANG_ACCOUNT_LOCATION_CLEARED "Ok, place of residence cleared.\n"
#define _LANG_ACCOUNT_LOCATION_SET "Ok, place of residence set to " + str + ".\n"
#define _LANG_ACCOUNT_LOCATION_KEPT "Ok, place of residence unchanged.\n"

#define _LANG_ACCOUNT_ASK_BIRTHDAY "\nType your birthday (ddmm) [" + \
  this_object()->query_birthday() + "]\n('" + _LANG_ACCOUNT_NONE_WORDS[0] + "' to clear it): "
#define _LANG_ACCOUNT_BIRTHDAY_CLEARED "Ok, birthday cleared.\n"
#define _LANG_ACCOUNT_BIRTHDAY_SET "Ok, birthday set to " + str + ".\n"
#define _LANG_ACCOUNT_BIRTHDAY_KEPT "Ok, birthday unchanged.\n"
#define _LANG_ACCOUNT_BIRTHDAY_INVALID "That date is not valid, type it again (ddmm): "
