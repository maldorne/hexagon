
// props.en.h — English strings for the location-props subsystem.
//
// Only type-agnostic strings live here: the section header, the list
// joiners, and the generic dispatcher messages. Per-type strings
// (_LANG_PROP_CHAIR_*, _LANG_PROP_TABLE_*, ...) ship with their
// respective type entries / blueprints.
//
// Player code should `#include <translations/props.h>` and let the
// driver rewrite the include to .en.h / .es.h according to
// GLOBAL_COMPILE_LANG. See /lib/core/driver.c::include_file.

#ifndef _LANG_PROPS_H_EN
#define _LANG_PROPS_H_EN

// Section header rendered at the start of the props line in look output.
#define _LANG_PROPS_SECTION_HEADER    "You see:"

// List joiners — used to compose the section line for multiple instances.
// Final form: "<header> <a><sep><b><sep_last><c>."
#define _LANG_PROPS_LIST_SEPARATOR    ", "
#define _LANG_PROPS_LIST_AND          " and "
#define _LANG_PROPS_LIST_TERMINATOR   "."

// Dispatcher messages — used by /lib/location/components/props.c
// when do_prop_action cannot satisfy the verb.
#define _LANG_PROPS_NO_TARGET         "There is nothing like that here."
#define _LANG_PROPS_AMBIGUOUS_TARGET  "Which one do you mean?"
#define _LANG_PROPS_REMOVED_ACTION    "You cannot do that with this one."
#define _LANG_PROPS_NO_HANDLER        "Nothing happens."

// ************************************************************
//  Per-type strings — chair
// ************************************************************
#define _LANG_PROP_CHAIR_ID             "chair"
#define _LANG_PROP_CHAIR_ID_ALIAS_1     "seat"
#define _LANG_PROP_CHAIR_ID_ALIAS_2     "wooden chair"
#define _LANG_PROP_CHAIR_SHORT          "a wooden chair"
#define _LANG_PROP_CHAIR_LONG           "A sturdy wooden chair, sized for one."

#define _LANG_PROP_CHAIR_SUFFIX_TIPPED      " (tipped over)"
#define _LANG_PROP_CHAIR_SUFFIX_OCCUPIED    " (%s is sitting on it)"

#define _LANG_PROP_CHAIR_SIT_ME             "You sit on the chair."
#define _LANG_PROP_CHAIR_SIT_OTHERS         "%s sits on the chair."
#define _LANG_PROP_CHAIR_STAND_ME           "You stand up."
#define _LANG_PROP_CHAIR_STAND_OTHERS       "%s stands up."
#define _LANG_PROP_CHAIR_TIP_ME             "You tip the chair over."
#define _LANG_PROP_CHAIR_TIP_OTHERS         "%s tips the chair over."
#define _LANG_PROP_CHAIR_RIGHT_ME           "You right the chair."
#define _LANG_PROP_CHAIR_RIGHT_OTHERS       "%s rights the chair."

#define _LANG_PROP_CHAIR_CANT_SIT           "You can't sit on this chair right now."
#define _LANG_PROP_CHAIR_OCCUPIED_BY        "%s is already sitting on it."
#define _LANG_PROP_CHAIR_NOT_SITTING        "You are not sitting on this chair."
#define _LANG_PROP_CHAIR_NAILED_OR_TIPPED   "You cannot tip this chair."
#define _LANG_PROP_CHAIR_NOT_TIPPED         "The chair is not tipped over."

// ************************************************************
//  Per-type strings — table
// ************************************************************
#define _LANG_PROP_TABLE_ID             "table"
#define _LANG_PROP_TABLE_ID_ALIAS_1     "wooden table"
#define _LANG_PROP_TABLE_SHORT          "a wooden table"
#define _LANG_PROP_TABLE_LONG           "A long wooden table, scarred by years of use."

#define _LANG_PROP_TABLE_SMELL_ME       "The table smells of polish and old wood."
#define _LANG_PROP_TABLE_LEAN_ME        "You lean on the table."
#define _LANG_PROP_TABLE_LEAN_OTHERS    "%s leans on the table."
#define _LANG_PROP_TABLE_CLIMB_ME       "You climb onto the table."
#define _LANG_PROP_TABLE_CLIMB_OTHERS   "%s climbs onto the table."

#endif // _LANG_PROPS_H_EN
