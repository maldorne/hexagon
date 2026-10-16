
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

// The read action given to every type without one of its own: what is
// written on an instance (its overrides.inscription).
#define _LANG_PROPS_READ_VERBS        ({ "read" })
#define _LANG_PROPS_READ_ME           "You step closer and read what is written."
#define _LANG_PROPS_READ_OTHERS       "$mcname$ steps closer and reads something."

// Appended under the long description on `look <prop>`. Followed by
// the comma-separated verb list. "You can: light, extinguish."
#define _LANG_PROPS_YOU_CAN           "You can: "

// ************************************************************
//  scenery: whatever each instance says it is (short, ids, description)
// ************************************************************
#define _LANG_PROP_SCENERY_SHORT        "something"
#define _LANG_PROP_SCENERY_LONG         "There is nothing special about it."

// Noun-phrase templates consumed by the props component's hook_long
// to describe attached instances in the location body. Variables in
// scope at the call site: count_word, noun, material_phrase. Order
// differs per language; see props.es.h for the mirror.
#define _LANG_PROPS_NOUN_PHRASE             count_word + " " + material_phrase + " " + noun
#define _LANG_PROPS_NOUN_PHRASE_NO_MATERIAL count_word + " " + noun
// the noun with the instance's colour: "red rune"
#define _LANG_PROPS_NOUN_WITH_COLOUR        colour + " " + noun

// Material-composed id template used by the props component to
// enumerate its parser aliases. Variables in scope: id, material_phrase.
// English glues the material adjective before the noun ("wooden
// chair"); Spanish appends the material phrase after ("silla de
// madera"). Same variable order, distinct concatenation per language.
#define _LANG_PROPS_ID_WITH_MATERIAL        material_phrase + " " + id

// ************************************************************
//  milestone
// ************************************************************
#define _LANG_PROP_MILESTONE_ID           "milestone"
#define _LANG_PROP_MILESTONE_ID_ALIAS_1   "marker"
#define _LANG_PROP_MILESTONE_ID_ALIAS_2   "waymarker"

#define _LANG_PROP_MILESTONE_NOUN         "milestone"
#define _LANG_PROP_MILESTONE_NOUN_PLURAL  "milestones"

#define _LANG_PROP_MILESTONE_SHORT        "a $material_phrase$ milestone"
#define _LANG_PROP_MILESTONE_LONG         "A $material_phrase$ milestone marks the edge of the road."

// ************************************************************
//  arch
// ************************************************************
#define _LANG_PROP_ARCH_ID                "arch"
#define _LANG_PROP_ARCH_ID_ALIAS_1        "archway"

#define _LANG_PROP_ARCH_NOUN              "arch"
#define _LANG_PROP_ARCH_NOUN_PLURAL       "arches"

#define _LANG_PROP_ARCH_SHORT             "a $material_phrase$ arch"
#define _LANG_PROP_ARCH_LONG              "A $material_phrase$ arch stands over the passage."

// ************************************************************
//  pergola
// ************************************************************
#define _LANG_PROP_PERGOLA_ID                "pergola"
#define _LANG_PROP_PERGOLA_ID_ALIAS_1        "arbour"
#define _LANG_PROP_PERGOLA_ID_ALIAS_2        "arbor"

#define _LANG_PROP_PERGOLA_NOUN              "pergola"
#define _LANG_PROP_PERGOLA_NOUN_PLURAL       "pergolas"

#define _LANG_PROP_PERGOLA_SHORT             "a $material_phrase$ pergola"
#define _LANG_PROP_PERGOLA_LONG              "A $material_phrase$ pergola on carved pillars shades the ground, climbing plants twisting around the pillars and up to the top."

// ************************************************************
//  chair
// ************************************************************
#define _LANG_PROP_CHAIR_ID             "chair"
#define _LANG_PROP_CHAIR_ID_ALIAS_1     "seat"

#define _LANG_PROP_CHAIR_NOUN           "chair"
#define _LANG_PROP_CHAIR_NOUN_PLURAL    "chairs"

#define _LANG_PROP_CHAIR_SHORT          "a $material_phrase$ chair"
#define _LANG_PROP_CHAIR_LONG           "A sturdy $material_phrase$ chair, sized for one."

#define _LANG_PROP_CHAIR_SUFFIX_TIPPED         " (tipped over)"
#define _LANG_PROP_CHAIR_SUFFIX_TIPPED_PLURAL  " (tipped over)"
#define _LANG_PROP_CHAIR_SUFFIX_OCCUPIED       " (%s is sitting on it)"

