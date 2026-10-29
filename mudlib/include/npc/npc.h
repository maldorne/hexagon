
#define NPC_OB       "/lib/npc"

#define NO_SPAM      "no_spam"

// Votes of the flee chain (see query_wants_to_flee in /lib/npc.c): a veto
// keeps the NPC fighting whatever the others say, a force sends it away.
#define FLEE_VETO     -1
#define FLEE_ABSTAIN   0
#define FLEE_FORCE     1

// Chance, in percent per heart beat, that a monster in a fight tries to use one
// of its combat effects (see combat_effects_heart_beat in /lib/monster/combat.c)
#define NPC_CAST_CHANCE 20

// #define NO_WANDER
// Grudges (see /lib/monster/memory.c): from which level a monster remembers the
// players who fought it, from which level a player is remembered, and how long
// a grudge lasts after the last blow
#define GRUDGE_MIN_MONSTER_LEVEL  6
#define GRUDGE_MIN_ATTACKER_LEVEL 1
#define GRUDGE_EXPIRY_SECONDS     300
// Idle wander cadence: a step every 300 + random(300) heart_beats -- a slow,
// occasional drift.
#define MOVE_DEFAULT ({ 300, 300 })
// Directed-travel cadence: a step every heart_beat (~one room per 2s), so a
// walking NPC is visible and interactable instead of teleporting. Per-NPC speed
// overrides this via set_travel_speed.
#define TRAVEL_DEFAULT ({ 1, 0 })
