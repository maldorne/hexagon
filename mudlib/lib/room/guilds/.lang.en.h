// guild entrances and guild rooms. `me` is the player joining or leaving,
// `guild` the guild object, `wait` the seconds of play left to wait

#define _LANG_GUILD_ENTRANCE_CMDS_JOIN ({ "join" })
#define _LANG_GUILD_ENTRANCE_CMDS_LEAVE ({ "leave" })
#define _LANG_GUILD_ENTRANCE_YES ({ "y", "yes" })
#define _LANG_GUILD_CMDS_RULES ({ "rules" })

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
#define _LANG_GUILD_ENTRANCE_JOINED "You join the guild. For the next " + \
        (GUILD_JOIN_WAIT / 3600) + " hours of play you will not be able to join " + \
        "any other. In the guild hall, type 'rules' to read its rules.\n"
#define _LANG_GUILD_ENTRANCE_JOINED_OTHERS me->query_cap_name() + " joins the " + \
        "guild.\n"
#define _LANG_GUILD_ENTRANCE_NO_GUILD_TO_LEAVE "You do not belong to any guild.\n"
#define _LANG_GUILD_ENTRANCE_CANNOT_LEAVE "For some reason you are not allowed to " + \
        "leave your guild.\n"
#define _LANG_GUILD_ENTRANCE_LEAVE_WAIT_WARNING "You joined recently: if you " + \
        "leave, you will not be able to join any guild until you play " + \
        seconds_to_time_string(wait) + " more.\n"
#define _LANG_GUILD_ENTRANCE_LEAVE_CONFIRM "Are you sure you want to leave your " + \
        "guild? (y/n) "
#define _LANG_GUILD_ENTRANCE_LEAVE_CANCELLED "Very well, you stay in your guild.\n"
#define _LANG_GUILD_ENTRANCE_LEFT "You leave your guild: from now on you belong " + \
        "to none.\n"

#define _LANG_GUILD_ROOM_CMDS_COST ({ "cost" })
#define _LANG_GUILD_ROOM_CMDS_ADVANCE ({ "advance" })
#define _LANG_GUILD_ROOM_SIGN_LONG "A sign on the wall says what can be done here.\n"
#define _LANG_GUILD_ROOM_SIGN_TEXT "Guild hall\n\n" + \
        "  cost       what your next guild level costs\n" + \
        "  advance    go up a level of the guild, if you have that experience\n" + \
        "  learn      learn the spells and skills your guild level allows\n" + \
        "  rules      who may join, what is learned and the rules of the guild\n\n" + \
        "Only members of the guild may advance and read its rules here."
#define _LANG_GUILD_ROOM_NO_GUILD "This hall belongs to no guild right now.\n"
#define _LANG_GUILD_ROOM_NOT_MEMBER "Only members of this guild may advance here.\n"
#define _LANG_GUILD_ROOM_RULES_NOT_MEMBER "Only members of this guild may read its " + \
        "rules here.\n"
#define _LANG_GUILD_ROOM_MAX_LEVEL "You have already reached the highest level of your guild.\n"
#define _LANG_GUILD_ROOM_NOT_ENOUGH_XP "You do not have experience enough to advance yet: " + \
        "you have " + have + " and need " + cost + ".\n"
#define _LANG_GUILD_ROOM_ADVANCED_OTHERS me->query_cap_name() + " advances a level in the guild.\n"
#define _LANG_GUILD_ROOM_COST "To advance to level " + next + " of your guild " + \
        "you need " + cost + " experience points (you have " + have + ").\n"
#define _LANG_GUILD_ROOM_CMDS_LEARN ({ "learn" })
#define _LANG_GUILD_ROOM_LEARN_NOT_MEMBER "Only the members of this guild may learn here.\n"
#define _LANG_GUILD_ROOM_LEARNED "You learn " + implode(names, ", ") + ".\n"
#define _LANG_GUILD_ROOM_LEARNED_OTHERS me->query_cap_name() + " studies with the masters of the guild.\n"
#define _LANG_GUILD_ROOM_LEARN_NEXT "You know everything the guild teaches at your level. Next comes " + \
        next_name + ", at level " + next_level + ".\n"
#define _LANG_GUILD_ROOM_LEARN_NOTHING "You know everything the guild can teach you.\n"