#define _LANG_PROP_CHAIR_LONG_TIPPED        " It lies on its side."
#define _LANG_PROP_CHAIR_LONG_OCCUPIED      " %s is sitting on it."
#define _LANG_PROP_CHAIR_LONG_TIPPED_PLURAL " They lie on their sides."

#define _LANG_PROP_CHAIR_SIT_VERBS          ({ "sit" })
#define _LANG_PROP_CHAIR_STAND_VERBS        ({ "stand", "rise" })
#define _LANG_PROP_CHAIR_TIP_VERBS          ({ "tip" })
#define _LANG_PROP_CHAIR_RIGHT_VERBS        ({ "right" })

#define _LANG_PROP_CHAIR_SIT_ME             "You sit on the chair."
#define _LANG_PROP_CHAIR_SIT_OTHERS         "$mcname$ sits on the chair."
#define _LANG_PROP_CHAIR_STAND_ME           "You stand up."
#define _LANG_PROP_CHAIR_STAND_OTHERS       "$mcname$ stands up."
#define _LANG_PROP_CHAIR_TIP_ME             "You tip the chair over."
#define _LANG_PROP_CHAIR_TIP_OTHERS         "$mcname$ tips the chair over."
#define _LANG_PROP_CHAIR_RIGHT_ME           "You right the chair."
#define _LANG_PROP_CHAIR_RIGHT_OTHERS       "$mcname$ rights the chair."

#define _LANG_PROP_CHAIR_CANT_SIT           "You can't sit on this chair right now."
#define _LANG_PROP_CHAIR_ALREADY_SITTING "You are already sitting on that chair."
#define _LANG_PROP_CHAIR_OCCUPIED_BY        "$lastarg$ is already sitting on it."
#define _LANG_PROP_CHAIR_NOT_SITTING        "You are not sitting on this chair."
#define _LANG_PROP_CHAIR_NAILED_OR_TIPPED   "You cannot tip this chair."
#define _LANG_PROP_CHAIR_NOT_TIPPED         "The chair is not tipped over."

// ************************************************************
//  throne
// ************************************************************
#define _LANG_PROP_THRONE_ID            "throne"
#define _LANG_PROP_THRONE_ID_ALIAS_1    "seat of state"

#define _LANG_PROP_THRONE_NOUN          "throne"
#define _LANG_PROP_THRONE_NOUN_PLURAL   "thrones"

#define _LANG_PROP_THRONE_SHORT         "a $material_phrase$ throne"
#define _LANG_PROP_THRONE_LONG          "A grand, solemn $material_phrase$ throne with a tall back, made for a sovereign to preside from."

#define _LANG_PROP_THRONE_SUFFIX_OCCUPIED   " (%s is sitting on it)"
#define _LANG_PROP_THRONE_LONG_OCCUPIED     " %s is sitting on it."

#define _LANG_PROP_THRONE_SIT_VERBS         ({ "sit" })
#define _LANG_PROP_THRONE_STAND_VERBS       ({ "stand", "rise" })

#define _LANG_PROP_THRONE_SIT_ME            "You sit on the throne."
#define _LANG_PROP_THRONE_SIT_OTHERS        "$mcname$ sits on the throne."
#define _LANG_PROP_THRONE_STAND_ME          "You stand up."
#define _LANG_PROP_THRONE_STAND_OTHERS      "$mcname$ stands up."

#define _LANG_PROP_THRONE_CANT_SIT          "You can't sit on this throne right now."
#define _LANG_PROP_THRONE_ALREADY_SITTING   "You are already sitting on that throne."
#define _LANG_PROP_THRONE_OCCUPIED_BY       "$lastarg$ is already sitting on it."
#define _LANG_PROP_THRONE_NOT_SITTING       "You are not sitting on this throne."

// ************************************************************
//  table
// ************************************************************
#define _LANG_PROP_TABLE_ID             "table"

#define _LANG_PROP_TABLE_NOUN           "table"
#define _LANG_PROP_TABLE_NOUN_PLURAL    "tables"

#define _LANG_PROP_TABLE_SHORT          "a $material_phrase$ table"
#define _LANG_PROP_TABLE_LONG           "A long $material_phrase$ table, scarred by years of use."

#define _LANG_PROP_TABLE_LEAN_VERBS     ({ "lean" })
#define _LANG_PROP_TABLE_CLIMB_VERBS    ({ "climb" })

#define _LANG_PROP_TABLE_SMELL          "The table smells of polish and old wood."
#define _LANG_PROP_TABLE_LEAN_ME        "You lean on the table."
#define _LANG_PROP_TABLE_LEAN_OTHERS    "$mcname$ leans on the table."
#define _LANG_PROP_TABLE_CLIMB_ME       "You climb onto the table."
#define _LANG_PROP_TABLE_CLIMB_OTHERS   "$mcname$ climbs onto the table."

