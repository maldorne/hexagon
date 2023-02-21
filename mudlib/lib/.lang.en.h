
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

// item.c

#define _LANG_ITEM_BREAKS "Your " + query_name() + " breaks in a thousand pieces!\n"

// armour.c

#define _LANG_DESC_IS_SET_PIECE "   This item is part of the set: " + \
              main_piece->query_piece_set_name() + ".\n"
#define _LANG_DESC_IS_MAIN_SET_PIECE "   This item is the main piece of the '" + \
              main_piece->query_piece_set_name() + "' set, made up of "