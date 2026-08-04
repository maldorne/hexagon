#ifndef NPC_PERSISTED_H
#define NPC_PERSISTED_H

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

#endif
