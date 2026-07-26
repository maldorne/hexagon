#ifndef ROOM_LOCATION_CLEANER_H
#define ROOM_LOCATION_CLEANER_H

#define LOCATION_CLEANER  "/lib/handlers/location-cleaner"

// Graph radius warmed around each player, in BFS steps. The map shows the
// player's cell plus depth 3, so depth 4 keeps one ring beyond what the
// player can currently see already resident.
#define CLEANER_RADIUS        4

// Objects loaded per worker tick. Loading a map object (with its area
// registration and future statistical population) is the expensive unit,
// so this stays small to keep each worker tick well under the tick budget.
#define CLEANER_CHUNK         6

// L1 coarse cache: a player's current cell stays "warm" this many seconds.
// A move into a still-warm cell skips the neighbourhood walk entirely.
#define CLEANER_REGION_TTL    30

// L2 fine cache: a freshly loaded file is not re-walked or re-loaded while
// its stamp is fresh. Kept above REGION_TTL so a warm cell's ring does not
// churn faster than the cell itself.
#define CLEANER_FILE_TTL      60

// Idle grace before an out-of-range object becomes eligible for eviction
// (consumed by the evict sweep). Hysteresis against load<->evict churn when
// a player steps out of a zone and straight back in.
#define CLEANER_GRACE         120

#endif