// ************************************************************
//  statue
// ************************************************************
#define _LANG_PROP_STATUE_ID            "statue"
#define _LANG_PROP_STATUE_ID_ALIAS_1    "idol"

#define _LANG_PROP_STATUE_NOUN          "statue"
#define _LANG_PROP_STATUE_NOUN_PLURAL   "statues"

#define _LANG_PROP_STATUE_SHORT         "a $material_phrase$ statue"
#define _LANG_PROP_STATUE_LONG          "A $material_phrase$ statue stands here, depicting an unknown figure."

#define _LANG_PROP_STATUE_PRAY_VERBS    ({ "pray" })

#define _LANG_PROP_STATUE_PRAY_ME       "You kneel before the statue and offer a silent prayer."
#define _LANG_PROP_STATUE_PRAY_OTHERS   "$mcname$ kneels before the statue and prays in silence."

#define _LANG_PROP_STATUE_READ_VERBS    ({ "read" })
#define _LANG_PROP_STATUE_READ_ME       "You step up to the pedestal of the statue and read what is carved on it."
#define _LANG_PROP_STATUE_READ_OTHERS   "$mcname$ steps up to the pedestal of the statue and reads something."


// ************************************************************
//  ruined statue
// ************************************************************
#define _LANG_PROP_RUINED_STATUE_ID          "statue"
#define _LANG_PROP_RUINED_STATUE_ID_ALIAS_1  "plaque"

#define _LANG_PROP_RUINED_STATUE_NOUN        "abandoned statue"
#define _LANG_PROP_RUINED_STATUE_NOUN_PLURAL "abandoned statues"

#define _LANG_PROP_RUINED_STATUE_SHORT       "an abandoned $material_phrase$ statue"
#define _LANG_PROP_RUINED_STATUE_LONG        "An old $material_phrase$ statue stands here, abandoned in the middle of nowhere. Time has worn away the figure's features and broken off an arm; it is cracked and covered in lichen, and plants and creepers grow around it and climb it up to the shoulders. On the pedestal, half hidden by the undergrowth, there is a plaque."

#define _LANG_PROP_RUINED_STATUE_SMELL       "It smells of moss and damp earth."
#define _LANG_PROP_RUINED_STATUE_FEEL        "It is cold and rough, and the creepers catch on your fingers."

#define _LANG_PROP_RUINED_STATUE_READ_VERBS  ({ "read" })
#define _LANG_PROP_RUINED_STATUE_READ_ME     "You push aside the creepers covering the pedestal and uncover the plaque."
#define _LANG_PROP_RUINED_STATUE_READ_OTHERS "$mcname$ pushes the creepers away from the foot of the statue and reads something."

// ************************************************************
//  rune
// ************************************************************
#define _LANG_PROP_RUNE_ID              "rune"
#define _LANG_PROP_RUNE_ID_ALIAS_1      "runestone"
#define _LANG_PROP_RUNE_NOUN            "rune"
#define _LANG_PROP_RUNE_NOUN_PLURAL     "runes"

#define _LANG_PROP_RUNE_SHORT           "a $colour$ rune carved in $material_name$"
#define _LANG_PROP_RUNE_LONG            "There is a $colour$ rune carved in $material_name$."
#define _LANG_PROP_RUNE_LONG_SYMBOL     " It depicts $symbol$."

#define _LANG_PROP_RUNE_SUFFIX_LIT          " that glows"
#define _LANG_PROP_RUNE_SUFFIX_LIT_PLURAL   " that glow"
#define _LANG_PROP_RUNE_LONG_LIT            " It glows with a light of its own."
#define _LANG_PROP_RUNE_LONG_LIT_PLURAL     " They glow with a light of their own."
#define _LANG_PROP_RUNE_LONG_UNLIT          " It is dark."
#define _LANG_PROP_RUNE_LONG_UNLIT_PLURAL   " They are dark."

#define _LANG_PROP_RUNE_FEEL            "The surface of the rune is cold and smooth."

#define _LANG_PROP_RUNE_LIGHT_VERBS     ({ "activate", "light" })
#define _LANG_PROP_RUNE_ALREADY_LIT     "The rune is already glowing."
#define _LANG_PROP_RUNE_LIGHT_ME        "You pass your hand over the rune and it starts to glow."
#define _LANG_PROP_RUNE_LIGHT_OTHERS    "$mcname$ passes a hand over a rune, which starts to glow."

