#ifndef PERSISTED_H
#define PERSISTED_H

// Coarse NPC category ids (English), used by the area census and the spawner
// to classify persisted NPCs. Plain string ids so area config and diplomacy
// generation can key on them directly.
#define NPC_CATEGORY_AGGRESSIVE  "aggressive"
#define NPC_CATEGORY_PACIFIC     "pacific"
#define NPC_CATEGORY_ANIMAL      "animal"
#define NPC_CATEGORY_CITIZEN     "citizen"
#define NPC_CATEGORY_GUARD       "guard"

// Persisted-NPC savefiles: /save/games/<game>/npcs/<letter>/<uuid>/npc.o
#define NPC_SAVE_FILE  "npc.o"

// version-4 uuid generator (packages/uuid)
#define UUID_OB  "/packages/uuid/master"

// NPC data-template store: snapshots a hand-authored NPC .c into a data
// template once; a generic NPC is then spawned from the template so the .c
// is a one-time seed, never reloaded at spawn (mirrors room -> location).
#define BESTIARY_HANDLER  "/lib/handlers/bestiary"
#define GENERIC_NPC       "/lib/npc"
// A generic NPC with the guardian role: blocks enemy citizenships at a watched
// exit. Spawned instead of GENERIC_NPC for a citizenship's guards; still takes
// its appearance / race / level from a bestiary template like any NPC.
#define GUARD_NPC         "/lib/guard"

#endif
