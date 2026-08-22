
#define DIPLOMACY_HANDLER "/lib/handlers/diplomacy"
// The handler owns its graph as mutable, persisted state (built by hand via
// admin commands, never from a static table); this is where it is saved.
#define DIPLOMACY_SAVE "/save/diplomacy.o"

// The two relationship kinds a citizenship graph tracks. They are also the keys
// of the handler's `links` mapping, so these string values are persisted -- do
// not change them without migrating existing saves.
// Bound on how deep a citizenship parent chain may be walked, so a graph
// edited into a cycle cannot spin forever.
#define DIPLOMACY_MAX_DEPTH 16

#define DIPLOMACY_RELATION_ALLY  "ally"
#define DIPLOMACY_RELATION_ENEMY "enemy"

// Log file name
#define LOG_DIPLOMACY "diplomacy"
