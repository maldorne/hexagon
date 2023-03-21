
// user.c

#define _LANG_DISCONNECTED "Disconnected.\n"
#define _LANG_LAST_CONNECTION "Your last connection was from '"+last_on_from+"'.\n"
#define _LANG_USER_COMMAND_TOO_LONG "Command too long - processing anyway.\n"

// monster.c

#define _LANG_MONSTER_CANNOT_CLONE query_cap_name() + " tries to hold " + query_possessive() + \
              " " + ob->query_name() + " but in the end " + query_pronoun() + " drops it.\n"

// room.c

#define _LANG_DEFAULT_SIGN_NAME "sign"
#define _LANG_NO_EXITS "no exits"

#define _LANG_ROOM_TOO_DARK "It is too dark to see anything."
#define _LANG_ROOM_LIGHT_DEF "Hmm, you can't see very well. " + dark_mess + "\n"
#define _LANG_ROOM_LIGHT_2 "You can't see much.\n"+ exit_string
#define _LANG_ROOM_LIGHT_3 "It's too hard to see in this darkness.\n"+::short(1)+".\n"+exit_string
#define _LANG_ROOM_LIGHT_4 "You are blinded by the light.\n"+::short(0)+".\n"+exit_string
#define _LANG_ROOM_LIGHT_5 "The light is really blinding.\n"+exit_string
#define _LANG_ROOM_LIGHT_6 "You are blinded by the light.\n"

// language.c

#define _LANG_LANGUAGE_DEFAULT_DESC "This is the normal language spoken by most creatures.\n"

// player.c

#define _LANG_PLAYER_SAVING "Saving...\n"
#define _LANG_PLAYER_NO_GUEST_SAVE "Oops, guests can't save...\n"
#define _LANG_PLAYER_SAVE_ACTIONS ({ "save" })
#define _LANG_PLAYER_NOT_IN_CONDITIONS "You are not in conditions to run away.\n"
#define _LANG_PLAYER_SEE_FEET_RUNNING "You see your feet running before you.\n"
#define _LANG_PLAYER_FAILED_ESCAPE "You tried to escape, but you couldn't find any exit.\n"
#define _LANG_PLAYER_PLURAL "persons that look like " + str

// item.c

#define _LANG_ITEM_BREAKS "Your " + query_name() + " breaks in a thousand pieces!\n"

// armour.c

#define _LANG_DESC_IS_SET_PIECE "   This item is part of the set: " + \
              main_piece->query_piece_set_name() + ".\n"
#define _LANG_DESC_IS_MAIN_SET_PIECE "   This item is the main piece of the '" + \
              main_piece->query_piece_set_name() + "' set, made up of "

// weapon.c

#define _LANG_WEAPON_FUMBLE_MSG "Oh, what a blunder!\n"
#define _LANG_WEAPON_CRITICAL_MSG "Oh, what skill!\n"

#define _LANG_WEAPON_DODGE_MSG_ATTACKER " gets out of the way of your attack.\n"
#define _LANG_WEAPON_DODGE_MSG_DEFENDER " tries to hit you but you get out of the way.\n"
#define _LANG_WEAPON_DODGE_MSG_ROOM " tries to hit "
#define _LANG_WEAPON_DODGE_MSG_ROOM_END " but "+defender->query_demonstrative()+" gets out of the way.\n"
#define _LANG_WEAPON_FUMBLE_MSG_ATTACKER "You make a blunder and are unable to hit "
#define _LANG_WEAPON_FUMBLE_MSG_DEFENDER " makes a blunder and is unable to hit you.\n"
#define _LANG_WEAPON_FUMBLE_MSG_ROOM " makes a blunder and is unable to hit "
#define _LANG_WEAPON_PARRY_MSG_ATTACKER " parries your attack with "+defender->query_possessive()+" "
#define _LANG_WEAPON_PARRY_MSG_DEFENDER " tries to hit you but you parry the attack with your "
#define _LANG_WEAPON_PARRY_MSG_ROOM " tries to hit "
#define _LANG_WEAPON_PARRY_MSG_ROOM_END " but "+defender->query_demonstrative()+" parries the attack with "+defender->query_possessive()+" "
#define _LANG_WEAPON_NO_HIT "You are unable to hit "
#define _LANG_WEAPON_NOT_HIT " is not able to hit you."
#define _LANG_WEAPON_NO_HIT_ROOM " is not able to hit "
