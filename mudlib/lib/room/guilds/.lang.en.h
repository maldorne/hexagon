// guild entrances and guild rooms. `me` is the player joining or leaving,
// `guild` the guild object, `wait` the seconds of play left to wait

#define _LANG_GUILD_ENTRANCE_CMDS_JOIN ({ "join" })
#define _LANG_GUILD_ENTRANCE_CMDS_LEAVE ({ "leave" })
#define _LANG_GUILD_ENTRANCE_YES ({ "y", "yes" })

#define _LANG_GUILD_ENTRANCE_SIGN_LONG "A sign hangs on the wall, by the entrance.\n"
#define _LANG_GUILD_ENTRANCE_SIGN_NO_GUILD "This guild admits nobody right now."
#define _LANG_GUILD_ENTRANCE_SIGN_TITLE "Guild: "
#define _LANG_GUILD_ENTRANCE_SIGN_RACES "It admits: "
#define _LANG_GUILD_ENTRANCE_SIGN_CLASSES "Classes: "
#define _LANG_GUILD_ENTRANCE_SIGN_COMMANDS \
        "  join     become a member of the guild\n" + \
        "  leave    leave the guild you belong to\n\n" + \
        "After joining a guild you will have to wait " + \
        (GUILD_JOIN_WAIT / 3600) + " hours of play before you may join another."

#define _LANG_GUILD_ENTRANCE_NO_GUILD "This guild admits nobody right now.\n"
#define _LANG_GUILD_ENTRANCE_ALREADY_MEMBER "You are already a member of this guild.\n"
#define _LANG_GUILD_ENTRANCE_OTHER_GUILD "You already belong to a guild: you will " + \
        "have to leave it before you join another.\n"
#define _LANG_GUILD_ENTRANCE_NOT_LEGAL "You do not meet the conditions to join " + \
        "this guild.\n"
#define _LANG_GUILD_ENTRANCE_TOO_SOON "You joined a guild very recently: you will " + \
        "have to play " + seconds_to_time_string(wait) + " more before you join " + \
        "another.\n"
#define _LANG_GUILD_ENTRANCE_JOINED "You join the guild.\n"
#define _LANG_GUILD_ENTRANCE_JOINED_OTHERS me->query_cap_name() + " joins the " + \
        "guild.\n"
#define _LANG_GUILD_ENTRANCE_NO_GUILD_TO_LEAVE "You do not belong to any guild.\n"
#define _LANG_GUILD_ENTRANCE_CANNOT_LEAVE "For some reason you are not allowed to " + \
        "leave your guild.\n"
#define _LANG_GUILD_ENTRANCE_LEAVE_CONFIRM "Are you sure you want to leave your " + \
        "guild? (y/n) "
#define _LANG_GUILD_ENTRANCE_LEAVE_CANCELLED "Very well, you stay in your guild.\n"
#define _LANG_GUILD_ENTRANCE_LEFT "You leave your guild: from now on you belong " + \
        "to none.\n"

#define _LANG_GUILD_ROOM_CMDS_MEMBERS ({ "members" })
#define _LANG_GUILD_ROOM_SIGN_LONG "A sign on the wall says what can be done here.\n"
#define _LANG_GUILD_ROOM_SIGN_TEXT "Guild hall\n\n" + \
        "  members    the most famous members of the guild"
#define _LANG_GUILD_ROOM_NOW_FAMOUS "You are now on the list of the most famous " + \
        "members of your guild.\n"
#define _LANG_GUILD_ROOM_NOW_FAMOUS_OVER "You are now on the list of the most " + \
        "famous members of your guild: you have overtaken " + lowest + ".\n"
#define _LANG_GUILD_ROOM_NO_FAMOUS "There is nobody on the list of the most " + \
        "famous members of this guild yet.\n"
#define _LANG_GUILD_ROOM_FAMOUS_HEADER "The most famous members of the guild:\n\n"
#define _LANG_GUILD_ROOM_LEVEL "level"