#define _LANG_PROP_RUNE_EXT_VERBS       ({ "extinguish", "deactivate" })
#define _LANG_PROP_RUNE_NOT_LIT         "The rune is already dark."
#define _LANG_PROP_RUNE_EXT_ME          "You cover the rune with your hand and its glow goes out."
#define _LANG_PROP_RUNE_EXT_OTHERS      "$mcname$ covers a rune with a hand and its glow goes out."

#define _LANG_PROP_RUNE_READ_VERBS      ({ "read" })
#define _LANG_PROP_RUNE_READ_ME         "You step up to the rune and read what is carved on it."
#define _LANG_PROP_RUNE_READ_OTHERS     "$mcname$ steps up to a rune and reads it."

#define _LANG_PROP_RUINED_STATUE_PRAY_VERBS  ({ "pray" })
#define _LANG_PROP_RUINED_STATUE_PRAY_ME     "You kneel before the abandoned statue. Nobody seems to have done so in a long time."
#define _LANG_PROP_RUINED_STATUE_PRAY_OTHERS "$mcname$ kneels before the abandoned statue and prays in silence."

// ************************************************************
//  puddle
// ************************************************************
#define _LANG_PROP_PUDDLE_ID            "puddle"
#define _LANG_PROP_PUDDLE_ID_ALIAS_1    "pool"

#define _LANG_PROP_PUDDLE_NOUN          "puddle"
#define _LANG_PROP_PUDDLE_NOUN_PLURAL   "puddles"

#define _LANG_PROP_PUDDLE_SHORT         "a puddle"
#define _LANG_PROP_PUDDLE_LONG          "A puddle of murky water has gathered in a hollow of the ground, with muddy edges."
#define _LANG_PROP_PUDDLE_SMELL         "It smells of mud and standing water."

#define _LANG_PROP_PUDDLE_STEP_VERBS    ({ "step", "splash" })
#define _LANG_PROP_PUDDLE_STEP_ME       "You put a foot in the puddle and the cold water soaks your boot."
#define _LANG_PROP_PUDDLE_STEP_OTHERS   "$mcname$ puts a foot in the puddle and splashes mud around."

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
#define _LANG_PROP_ALTAR_PRAY_OTHERS    "$mcname$ bows $mposs$ head before the altar."

// ************************************************************
//  bowl
// ************************************************************

#define _LANG_PROP_BOWL_ID              "bowl"
#define _LANG_PROP_BOWL_ID_ALIAS_1      "basin"
#define _LANG_PROP_BOWL_NOUN            "bowl"
#define _LANG_PROP_BOWL_NOUN_PLURAL     "bowls"
#define _LANG_PROP_BOWL_SHORT           "a $material_phrase$ bowl"
#define _LANG_PROP_BOWL_LONG            "A small open $material_phrase$ bowl rests here."

// ************************************************************
//  fountain
// ************************************************************
#define _LANG_PROP_FOUNTAIN_ID          "fountain"
#define _LANG_PROP_FOUNTAIN_ID_ALIAS_1  "water fountain"

#define _LANG_PROP_FOUNTAIN_NOUN        "fountain"
#define _LANG_PROP_FOUNTAIN_NOUN_PLURAL "fountains"

#define _LANG_PROP_FOUNTAIN_SHORT       "a $material_phrase$ fountain"
#define _LANG_PROP_FOUNTAIN_LONG        "A $material_phrase$ fountain. Clear water bubbles gently in its basin."

#define _LANG_PROP_FOUNTAIN_DRINK_VERBS ({ "drink" })

#define _LANG_PROP_FOUNTAIN_DRINK_ME    "You drink some fresh water from the fountain."
#define _LANG_PROP_FOUNTAIN_DRINK_OTHERS "$mcname$ drinks some fresh water from the fountain."
#define _LANG_PROP_FOUNTAIN_SMELL       "The water smells faintly of moss and cold stone."

// ************************************************************
//  fireplace
// ************************************************************
#define _LANG_PROP_FIREPLACE_ID         "fireplace"
#define _LANG_PROP_FIREPLACE_ID_ALIAS_1 "hearth"

#define _LANG_PROP_FIREPLACE_NOUN       "fireplace"
#define _LANG_PROP_FIREPLACE_NOUN_PLURAL "fireplaces"

#define _LANG_PROP_FIREPLACE_SHORT      "a small fireplace"
#define _LANG_PROP_FIREPLACE_LONG       "A small fireplace built of $material_name$."

#define _LANG_PROP_FIREPLACE_SUFFIX_LIT " (lit)"

#define _LANG_PROP_FIREPLACE_LONG_LIT   " Flames crackle inside, casting warm light across the room."
#define _LANG_PROP_FIREPLACE_LONG_UNLIT " The ashes are cold; nobody has lit it in a while."
#define _LANG_PROP_FIREPLACE_LONG_UNLIT_PLURAL " The ashes are cold; nobody has lit them in a while."

