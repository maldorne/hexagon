
// door.c

#define _LANG_DOOR_LONG "It's a door.\n"
#define _LANG_DOOR_OPEN_ACTIONS ({ "open" })
#define _LANG_DOOR_CLOSE_ACTIONS ({ "close" })
#define _LANG_DOOR_LOCK_ACTIONS ({ "lock" })
#define _LANG_DOOR_UNLOCK_ACTIONS ({ "unlock" })
#define _LANG_DOOR_NOT_DEAD "Being dead you can't do such things.\n"

#define _LANG_DOOR_IS_LOCKED "The " + dest  + " exit is locked.\n"
#define _LANG_DOOR_IS_LOCKED_KNOWN "The " + dest  + " exit is locked.\n"
#define _LANG_DOOR_IS_LOCKED_CUSTOM "The " + dest + ((!number) ? " is locked" : " are locked") + ".\n"

#define _LANG_DOOR_USE_TO_UNLOCK "You use your " + obs[i]->short() + ".\n"

#define _LANG_DOOR_NOT_LOCKED "Is not locked.\n"
#define _LANG_DOOR_CANNOT_BE_LOCKED "Cannot be locked.\n"

#define _LANG_DOOR_OPEN_ALREADY "The " + dest + " door is already open.\n"
#define _LANG_DOOR_OPEN_ALREADY_KNOWN "The " + dest + " door is already open.\n"
#define _LANG_DOOR_OPEN_ALREADY_CUSTOM "The " + dest + " " + ((!number) ? "is" : "are") + " already open.\n"

#define _LANG_DOOR_YOU_OPEN "You open the " + door + " door.\n"
#define _LANG_DOOR_YOU_OPEN_KNOWN "You open the " + door + " door.\n"
#define _LANG_DOOR_YOU_OPEN_CUSTOM "You open the " + door + ".\n"
#define _LANG_DOOR_PLAYER_OPENS ob->query_cap_name() + " opens the " + door + " door.\n"
#define _LANG_DOOR_PLAYER_OPENS_KNOWN ob->query_cap_name() + " opens the " + door + " door.\n"
#define _LANG_DOOR_PLAYER_OPENS_CUSTOM ob->query_cap_name() + " opens the " + door + ".\n"
#define _LANG_DOOR_SOMEBODY_OPENS "Somebody opens the " + door + " door from the other side.\n"
#define _LANG_DOOR_SOMEBODY_OPENS_KNOWN "Somebody opens the " + door + " door from the other side.\n"
#define _LANG_DOOR_SOMEBODY_OPENS_CUSTOM "Somebody opens the " + door + " from the other side.\n"

#define _LANG_DOOR_BROKEN "The " + dest + " door is broken.\n"
#define _LANG_DOOR_BROKEN_KNOWN "The " + dest + " door is broken.\n"
#define _LANG_DOOR_BROKEN_CUSTOM "The " + dest + " " + ((!number) ? "is" : "are") + " broken.\n"

#define _LANG_DOOR_VERB_TO_LOCK_USED FALSE

#define _LANG_DOOR_LOCK_ALREADY "The " + dest + " door is already locked.\n"
#define _LANG_DOOR_LOCK_ALREADY_KNOWN "The " + dest + " door is already locked.\n"
#define _LANG_DOOR_LOCK_ALREADY_CUSTOM "The " + dest + " " + ((!number) ? "is" : "are") + " already locked.\n"

#define _LANG_DOOR_NEED_A_KEY "You do not have the necessary key.\n"
#define _LANG_DOOR_USE_TO_LOCK "You use your " + obs[i]->short() + ".\n"

#define _LANG_DOOR_OTHERS_LOCK "You can hear the lock in the " + dest + " door.\n"
#define _LANG_DOOR_OTHERS_LOCK_KNOWN "You can hear the lock in the " + dest + " door.\n"
#define _LANG_DOOR_OTHERS_LOCK_CUSTOM "You can hear the lock in the " + dest + ".\n"

#define _LANG_DOOR_OTHER_SIDE_LOCK "You can hear the lock in the " + dir_other_side + " door.\n"
#define _LANG_DOOR_OTHER_SIDE_LOCK_KNOWN "You can hear the lock in the " + dir_other_side + " door.\n"
#define _LANG_DOOR_OTHER_SIDE_LOCK_CUSTOM "You can hear the lock in the " + dest + ".\n"

#define _LANG_DOOR_CLOSED_ALREADY "The " + dest + " door is already closed.\n"
#define _LANG_DOOR_CLOSED_ALREADY_KNOWN "The " + dest + " door is already closed.\n"
#define _LANG_DOOR_CLOSED_ALREADY_CUSTOM "The " + dest + " " + ((!number) ? "is" : "are") + " already closed.\n"

#define _LANG_DOOR_YOU_CLOSE "You close the " + door + " door.\n"
#define _LANG_DOOR_YOU_CLOSE_KNOWN "You close the " + door + " door.\n"
#define _LANG_DOOR_YOU_CLOSE_CUSTOM "You close the " + door + ".\n"
#define _LANG_DOOR_PLAYER_CLOSES ob->query_cap_name() + " closes the " + door + " door.\n"
#define _LANG_DOOR_PLAYER_CLOSES_KNOWN ob->query_cap_name() + " closes the " + door + " door.\n"
#define _LANG_DOOR_PLAYER_CLOSES_CUSTOM ob->query_cap_name() + " closes the " + door + ".\n"
#define _LANG_DOOR_SOMEBODY_CLOSES "Somebody closes the " + door + " door from the other side.\n"
#define _LANG_DOOR_SOMEBODY_CLOSES_KNOWN "Somebody closes the " + door + " door from the other side.\n"
#define _LANG_DOOR_SOMEBODY_CLOSES_CUSTOM "Somebody closes the " + door + " from the other side.\n"

#define _LANG_DOOR_BREAKS "The " + dest + " door breaks into pieces.\n"
#define _LANG_DOOR_BREAKS_KNOWN  "The " + dest + " door breaks into pieces.\n"
#define _LANG_DOOR_BREAKS_CUSTOM "The " + dest + " break" + ((!number) ? "s" : "") + " into pieces.\n"

#define _LANG_DOOR_HEALTH_STATUSES ({ "In perfect shape", "A little bit damaged", \
                                   "Not in good shape", "In bad shape", \
                                   "Almost broken", "Is broken" })

#define _LANG_DOOR_OPENS "The " + dest + " door opens slowly.\n"
#define _LANG_DOOR_OPENS_KNOWN "The " + dest + " door opens slowly.\n"
#define _LANG_DOOR_OPENS_CUSTOM "The " + dest + " opens slowly.\n"
#define _LANG_DOOR_CLOSES "The " + dest + " door closes slowly.\n"
#define _LANG_DOOR_CLOSES_KNOWN "The " + dest + " door closes slowly.\n"
#define _LANG_DOOR_CLOSES_CUSTOM "The " + dest + " closes slowly.\n"

// item.c

#define _LANG_ITEM_NOTHING_IMPORTANT "You don't see anything important.\n"
#define _LANG_ITEM_ERROR "Error in the object, report it to a coder.\n"
