
// common

#define _LANG_CMD_FILE "File"
#define _LANG_CMD_FILE_DOES_NOT_EXIST "That file does not exist.\n"
#define _LANG_CMD_CANNOT_READ_FILE "Cannot read that file.\n"
#define _LANG_CMD_NO_FILES "There are no files with that name.\n"

// uptime

#define _LANG_UPTIME_SINCE "The mud has been up for:\n\t"
#define _LANG_UPTIME_DAY "day"
#define _LANG_UPTIME_HOUR "hour"
#define _LANG_UPTIME_MINUTE "minute"
#define _LANG_UPTIME_SECOND "second"

// cat

#define _LANG_CAT_WHAT "Cat what file?\n"
#define _LANG_CAT_ASTERISK "cat *? Forget about it.\n"
#define _LANG_CAT_TRUNCATED "\n ----- TRUNCATED ----- \n\n"

// clone

#define _LANG_CLONE_WHAT "Clone what?\n"
#define _LANG_CLONE_ERROR_IN "Error in "
#define _LANG_CLONE_ERROR_MAYBE_UNIQUE "Error, could not clone object (maybe unique?).\n"
#define _LANG_CLONE_OK_YOU "Ok. Object " + file_name(ob) + " cloned in " + \
        (environment(ob) == this_player() ? "you" : \
        (environment(ob) == environment(this_player()) ? "this place" : \
          this_player()->desc_object(ob))) + \
        ".\n"
#define _LANG_CLONE_OK_ENV this_player()->query_cap_name() + " looks for a " + \
        ((string)ob->query_short() ? (string)ob->query_short() : "object") + \
        " in other dimension.\n"
#define _LANG_CLONE_ERROR "Error, the object could not be cloned.\n"

// compare

#define _LANG_COMPARE_HELP "This command allows the user to compare two files. " + \
      "The command returns the lines of file1 that were " + \
      "not found in file2 if which_file is equal to 1 and " + \
      "vice-versa if which_file is equal to 2.\n\n" + \
      "Note: both file1 and file2 differences are returned if " + \
      "which_file is not entered."
#define _LANG_COMPARE_SYNTAX "Usage : compare -<which_file> file1 file2\n"
#define _LANG_COMPARE_INVALID_WHICH "Invalid -<which_file> value.\n\nOptions:\n" + \
      "-1 for file1\n-2 for file2\n-3 or nothing for both\n"
#define _LANG_COMPARE_NOT_A_FILE "Not a file: "

// peace

#define _LANG_PEACE_MESSAGES_MINE "You raise your hand and exclaim: " + \
        "%^YELLOW%^Stop fighting, mortals!%^RESET%^\n"
#define _LANG_PEACE_MESSAGES_OTHERS this_player()->query_cap_name() + \
        " raises " + this_player()->query_possessive() + " hand and exclaims: " + \
        "%^YELLOW%^Stop fighting, mortals!%^RESET%^\n"