#define _LANG_PROP_FIREPLACE_LIGHT_VERBS ({ "light", "ignite" })
// "put out" intentionally omitted — DGD's add_action registers
// single-word verbs only ("put" alone would collide with the global
// `put` cmd). Re-add as a properly composed verb when we handle
// multi-word verb dispatch.
#define _LANG_PROP_FIREPLACE_EXT_VERBS  ({ "extinguish", "douse" })

#define _LANG_PROP_FIREPLACE_LIGHT_ME       "You light the fireplace. Flames flicker to life."
#define _LANG_PROP_FIREPLACE_LIGHT_OTHERS   "$mcname$ lights the fireplace."
#define _LANG_PROP_FIREPLACE_EXT_ME         "You extinguish the fire. The hearth goes dark."
#define _LANG_PROP_FIREPLACE_EXT_OTHERS     "$mcname$ extinguishes the fire."
#define _LANG_PROP_FIREPLACE_ALREADY_LIT    "The fireplace is already lit."
#define _LANG_PROP_FIREPLACE_NOT_LIT        "The fireplace is not lit."

// ************************************************************
//  bar (tavern counter)
// ************************************************************
#define _LANG_PROP_BAR_ID               "bar"
#define _LANG_PROP_BAR_ID_ALIAS_1       "counter"

#define _LANG_PROP_BAR_NOUN             "bar"
#define _LANG_PROP_BAR_NOUN_PLURAL      "bars"

#define _LANG_PROP_BAR_SHORT            "a long $material_phrase$ bar"
#define _LANG_PROP_BAR_LONG             "A long $material_phrase$ bar runs along the wall, its surface scarred by years of mugs and elbows. It is not very clean."

#define _LANG_PROP_BAR_LEAN_VERBS       ({ "lean" })

#define _LANG_PROP_BAR_LEAN_ME          "You lean against the bar."
#define _LANG_PROP_BAR_LEAN_OTHERS      "$mcname$ leans against the bar."
#define _LANG_PROP_BAR_SMELL            "The bar reeks of stale ale and old wood."

// ------------------------------------------------------------
//  bunk
// ------------------------------------------------------------
#define _LANG_PROP_BUNK_ID              "bunk"
#define _LANG_PROP_BUNK_ID_ALIAS_1      "cot"
#define _LANG_PROP_BUNK_NOUN            "bunk"
#define _LANG_PROP_BUNK_NOUN_PLURAL     "bunks"
#define _LANG_PROP_BUNK_SHORT           "a $material_phrase$ bunk"
#define _LANG_PROP_BUNK_LONG            "A narrow $material_phrase$ bunk, its blanket folded at the foot."

#define _LANG_PROP_BUNK_SUFFIX_OCCUPIED " (%s is lying on it)"
#define _LANG_PROP_BUNK_LONG_OCCUPIED   " %s is resting on it."

#define _LANG_PROP_BUNK_LIE_VERBS       ({ "lie", "liedown" })
#define _LANG_PROP_BUNK_RISE_VERBS      ({ "rise", "getup" })

#define _LANG_PROP_BUNK_OCCUPIED_BY     "Somebody is already lying there."
#define _LANG_PROP_BUNK_ALREADY_LYING   "You are already lying on that bunk."
#define _LANG_PROP_BUNK_NOT_LYING       "You are not lying on the bunk."
#define _LANG_PROP_BUNK_LIE_ME          "You lie down on the bunk."
#define _LANG_PROP_BUNK_LIE_OTHERS      "$mcname$ lies down on the bunk."
#define _LANG_PROP_BUNK_RISE_ME         "You get up from the bunk."
#define _LANG_PROP_BUNK_RISE_OTHERS     "$mcname$ gets up from the bunk."

// ------------------------------------------------------------
//  bed
// ------------------------------------------------------------
#define _LANG_PROP_BED_ID               "bed"
#define _LANG_PROP_BED_NOUN             "bed"
#define _LANG_PROP_BED_NOUN_PLURAL      "beds"
#define _LANG_PROP_BED_SHORT            "a $material_phrase$ bed"
#define _LANG_PROP_BED_LONG             "A wide, neatly made $material_phrase$ bed."

#define _LANG_PROP_BED_SUFFIX_OCCUPIED  " (%s is lying on it)"
#define _LANG_PROP_BED_LONG_OCCUPIED    " %s is resting on it."

