// Riding: mounts and vehicles. A mount is anything a living can be carried by;
// its spec is a mapping with the keys below, assembled by the riding handler
// (handler("riding", ob)) from the default row, the row of its kind in the
// mounts table and the mount's own overrides, in that order. Details:
// /lib/living/riding.md.

#ifndef _RIDING_H
#define _RIDING_H

// the row every kind falls back to, and every missing text is read from
#define RIDING_DEFAULT_KIND "mount"

// the first seat drives, the others only travel
#define RIDING_DRIVER_SEAT 0

// spec keys
#define RIDE_KIND         "kind"          // row of the mounts table
#define RIDE_SEATS        "seats"         // how many ride at once
#define RIDE_MEDIUM       "medium"        // RIDE_MEDIUM_* it can cross
#define RIDE_TERRAIN_DENY "terrain_deny"  // RIDE_TERRAIN_* it cannot enter
#define RIDE_TERRAIN_ONLY "terrain_only"  // exit types it is limited to, if any
#define RIDE_EXITS_DENY   "exits_deny"    // exit types it cannot use
#define RIDE_EXIT_SIZE    "exit_size"     // smallest exit size (ROOM_SIZE) it fits
#define RIDE_REQUIRES     "requires"      // what the driver needs, see below
#define RIDE_LEARN        "learn"         // skill the driver gets at 10% on a first ride
#define RIDE_SWAMP_WALKER "swamp_walker"  // 1 when it crosses a bog without sinking
#define RIDE_COMBAT       "combat"        // RIDE_COMBAT_* how it takes part in its rider's fights
#define RIDE_PROTECT      "protect"       // percent chance it takes the blows aimed at its rider
#define RIDE_WIMPY        "wimpy"         // percent of its hit points under which it flees with its rider
#define RIDE_FEARS        "fears"         // names, race ids or properties that make it flee with its rider

// RIDE_COMBAT values
#define RIDE_COMBAT_LINKED "linked"  // it fights whoever fights its rider, and the rider whoever fights it
#define RIDE_COMBAT_NONE   "none"    // it stays out of the fights

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

// the mounts a player owns: how many at most, and the keys that tie a mount to
// its owner and to its save file (both kept in its spec)
#define MAX_OWNED_MOUNTS 10
#define RIDE_OWNER       "owner"      // name of the player who owns it
#define RIDE_ID          "mount_id"   // name of its save file, and its key in the owner's list

// keys of an entry of a mount shop's stock: what the mount is made from
#define MOUNT_TEMPLATE "template"  // bestiary template it is made from, or
#define MOUNT_FILE     "file"      // the file it is cloned from

// the folder, inside the owner's save folder, holding one file per mount
#define MOUNTS_SAVE_DIR "mounts/"

#endif
