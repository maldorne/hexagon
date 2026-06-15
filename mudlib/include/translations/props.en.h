
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

#endif // _LANG_PROPS_H_EN