#define _LANG_PROP_BED_LIE_VERBS        ({ "lie", "liedown" })
#define _LANG_PROP_BED_RISE_VERBS       ({ "rise", "getup" })

#define _LANG_PROP_BED_OCCUPIED_BY      "Somebody is already lying there."
#define _LANG_PROP_BED_ALREADY_LYING    "You are already lying on that bed."
#define _LANG_PROP_BED_NOT_LYING        "You are not lying on the bed."
#define _LANG_PROP_BED_LIE_ME           "You lie down on the bed."
#define _LANG_PROP_BED_LIE_OTHERS       "$mcname$ lies down on the bed."
#define _LANG_PROP_BED_RISE_ME          "You get up from the bed."
#define _LANG_PROP_BED_RISE_OTHERS      "$mcname$ gets up from the bed."

// ------------------------------------------------------------
//  tapestry
// ------------------------------------------------------------
#define _LANG_PROP_TAPESTRY_ID          "tapestry"
#define _LANG_PROP_TAPESTRY_NOUN        "tapestry"
#define _LANG_PROP_TAPESTRY_NOUN_PLURAL "tapestries"
#define _LANG_PROP_TAPESTRY_SHORT       "a tapestry"
#define _LANG_PROP_TAPESTRY_LONG        "A tapestry hangs on the wall, woven in coloured threads that have faded a little."

#define _LANG_PROP_PAINTING_ID          "painting"
#define _LANG_PROP_PAINTING_ID_ALIAS_1  "canvas"
#define _LANG_PROP_PAINTING_ID_ALIAS_2  "plaque"
#define _LANG_PROP_PAINTING_NOUN        "painting"
#define _LANG_PROP_PAINTING_NOUN_PLURAL "paintings"
#define _LANG_PROP_PAINTING_SHORT       "a painting"
#define _LANG_PROP_PAINTING_LONG        "A painting hangs on the wall: a canvas in oils in a wooden frame, with a small engraved plaque at the foot."
#define _LANG_PROP_PAINTING_READ_VERBS  ({ "read" })
#define _LANG_PROP_PAINTING_READ_ME     "You step up to the painting and read the plaque engraved at the foot of the frame."
#define _LANG_PROP_PAINTING_READ_OTHERS "$mcname$ steps up to the painting and reads the plaque on its frame."

// ------------------------------------------------------------
//  rack
// ------------------------------------------------------------
#define _LANG_PROP_RACK_ID              "rack"
#define _LANG_PROP_RACK_ID_ALIAS_1      "pegs"
#define _LANG_PROP_RACK_NOUN            "rack"
#define _LANG_PROP_RACK_NOUN_PLURAL     "racks"
#define _LANG_PROP_RACK_SHORT           "a rack of pegs"
#define _LANG_PROP_RACK_LONG            "A row of $material_phrase$ pegs fixed to the wall, hung with cloaks and belts."

#define _LANG_PROP_RACK_SEARCH_VERBS    ({ "search", "rummage" })
#define _LANG_PROP_RACK_SEARCH_ME       "You rummage through the cloaks: nothing but old wool and worn straps."
#define _LANG_PROP_RACK_SEARCH_OTHERS   "$mcname$ rummages through the hanging cloaks."
#define _LANG_PROP_RACK_SMELL           "The cloaks smell of sweat, leather and hearth smoke."

// ------------------------------------------------------------
//  arms rack
// ------------------------------------------------------------
#define _LANG_PROP_ARMS_ID              "armsrack"
#define _LANG_PROP_ARMS_ID_ALIAS_1      "frame"
#define _LANG_PROP_ARMS_NOUN            "arms rack"
#define _LANG_PROP_ARMS_NOUN_PLURAL     "arms racks"
#define _LANG_PROP_ARMS_SHORT           "an arms rack of spears"
#define _LANG_PROP_ARMS_LONG            "A $material_phrase$ frame propped by the door, half a dozen spears socketed by the shaft with their heads up. A chain runs through every shaft and padlocks to the frame: none of these is leaving."

#define _LANG_PROP_ARMS_COUNT_VERBS     ({ "count" })
#define _LANG_PROP_ARMS_COUNT_ME        "You count six spears, every head clean and oiled, every shaft chained to the frame."
#define _LANG_PROP_ARMS_COUNT_OTHERS    "$mcname$ counts the spears in the rack."

// cobwebs
#define _LANG_PROP_COBWEB_ID            "cobweb"
#define _LANG_PROP_COBWEB_ID_ALIAS_1    "web"
#define _LANG_PROP_COBWEB_NOUN          "cobweb"
#define _LANG_PROP_COBWEB_NOUN_PLURAL   "cobwebs"
#define _LANG_PROP_COBWEB_SHORT         "thick cobwebs"
#define _LANG_PROP_COBWEB_LONG          "Cobwebs sturdier than they have any right to be hang from branch to branch, woven so close they barely let the light through."

