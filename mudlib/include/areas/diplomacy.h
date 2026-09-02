
#define DIPLOMACY_HANDLER "/lib/handlers/diplomacy"
// The handler owns its graph as mutable, persisted state (built by hand via
// admin commands, never from a static table); this is where it is saved.
// One graph per game, beside its other state: the countries at war in one world
// have nothing to do with another's.
#define DIPLOMACY_SAVE_DIR  "/save/games/"
#define DIPLOMACY_SAVE_FILE "/diplomacy"

// The two relationship kinds a citizenship graph tracks. They are also the keys
// of the handler's `links` mapping, so these string values are persisted -- do
// not change them without migrating existing saves.
// Bound on how deep a citizenship parent chain may be walked, so a graph
// edited into a cycle cannot spin forever.
#define DIPLOMACY_MAX_DEPTH 16

// The deities a citizenship accepts. A temple consecrated to anything else
// stands closed for as long as this citizenship holds the ground it is on.
// Unset means it accepts whatever its parent accepts; a citizenship with no
// ancestor that declares any accepts everything.
#define DIPLOMACY_DEITIES "deities"

#define DIPLOMACY_RELATION_ALLY  "ally"
#define DIPLOMACY_RELATION_ENEMY "enemy"

// Log file name
#define LOG_DIPLOMACY "diplomacy"
