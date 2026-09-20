
#define NPC_OB       "/lib/npc"

#define NO_SPAM      "no_spam"

// Who an attack effect of a monster is aimed at, the fourth value of the data
// given to add_attack_effect
#define ATTACK_EFFECT_TARGET_ONE    0   // one of the attackers, at random
#define ATTACK_EFFECT_TARGET_MANY   1   // every attacker, named one by one
#define ATTACK_EFFECT_TARGET_ALL    2   // everybody in the room
#define ATTACK_EFFECT_TARGET_CASTER 3   // the monster itself

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
