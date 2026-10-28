// Traits: lasting conditions a living carries (a wound, a poison, a curse).
// See /lib/obj/traits/traits.md.

#ifndef LIVING_TRAITS_H
#define LIVING_TRAITS_H

// The base every trait file inherits.
#define TRAIT_BASE "/lib/obj/traits/trait.c"

// Where a trait id resolves to a file: the game's own directory first, then
// the shared one.
#define TRAIT_DIR "obj/traits/"
#define TRAIT_SHARED_DIR "/lib/obj/traits/"

// The reasons a trait ends (trait_removed).
#define TRAIT_END_EXPIRED  "expired"
#define TRAIT_END_REMOVED  "removed"
#define TRAIT_END_REPLACED "replaced"
#define TRAIT_END_DEATH    "death"
#define TRAIT_END_ERROR    "error"
#define TRAIT_END_CURED    "cured"

// Keys the living itself reads from a trait's data mapping.
#define TRAIT_DATA_EXPIRES "expires"   // absolute time(), optional
#define TRAIT_DATA_SOURCE  "source"    // object name of whoever caused it

// Keys of the effects mapping a trait returns from trait_effects.
#define TRAIT_EFFECT_STATS "stats"     // ([ "dex" : -1 ]), see TRAIT_STATS
#define TRAIT_EFFECT_FLAGS "flags"     // ({ "flag", ... }) state flags

// The stat names a trait may change, by position in <living/stats.h>.
#define TRAIT_STATS ([ "str" : STR_POS, "dex" : DEX_POS, "con" : CON_POS, \
                       "int" : INT_POS, "wis" : WIS_POS, "cha" : CHA_POS, \
                       "per" : PER_POS, "wil" : WIL_POS ])

#endif