#define _LANG_PROP_COBWEB_TOUCH_VERBS   ({ "touch", "feel" })
#define _LANG_PROP_COBWEB_TOUCH_ME      "You brush a strand with your fingers. It draws tight, hums, and somewhere something answers by moving."
#define _LANG_PROP_COBWEB_TOUCH_OTHERS  "$mcname$ brushes a strand of the cobwebs and the whole tangle shivers."

// anvil
#define _LANG_PROP_ANVIL_ID             "anvil"
#define _LANG_PROP_ANVIL_ID_ALIAS_1     "bickern"
#define _LANG_PROP_ANVIL_NOUN           "anvil"
#define _LANG_PROP_ANVIL_NOUN_PLURAL    "anvils"
#define _LANG_PROP_ANVIL_SHORT          "an anvil"
#define _LANG_PROP_ANVIL_LONG           "A heavy, black anvil $material_phrase$ on an oak stump. Its face is polished by years of blows and its horn worn from bending iron."

#define _LANG_PROP_ANVIL_STRIKE_VERBS   ({ "strike", "knock", "tap" })
#define _LANG_PROP_ANVIL_STRIKE_ME      "You strike the anvil. It rings deep and long, like a muffled bell."
#define _LANG_PROP_ANVIL_STRIKE_OTHERS  "$mcname$ strikes the anvil, and it rings like a muffled bell."

// bellows
#define _LANG_PROP_BELLOWS_ID           "bellows"
#define _LANG_PROP_BELLOWS_ID_ALIAS_1   "bellow"
#define _LANG_PROP_BELLOWS_NOUN         "bellows"
#define _LANG_PROP_BELLOWS_NOUN_PLURAL  "bellows"
#define _LANG_PROP_BELLOWS_SHORT        "a pair of bellows"
#define _LANG_PROP_BELLOWS_LONG         "A large pair of bellows $material_phrase$ with wooden boards, hanging by the forge and joined to it by an iron nozzle. A long lever works it."

#define _LANG_PROP_BELLOWS_PUMP_VERBS   ({ "pump", "work", "blow" })
#define _LANG_PROP_BELLOWS_PUMP_ME      "You pull the lever down a couple of times. The bellows wheeze and the air whistles through the nozzle into the forge."
#define _LANG_PROP_BELLOWS_PUMP_OTHERS  "$mcname$ works the bellows, which wheeze hard into the forge."

// forge
#define _LANG_PROP_FORGE_ID             "forge"
#define _LANG_PROP_FORGE_ID_ALIAS_1     "hearth"
#define _LANG_PROP_FORGE_NOUN           "forge"
#define _LANG_PROP_FORGE_NOUN_PLURAL    "forges"
#define _LANG_PROP_FORGE_SHORT          "a forge"
#define _LANG_PROP_FORGE_LONG           "A wide, low forge $material_phrase$, its open hearth under a hood blackened by smoke."
#define _LANG_PROP_FORGE_SUFFIX_LIT     " (lit)"
#define _LANG_PROP_FORGE_LONG_LIT       " A lively fire burns in it, and the heat reaches the door."
#define _LANG_PROP_FORGE_LONG_UNLIT     " It is out and its embers are cold; nobody is working the anvil today."
#define _LANG_PROP_FORGE_LONG_UNLIT_PLURAL " They are out and their embers are cold."

#define _LANG_PROP_FORGE_WARM_VERBS     ({ "warm" })
#define _LANG_PROP_FORGE_WARM_ME        "You step up to the forge and hold your hands to the fire. The heat goes right to your bones."
#define _LANG_PROP_FORGE_WARM_OTHERS    "$mcname$ steps up to the forge to warm their hands."
#define _LANG_PROP_FORGE_NOT_LIT        "The forge is out; it gives no heat at all."

// tools
#define _LANG_PROP_TOOLS_ID             "tools"
#define _LANG_PROP_TOOLS_ID_ALIAS_1     "tool"
#define _LANG_PROP_TOOLS_NOUN           "set of tools"
#define _LANG_PROP_TOOLS_NOUN_PLURAL    "sets of tools"
#define _LANG_PROP_TOOLS_SHORT          "a set of tools"
#define _LANG_PROP_TOOLS_LONG           "Tongs of several sizes, hammers, punches and files hang from pegs on the wall, all blackened by use and each in its place."

