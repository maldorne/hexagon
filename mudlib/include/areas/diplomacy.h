
#define DIPLOMACY_HANDLER "/lib/handlers/diplomacy"
// The handler owns its graph as mutable, persisted state (built by hand via
// admin commands, never from a static table); this is where it is saved.
// The shared default. A game's own handler overrides query_save_file to keep
// its graph beside the rest of that game's state, because the countries at war
// in one world have nothing to do with another's.
#define DIPLOMACY_SAVE "/save/diplomacy"

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

// The people born under a citizenship: the races its citizens are drawn from
// (race paths, one entry per share) and the wordlist their names come from. A
// node that does not set them takes them from its parent chain; when nobody up
// the chain does, the citizenship file of the top one decides. The nationality
// is not touched: a village of another people is still a subject of its realm.
#define DIPLOMACY_RACES      "races"
#define DIPLOMACY_NAME_STYLE "names"

// The law of a citizenship: its own law code (offence -> points, 0 = legal
// there), whether its guards kill, and the location its prisoners are taken to.
// A key a citizenship does not carry is inherited from its parent chain; what
// nobody up the chain sets falls back to the defaults in <crimes.h>.
#define DIPLOMACY_LAW    "law"
#define DIPLOMACY_LETHAL "lethal"
#define DIPLOMACY_PRISON "prison"

#define DIPLOMACY_RELATION_ALLY  "ally"
#define DIPLOMACY_RELATION_ENEMY "enemy"

// Log file name
#define LOG_DIPLOMACY "diplomacy"
