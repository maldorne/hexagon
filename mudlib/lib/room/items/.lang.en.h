// door.c

// What a door is called when it is given no name of its own, and its gender.
#define _LANG_DOOR_DEFAULT_NAME "door"
#define _LANG_DOOR_DEFAULT_GENDER 0

// The door named with its way, without article; `dir` in scope ("east gate").
#define _LANG_DOOR_NOUN dir + " " + door_name
#define _LANG_DOOR_NOUN_FOR(d) (d + " " + door_name)

// with an article, capitalised, and the verb agreeing in number
#define _LANG_DOOR_THE(d) capitalize(query_door_phrase(d))
#define _LANG_DOOR_IS ((!number) ? "is" : "are")
#define _LANG_DOOR_S ((!number) ? "s" : "")

#define _LANG_DOOR_LONG "It's a door.\n"
#define _LANG_DOOR_OPEN_ACTIONS ({ "open" })
#define _LANG_DOOR_CLOSE_ACTIONS ({ "close" })
#define _LANG_DOOR_LOCK_ACTIONS ({ "lock" })
#define _LANG_DOOR_UNLOCK_ACTIONS ({ "unlock" })
#define _LANG_DOOR_NOT_DEAD "Being dead you can't do such things.\n"

#define _LANG_DOOR_IS_CLOSED _LANG_DOOR_THE(dest) + " " + _LANG_DOOR_IS + " closed.\n"
#define _LANG_DOOR_IS_LOCKED _LANG_DOOR_THE(dest) + " " + _LANG_DOOR_IS + " locked.\n"

#define _LANG_DOOR_USE_TO_UNLOCK "You use your " + obs[i]->short() + ".\n"

#define _LANG_DOOR_NOT_LOCKED "Is not locked.\n"
#define _LANG_DOOR_CANNOT_BE_LOCKED "Cannot be locked.\n"

#define _LANG_DOOR_OPEN_ALREADY _LANG_DOOR_THE(dest) + " " + _LANG_DOOR_IS + " already open.\n"

#define _LANG_DOOR_YOU_OPEN "You open " + query_door_phrase(door) + ".\n"
#define _LANG_DOOR_PLAYER_OPENS ob->query_cap_name() + " opens " + query_door_phrase(door) + ".\n"
#define _LANG_DOOR_SOMEBODY_OPENS "Somebody opens " + query_door_phrase(door) + " from the other side.\n"

#define _LANG_DOOR_BROKEN _LANG_DOOR_THE(dest) + " " + _LANG_DOOR_IS + " broken.\n"

#define _LANG_DOOR_VERB_TO_LOCK_USED FALSE

#define _LANG_DOOR_LOCK_ALREADY _LANG_DOOR_THE(dest) + " " + _LANG_DOOR_IS + " already locked.\n"

#define _LANG_DOOR_NEED_A_KEY "You do not have the necessary key.\n"
#define _LANG_DOOR_USE_TO_LOCK "You use your " + obs[i]->short() + ".\n"

#define _LANG_DOOR_OTHERS_LOCK "You can hear the lock in " + query_door_phrase(dest) + ".\n"
#define _LANG_DOOR_OTHER_SIDE_LOCK "You can hear the lock in " + query_door_phrase(dir_other_side) + ".\n"

#define _LANG_DOOR_CLOSED_ALREADY _LANG_DOOR_THE(dest) + " " + _LANG_DOOR_IS + " already closed.\n"

#define _LANG_DOOR_YOU_CLOSE "You close " + query_door_phrase(door) + ".\n"
#define _LANG_DOOR_PLAYER_CLOSES ob->query_cap_name() + " closes " + query_door_phrase(door) + ".\n"
#define _LANG_DOOR_SOMEBODY_CLOSES "Somebody closes " + query_door_phrase(door) + " from the other side.\n"

#define _LANG_DOOR_BREAKS _LANG_DOOR_THE(dest) + " break" + _LANG_DOOR_S + " into pieces.\n"

#define _LANG_DOOR_HEALTH_STATUSES ({ "In perfect shape", "A little bit damaged", \
                                   "Not in good shape", "In bad shape", \
                                   "Almost broken", "Is broken" })

#define _LANG_DOOR_OPENS _LANG_DOOR_THE(dest) + " open" + _LANG_DOOR_S + " slowly.\n"
#define _LANG_DOOR_CLOSES _LANG_DOOR_THE(dest) + " close" + _LANG_DOOR_S + " slowly.\n"

// item.c

#define _LANG_ITEM_NOTHING_IMPORTANT "You don't see anything important.\n"
#define _LANG_ITEM_ERROR "Error in the object, report it to a coder.\n"

#define _LANG_DOOR_OPEN_AS_FAMILY "You are of the house: the door knows you.\n"
