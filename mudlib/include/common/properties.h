
#ifndef _COMMON_PROPERTIES_H
#define _COMMON_PROPERTIES_H

#define LOADING_PROP "loading"
#define NORAISE_PROP "noregen"

#define AWAY_PROP "away"

#define SEARCH_PROP "search"
#define BLIND_PROP "blind"
#define SENSE_LIFE_PROP "sense_life"

// whoever has it breathes under water (an item worn, a spell, a nature), so
// it never drowns. See /lib/room/water.md
#define WATER_BREATHING_PROP "water_breathing"

#define SWIMMING_PROP "swimming direction"
#define FOLLOWING_PROP "is following"
#define STOP_FOLLOWING_PROP "stop following now"

#define DRUNK_PROP "next drunk heart beat"
#define PASSED_OUT_PROP "passed out"
#define SLEEP_PROP "sleeping"
#define PARALYZE_PROP "paralyzed"
#define PACIFY_PROP "pacify_on"
#define NOSKILL_PROP "noskill"
// whoever has it cannot cast spells (formular), though may use skills
#define NOCAST_PROP "nocast"
// a place where no spell can be cast (the effect base refuses a spell cast
// there; skills still work)
#define NO_CAST_PROP "no_cast"
#define NOKILL_PROP "nokill"
// nothing can be stolen from this item, living or place (steal skill)
#define NOSTEAL_PROP "nosteal"
// a victim who caught or noticed a thief stays alert for a while
#define STEAL_ALERT_PROP "steal alert"
// static property of a living: percentage of its sneaking steps that cost no
// energy (sneak skill)
#define SILENT_STEPS_PROP "silent_steps"
// the place has no law: nothing done there is a crime (crimes handler)
#define LAWLESS_PROP "lawless"
// #define NOCAST_PROP "nocast"
// #define NOGUILD_PROP "noguild"
#define CURSED_PROP "cursed"
#define FEAR_PROP "fear"
#define FLIPPED_PROP "flipped"
#define STONED_PROP "stoned"

// do not allow creation of undead npcs in a room
#define NO_UNDEAD "no_undead"
// this npc is undead
#define UNDEAD "undead"

// things dry off faster
// (see /global/pweath.c)
#define WARMTH_PROP "warmth"

// related with inform events
#define NO_INFORM "no_inform"

#define PC_GAVE_NPC_PROPERTY "pc_gave_npc"

// initial character adjust
#define ORIGINAL_ADJUST_PROP "original adjust"

#endif
