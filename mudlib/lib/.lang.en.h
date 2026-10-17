
// user.c

#define _LANG_DISCONNECTED "Disconnected.\n"
#define _LANG_USER_TAKEN_OVER "You have logged in with your character from another connection; this one closes.\n"
#define _LANG_IDLE_KICKED "You have been idle too long; disconnecting.\n"
#define _LANG_LAST_CONNECTION "Your last connection was from '"+last_on_from+"'.\n"
#define _LANG_USER_COMMAND_TOO_LONG "Command too long - processing anyway.\n"
#define _LANG_USER_PLAYER_RESTORE_ERROR "Your avatar has been destroyed and could not be restored.\n" + \
                        "Your connection will be terminated so you can log in again.\n"

// monster.c

#define _LANG_MONSTER_CANNOT_CLONE query_cap_name() + " tries to hold " + query_possessive() + \
              " " + ob->query_name() + " but in the end " + query_pronoun() + " drops it.\n"

// room.c

// location.c

// Header of the coder-only banner listing the location's attached
// components. Followed by the comma-separated component types.
#define _LANG_LOCATION_COMPONENTS_HEADER "Components"

// language.c

#define _LANG_LANGUAGE_DEFAULT_DESC "This is the normal language spoken by most creatures.\n"

// player.c

#define _LANG_PLAYER_SAVING "Saving...\n"
#define _LANG_PLAYER_SAVE_ACTIONS ({ "save" })
#define _LANG_PLAYER_PLURAL "persons that look like " + str

// item.c

#define _LANG_ITEM_BREAKS "Your " + query_name() + " breaks in a thousand pieces!\n"
#define _LANG_ITEM_HAS_HELP "This object has a help message available.\n"

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

// food.c

#define _LANG_FOOD_EAT_VERBS ({ "eat" })
#define _LANG_FOOD_DRINK_VERBS ({ "drink" })
#define _LANG_FOOD_CONSUME_WHAT capitalize(query_verb()) + " what?\n"
#define _LANG_FOOD_DEFAULT_EAT_ME "You eat some of the " + query_name() + ".\n"
#define _LANG_FOOD_DEFAULT_EAT_OTHERS "eats some of the " + query_name() + ".\n"
#define _LANG_FOOD_DEFAULT_DRINK_ME "You take a sip of the " + query_name() + ".\n"
#define _LANG_FOOD_DEFAULT_DRINK_OTHERS "takes a sip of the " + query_name() + ".\n"
#define _LANG_FOOD_SPOILED_ME "It has gone off, and your stomach turns.\n"
#define _LANG_FOOD_FINISHED "There is nothing left of the " + query_name() + ".\n"
#define _LANG_FOOD_ROTTED_HOLDER "What is left of the " + query_name() + " falls apart in your fingers.\n"
#define _LANG_FOOD_ROTTED_HOLDER_ROOM "throws away what is left of the " + query_name() + ", long rotten.\n"
#define _LANG_FOOD_ROTTED_ROOM "What is left of the " + query_name() + " has rotted away.\n"
#define _LANG_FOOD_COND_FRESH "It is freshly made.\n"
#define _LANG_FOOD_COND_PASSABLE "It is still good.\n"
#define _LANG_FOOD_COND_STALE "It is about to go off.\n"
#define _LANG_FOOD_COND_SPOILED "It has gone off.\n"
#define _LANG_FOOD_LONG_CONDITION "   " + cond_string()
#define _LANG_FOOD_LONG_BITES "   Good for " + number_as_string(query_bites_left()) + \
  ((query_bites_left() == 1) ? " more bite" : " more bites") + ".\n"

// wearable bags: what one holds
#define _LANG_WEARABLE_BAG_CONTAINS " contains:\n"

// deity.c -- $god$ is the deity's name, $noun$ god or goddess

#define _LANG_DEITY_NOUN_MALE "god"
#define _LANG_DEITY_NOUN_FEMALE "goddess"
#define _LANG_DEITY_REFUSE_RACE "$god$ does not take your people among the faithful.\n"
#define _LANG_DEITY_REFUSE_LINEAGE "$god$ does not take those of your lineage among the faithful.\n"
#define _LANG_DEITY_REFUSE_TOO_EVIL "There is too much evil in your soul for $god$ to take you.\n"
#define _LANG_DEITY_REFUSE_TOO_GOOD "There is too much good in your soul for $god$ to take you.\n"
#define _LANG_DEITY_REFUSE_GUILD_REQUIRED "$god$ only takes those of certain guilds, and yours is not one of them.\n"
#define _LANG_DEITY_REFUSE_GUILD_FORBIDDEN "$god$ takes nobody of your guild among the faithful.\n"
#define _LANG_DEITY_REFUSE_CITIZENSHIP "Your people do not allow the worship of $god$: you cannot worship that $noun$.\n"
#define _LANG_DEITY_OFFERING_ACCEPTED "Your offering vanishes. You feel that $god$ has accepted it.\n"
#define _LANG_DEITY_PRAYER_WORDS ([ \
      "heal"     : ({ "healing", "heal", "cure" }), \
      "power"    : ({ "power" }), \
      "blessing" : ({ "blessing", "bless" }), \
      "strength" : ({ "strength" }), \
      "wisdom"   : ({ "wisdom" }), \
    ])
#define _LANG_DEITY_PRAYER_SCOLDED "$god$ tells you: Stop bothering me, or you will feel my wrath!\n"
#define _LANG_DEITY_PRAYER_CURSE "$god$ tells you: I have had enough of your nuisance. Do not " + \
      "bother me again. You are now cursed in my eyes.\n"
#define _LANG_DEITY_PRAYER_HEARD "Your prayers are heard!\n"
#define _LANG_DEITY_PRAYER_UNANSWERED "Your prayers were not answered.\n"
#define _LANG_DEITY_PRAYER_HEALED "Your wounds close: you heal " + amount + " hit points.\n"
#define _LANG_DEITY_PRAYER_POWERED "You feel more powerful as you recover " + amount + \
      " energy points.\n"
#define _LANG_DEITY_PRAYER_BLESSED "You have been blessed!\n"
#define _LANG_DEITY_PRAYER_STRONG "A divine strength runs through your veins.\n"
#define _LANG_DEITY_PRAYER_WISE "You feel a great wisdom.\n"
