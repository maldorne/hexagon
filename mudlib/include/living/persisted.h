#ifndef PERSISTED_H
#define PERSISTED_H

// Persisted-NPC savefiles: /save/games/<game>/npcs/<letter>/<uuid>/npc.o
#define NPC_SAVE_FILE  "npc.o"

// version-4 uuid generator (packages/uuid)
#define UUID_OB  "/packages/uuid/master"

// NPC data-template store: snapshots a hand-authored NPC .c into a data
// template once; a generic NPC is then spawned from the template so the .c
// is a one-time seed, never reloaded at spawn (mirrors room -> location).
#define BESTIARY_HANDLER  "/lib/handlers/bestiary"
#define GENERIC_NPC       "/lib/npc"
#endif
