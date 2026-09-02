#define FAMILY_HANDLER "/lib/handlers/families"
// The shared default. A game's own handler overrides query_save_file to keep
// its register beside the rest of that game's state: the houses of a fantasy
// world and those of a science-fiction one share nothing, not even a surname.
#define FAMILY_SAVE "/save/families"

// How many times the generator is asked again when the surname it minted is
// already spoken for, before giving up on that citizenship.
#define FAMILY_MINT_TRIES 20

// A member is named the same way whether it is an NPC or a player, so nothing
// about a family has to know which it is holding: the prefix says how to
// resolve the rest.
#define FAMILY_NPC     "npc:"
#define FAMILY_PLAYER  "player:"

// A family's record
#define FAMILY_CITIZENSHIP "citizenship"
#define FAMILY_MEMBERS     "members"
#define FAMILY_HISTORY     "history"
#define FAMILY_PROPERTIES  "properties"

// A member's row, and an entry in the history
#define FAMILY_SPOUSE  "spouse"
#define FAMILY_PARENTS "parents"
#define FAMILY_NAME    "name"
#define FAMILY_FATE    "fate"

// What became of somebody the house no longer holds. The history keeps them either
// way: an NPC's savefile is deleted when it dies, so without this the dead stop
// being nameable and a generation means nothing.
#define FAMILY_DIED    "died"
#define FAMILY_MARRIED "married"

// Which parent's family a marriage joins, and whose surname the children take.
// Declared by the citizenship, so one culture can differ from the next.
#define DESCENT_PATRILINEAL "patrilineal"
#define DESCENT_MATRILINEAL "matrilineal"
#define DESCENT_NONE        "none"
