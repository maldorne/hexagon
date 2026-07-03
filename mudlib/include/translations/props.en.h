
// props.en.h — English strings for the location-props subsystem.
//
// Grouped by prop type. Type-agnostic dispatcher strings live at the
// top. Each per-type section bundles ids, noun, descriptions, verb
// arrays, action messages, state suffixes — everything the prop needs.
//
// Player code should `#include <translations/props.h>` and let the
// driver rewrite the include to .en.h / .es.h according to
// GLOBAL_COMPILE_LANG. See /lib/core/driver.c::include_file.

#ifndef _LANG_PROPS_H_EN
#define _LANG_PROPS_H_EN

// ************************************************************
//  Type-agnostic dispatcher strings.
// ************************************************************

// Section header rendered at the start of the props line in look output.
#define _LANG_PROPS_SECTION_HEADER    "You see"

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

// Appended under the long description on `look <prop>`. Followed by
// the comma-separated verb list. "You can: light, extinguish."
#define _LANG_PROPS_YOU_CAN           "You can: "
#define _LANG_PROPS_NO_ACTIONS        "Nothing to do with this one."

// Noun-phrase templates consumed by the props component's hook_long
// to describe attached instances in the location body. Variables in
// scope at the call site: count_word, noun, material_phrase. Order
// differs per language; see props.es.h for the mirror.
#define _LANG_PROPS_NOUN_PHRASE             count_word + " " + material_phrase + " " + noun
#define _LANG_PROPS_NOUN_PHRASE_NO_MATERIAL count_word + " " + noun

// ************************************************************
//  chair
// ************************************************************
#define _LANG_PROP_CHAIR_ID             "chair"
#define _LANG_PROP_CHAIR_ID_ALIAS_1     "seat"

#define _LANG_PROP_CHAIR_NOUN           "chair"
#define _LANG_PROP_CHAIR_NOUN_PLURAL    "chairs"

#define _LANG_PROP_CHAIR_SHORT          "a wooden chair"
#define _LANG_PROP_CHAIR_LONG           "A sturdy wooden chair, sized for one."

#define _LANG_PROP_CHAIR_SUFFIX_TIPPED      " (tipped over)"
#define _LANG_PROP_CHAIR_SUFFIX_OCCUPIED    " (%s is sitting on it)"

#define _LANG_PROP_CHAIR_LONG_TIPPED        " It lies on its side."
#define _LANG_PROP_CHAIR_LONG_OCCUPIED      " %s is sitting on it."

#define _LANG_PROP_CHAIR_SIT_VERBS          ({ "sit" })
#define _LANG_PROP_CHAIR_STAND_VERBS        ({ "stand", "rise" })
#define _LANG_PROP_CHAIR_TIP_VERBS          ({ "tip" })
#define _LANG_PROP_CHAIR_RIGHT_VERBS        ({ "right" })

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
//  table
// ************************************************************
#define _LANG_PROP_TABLE_ID             "table"

#define _LANG_PROP_TABLE_NOUN           "table"
#define _LANG_PROP_TABLE_NOUN_PLURAL    "tables"

#define _LANG_PROP_TABLE_SHORT          "a wooden table"
#define _LANG_PROP_TABLE_LONG           "A long wooden table, scarred by years of use."

#define _LANG_PROP_TABLE_SMELL_VERBS    ({ "smell", "sniff" })
#define _LANG_PROP_TABLE_LEAN_VERBS     ({ "lean" })
#define _LANG_PROP_TABLE_CLIMB_VERBS    ({ "climb" })

#define _LANG_PROP_TABLE_SMELL_ME       "The table smells of polish and old wood."
#define _LANG_PROP_TABLE_LEAN_ME        "You lean on the table."
#define _LANG_PROP_TABLE_LEAN_OTHERS    "%s leans on the table."
#define _LANG_PROP_TABLE_CLIMB_ME       "You climb onto the table."
#define _LANG_PROP_TABLE_CLIMB_OTHERS   "%s climbs onto the table."

// ************************************************************
//  statue
// ************************************************************
#define _LANG_PROP_STATUE_ID            "statue"
#define _LANG_PROP_STATUE_ID_ALIAS_1    "idol"

#define _LANG_PROP_STATUE_NOUN          "statue"
#define _LANG_PROP_STATUE_NOUN_PLURAL   "statues"

#define _LANG_PROP_STATUE_SHORT         "a metal statue"
#define _LANG_PROP_STATUE_LONG          "Forged in metal, it represents the God Lummen, carrier of the souls and benefactor of the priests."

#define _LANG_PROP_STATUE_PRAY_VERBS    ({ "pray" })

#define _LANG_PROP_STATUE_PRAY_ME       "You kneel before the statue and offer a silent prayer."
#define _LANG_PROP_STATUE_PRAY_OTHERS   "%s kneels before the statue and prays in silence."

