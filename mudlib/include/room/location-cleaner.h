#ifndef ROOM_LOCATION_CLEANER_H
#define ROOM_LOCATION_CLEANER_H

#include <cartography.h>

#define LOCATION_CLEANER  "/lib/handlers/location-cleaner"

// Radius warmed around each player, in coordinate steps, and one step wider
// than the map reaches so what it draws is always already resident. Derived
// from the map's own reach: raise the viewport and this follows.
//
// Measured in coordinates rather than in exit steps on purpose. The map is
// bounded by its viewport, so a winding street can put a location six exits
// away inside the picture while its coordinates sit two steps from the player;
// a walk bounded by exit steps would leave exactly those locations cold.
#define CLEANER_RADIUS        (CART_MAP_REACH + 1)

// Exit steps the walk may take before giving up. The coordinate window is the
// real bound; this only keeps a pathological layout (a spiral inside the
// window) from walking forever.
#define CLEANER_MAX_STEPS     ((2 * CLEANER_RADIUS + 1) * (2 * CLEANER_RADIUS + 1))

// Objects loaded per worker tick. Loading a map object (with its area
// registration and future statistical population) is the expensive unit,
// so this stays small to keep each worker tick well under the tick budget.
#define CLEANER_CHUNK         6

// L1 coarse cache: a player's current location stays "warm" this many seconds.
// A move into a still-warm location skips the neighbourhood walk entirely.
#define CLEANER_REGION_TTL    30

// L2 fine cache: a freshly loaded file is not re-walked or re-loaded while
// its stamp is fresh. Kept above REGION_TTL so the ring around a warm location
// does not churn faster than the location itself.
#define CLEANER_FILE_TTL      60

// Idle grace before an out-of-range object becomes eligible for eviction
// (consumed by the evict sweep). Hysteresis against load<->evict churn when
// a player steps out of a zone and straight back in.
#define CLEANER_GRACE         120

// Seconds between evict sweeps. Kept well below GRACE so an object that
// falls out of range is checked several times before it can age out.
#define CLEANER_SWEEP_INTERVAL  30

#endif
