// Riding: mounts and vehicles. A steed is anything a living can be carried by;
// its spec is a mapping with the keys below, assembled by the riding handler
// (handler("riding", ob)) from the default row, the row of its kind in the
// steeds table and the steed's own overrides, in that order. Details:
// /lib/living/riding.md.

#ifndef _RIDING_H
#define _RIDING_H

// the row every kind falls back to, and every missing text is read from
#define RIDING_DEFAULT_KIND "mount"

// the first seat drives, the others only travel
#define RIDING_DRIVER_SEAT 0

// spec keys
#define RIDE_KIND         "kind"          // row of the steeds table
#define RIDE_SEATS        "seats"         // how many ride at once
#define RIDE_MEDIUM       "medium"        // RIDE_MEDIUM_* it can cross
#define RIDE_TERRAIN_DENY "terrain_deny"  // RIDE_TERRAIN_* it cannot enter
#define RIDE_TERRAIN_ONLY "terrain_only"  // exit types it is limited to, if any
#define RIDE_EXITS_DENY   "exits_deny"    // exit types it cannot use
#define RIDE_EXIT_SIZE    "exit_size"     // smallest exit size (ROOM_SIZE) it fits
#define RIDE_REQUIRES     "requires"      // what the driver needs, see below
#define RIDE_LEARN        "learn"         // skill the driver gets at 10% on a first ride

// RIDE_MEDIUM values
#define RIDE_MEDIUM_GROUND "ground"
#define RIDE_MEDIUM_WATER  "water"
#define RIDE_MEDIUM_AIR    "air"

// RIDE_TERRAIN_DENY values
#define RIDE_TERRAIN_INDOORS     "indoors"
#define RIDE_TERRAIN_UNDERGROUND "underground"

// RIDE_REQUIRES keys
#define RIDE_REQ_SKILL     "skill"      // ({ skill id, minimum value })
#define RIDE_REQ_BODY_SIZE "body_size"  // ({ smallest, biggest }) body size of the race

#endif
