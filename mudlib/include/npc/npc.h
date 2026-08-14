
#define NPC_OB       "/lib/npc"

#define MONSTER_HAND "/lib/handlers/npcs.c"
#define NO_SPAM      "no_spam"

// #define NO_WANDER
#define MIN_PLAYER_LEVEL 1
#define MIN_MONSTER_LEVEL 10
// Idle wander cadence: a step every 300 + random(300) heart_beats -- a slow,
// occasional drift.
#define MOVE_DEFAULT ({ 300, 300 })
// Directed-travel cadence: a step every heart_beat (~one room per 2s), so a
// walking NPC is visible and interactable instead of teleporting. Per-NPC speed
// overrides this via set_travel_speed.
#define TRAVEL_DEFAULT ({ 1, 0 })
