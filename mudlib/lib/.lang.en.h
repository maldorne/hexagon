
// user.c

#define _LANG_DISCONNECTED "Disconnected.\n"
#define _LANG_LAST_CONNECTION "Your last connection was from '"+last_on_from+"'.\n"
#define _LANG_USER_COMMAND_TOO_LONG "Command too long - processing anyway.\n"

// monster.c

#define _LANG_MONSTER_CANNOT_CLONE query_cap_name() + " tries to hold " + query_possessive() + \
              " " + ob->query_name() + " but in the end " + query_pronoun() + " drops it.\n"

// room.c

#define _LANG_DEFAULT_SIGN_NAME "sign"

// language.c

#define _LANG_LANGUAGE_DEFAULT_DESC "This is the normal language spoken by most creatures.\n"

// player.c

#define _LANG_PLAYER_SAVING "Saving...\n"
#define _LANG_PLAYER_NO_GUEST_SAVE "Oops, guests can't save...\n"
#define _LANG_PLAYER_SAVE_ACTIONS ({ "save" })
#define _LANG_PLAYER_NOT_IN_CONDITIONS "You are not in conditions to run away.\n"
#define _LANG_PLAYER_SEE_FEET_RUNNING "You see your feet running before you.\n"
#define _LANG_PLAYER_FAILED_ESCAPE "You tried to escape, but you couldn't find any exit.\n"

// item.c

#define _LANG_ITEM_BREAKS "Your " + query_name() + " breaks in a thousand pieces!\n"

// armour.c

#define _LANG_DESC_IS_SET_PIECE "   This item is part of the set: " + \
              main_piece->query_piece_set_name() + ".\n"
#define _LANG_DESC_IS_MAIN_SET_PIECE "   This item is the main piece of the '" + \
              main_piece->query_piece_set_name() + "' set, made up of "