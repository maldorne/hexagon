#ifndef ROOM_WATER_H
#define ROOM_WATER_H

// How a water place lets one swim against its current (any way but the
// current's own exit). See /lib/room/water.c.
#define WATER_UPSTREAM_FREE  "free"
#define WATER_UPSTREAM_HARD  "hard"
#define WATER_UPSTREAM_BLOCK "block"

// Seconds a living stays in moving water, not swimming, before the current
// carries it off.
#define WATER_DRIFT_TIME 8

// Chance added to a swim along the current, and taken from every swim per
// point of strength against it.
#define WATER_DOWNSTREAM_BONUS 20
#define WATER_UPSTREAM_MALUS   10

#endif