#define _LANG_PROP_TOOLS_HANDLE_VERBS   ({ "handle", "touch" })
#define _LANG_PROP_TOOLS_HANDLE_ME      "You take down a pair of tongs and weigh them; they are heavier than they look. You hang them back on their peg."
#define _LANG_PROP_TOOLS_HANDLE_OTHERS  "$mcname$ takes down a pair of tongs, weighs them and hangs them back."

// shelf
#define _LANG_PROP_SHELF_ID             "shelf"
#define _LANG_PROP_SHELF_ID_ALIAS_1     "shelves"
#define _LANG_PROP_SHELF_NOUN           "shelf"
#define _LANG_PROP_SHELF_NOUN_PLURAL    "shelves"
#define _LANG_PROP_SHELF_SHORT          "a shelf"
#define _LANG_PROP_SHELF_LONG           "A shelf $material_phrase$ by the door, with what is for sale set out in view on its boards."

#define _LANG_PROP_SHELF_BROWSE_VERBS   ({ "browse" })
#define _LANG_PROP_SHELF_BROWSE_ME      "You glance over the boards. Whatever is for sale, you ask whoever is serving for it."
#define _LANG_PROP_SHELF_BROWSE_OTHERS  "$mcname$ glances over the boards of the shelf."

// well
#define _LANG_PROP_WELL_ID              "well"
#define _LANG_PROP_WELL_ID_ALIAS_1      "curb"
#define _LANG_PROP_WELL_NOUN            "well"
#define _LANG_PROP_WELL_NOUN_PLURAL     "wells"
#define _LANG_PROP_WELL_SHORT           "a well"
#define _LANG_PROP_WELL_LONG            "A well with a curb $material_phrase$, worn by the rubbing of ropes. A wooden bucket hangs from a pulley over its mouth."

#define _LANG_PROP_WELL_LEAN_VERBS      ({ "lean" })
#define _LANG_PROP_WELL_LEAN_ME         "You lean over the curb. Far below the water glints, and your voice comes back from it a little deeper."
#define _LANG_PROP_WELL_LEAN_OTHERS     "$mcname$ leans over the well."

#define _LANG_PROP_WELL_DRINK_VERBS     ({ "drink" })
#define _LANG_PROP_WELL_DRINK_ME        "You haul up the bucket and take a drink. The water is cold and tastes of stone."
#define _LANG_PROP_WELL_DRINK_OTHERS    "$mcname$ hauls up the bucket from the well and takes a drink."

// trough
#define _LANG_PROP_TROUGH_ID            "trough"
#define _LANG_PROP_TROUGH_ID_ALIAS_1    "water trough"
#define _LANG_PROP_TROUGH_NOUN          "trough"
#define _LANG_PROP_TROUGH_NOUN_PLURAL   "troughs"
#define _LANG_PROP_TROUGH_SHORT         "a trough"
#define _LANG_PROP_TROUGH_LONG          "A long, low trough $material_phrase$, full of water to the brim. Its bottom is green with slime and its edge worn by the animals."

#define _LANG_PROP_TROUGH_DRINK_VERBS   ({ "drink" })
#define _LANG_PROP_TROUGH_DRINK_ME      "You cup your hands and drink from the trough. The water is lukewarm and tastes of grass."
#define _LANG_PROP_TROUGH_DRINK_OTHERS  "$mcname$ leans over the trough and drinks the animals' water."

// manger
#define _LANG_PROP_MANGER_ID            "manger"
#define _LANG_PROP_MANGER_ID_ALIAS_1    "feeder"
#define _LANG_PROP_MANGER_NOUN          "manger"
#define _LANG_PROP_MANGER_NOUN_PLURAL   "mangers"
#define _LANG_PROP_MANGER_SHORT         "a manger"
#define _LANG_PROP_MANGER_LONG          "A long, low manger $material_phrase$, with leftover grain and straw at the bottom and the wood worn smooth with use."

#define _LANG_PROP_MANGER_SMELL         "You smell the manger. It smells of grain, of damp straw and, above all, of animal."

// Words a player puts in front of the thing's name that are not part of it:
// "lie ON THE bunk". Peeled off the front of the argument before looking up
// which prop is meant.
#define _LANG_PROPS_FILLER_WORDS ({ "in", "on", "onto", "into", "at", "to", \
                                    "the", "a", "an", "before", "front", \
                                    "of", "toward", "towards" })

// Question asked when a prop verb arrives with no object ("pray"). A verb
// with no entry uses the generic one, with the verb as the player typed it.
#define _LANG_PROPS_VERB_PROMPT_DEFAULT "%s what?"
#define _LANG_PROPS_VERB_PROMPTS ([ \
  "pray": "Pray before what?" ])

#endif // _LANG_PROPS_H_EN