// ************************************************************
//  altar
// ************************************************************
#define _LANG_PROP_ALTAR_ID             "altar"

#define _LANG_PROP_ALTAR_NOUN           "altar"
#define _LANG_PROP_ALTAR_NOUN_PLURAL    "altars"

#define _LANG_PROP_ALTAR_SHORT          "a stone altar"
#define _LANG_PROP_ALTAR_LONG           "A modest altar of polished stone."

#define _LANG_PROP_ALTAR_PRAY_VERBS     ({ "pray" })

#define _LANG_PROP_ALTAR_PRAY_ME        "You bow your head before the altar."
#define _LANG_PROP_ALTAR_PRAY_OTHERS    "%s bows their head before the altar."

// ************************************************************
//  fountain
// ************************************************************
#define _LANG_PROP_FOUNTAIN_ID          "fountain"
#define _LANG_PROP_FOUNTAIN_ID_ALIAS_1  "water fountain"

#define _LANG_PROP_FOUNTAIN_NOUN        "fountain"
#define _LANG_PROP_FOUNTAIN_NOUN_PLURAL "fountains"

#define _LANG_PROP_FOUNTAIN_SHORT       "a stone fountain"
#define _LANG_PROP_FOUNTAIN_LONG        "A stone fountain. Clear water bubbles gently in its basin."

#define _LANG_PROP_FOUNTAIN_DRINK_VERBS ({ "drink" })
#define _LANG_PROP_FOUNTAIN_SMELL_VERBS ({ "smell", "sniff" })

#define _LANG_PROP_FOUNTAIN_DRINK_ME    "You drink some fresh water from the fountain."
#define _LANG_PROP_FOUNTAIN_DRINK_OTHERS "%s drinks some fresh water from the fountain."
#define _LANG_PROP_FOUNTAIN_SMELL_ME    "The water smells faintly of moss and cold stone."

// ************************************************************
//  fireplace
// ************************************************************
#define _LANG_PROP_FIREPLACE_ID         "fireplace"
#define _LANG_PROP_FIREPLACE_ID_ALIAS_1 "hearth"

#define _LANG_PROP_FIREPLACE_NOUN       "fireplace"
#define _LANG_PROP_FIREPLACE_NOUN_PLURAL "fireplaces"

#define _LANG_PROP_FIREPLACE_SHORT      "a small fireplace"
#define _LANG_PROP_FIREPLACE_LONG       "A small fireplace built of stone."

#define _LANG_PROP_FIREPLACE_SUFFIX_LIT " (lit)"

#define _LANG_PROP_FIREPLACE_LONG_LIT   " Flames crackle inside, casting warm light across the room."
#define _LANG_PROP_FIREPLACE_LONG_UNLIT " The ashes are cold; nobody has lit it in a while."

#define _LANG_PROP_FIREPLACE_LIGHT_VERBS ({ "light", "ignite" })
// "put out" intentionally omitted — DGD's add_action registers
// single-word verbs only ("put" alone would collide with the global
// `put` cmd). Re-add as a properly composed verb when we handle
// multi-word verb dispatch.
#define _LANG_PROP_FIREPLACE_EXT_VERBS  ({ "extinguish", "douse" })

#define _LANG_PROP_FIREPLACE_LIGHT_ME       "You light the fireplace. Flames flicker to life."
#define _LANG_PROP_FIREPLACE_LIGHT_OTHERS   "%s lights the fireplace."
#define _LANG_PROP_FIREPLACE_EXT_ME         "You extinguish the fire. The hearth goes dark."
#define _LANG_PROP_FIREPLACE_EXT_OTHERS     "%s extinguishes the fire."
#define _LANG_PROP_FIREPLACE_ALREADY_LIT    "The fireplace is already lit."
#define _LANG_PROP_FIREPLACE_NOT_LIT        "The fireplace is not lit."

// ************************************************************
//  bar (tavern counter)
// ************************************************************
#define _LANG_PROP_BAR_ID               "bar"
#define _LANG_PROP_BAR_ID_ALIAS_1       "counter"

#define _LANG_PROP_BAR_NOUN             "bar"
#define _LANG_PROP_BAR_NOUN_PLURAL      "bars"

#define _LANG_PROP_BAR_SHORT            "a long wooden bar"
#define _LANG_PROP_BAR_LONG             "A long wooden bar runs along the wall, its surface scarred by years of mugs and elbows. It is not very clean."

#define _LANG_PROP_BAR_LEAN_VERBS       ({ "lean" })
#define _LANG_PROP_BAR_SMELL_VERBS      ({ "smell", "sniff" })

#define _LANG_PROP_BAR_LEAN_ME          "You lean against the bar."
#define _LANG_PROP_BAR_LEAN_OTHERS      "%s leans against the bar."
#define _LANG_PROP_BAR_SMELL_ME         "The bar reeks of stale ale and old wood."

#endif // _LANG_PROPS_H_EN
