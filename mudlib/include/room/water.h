#ifndef ROOM_WATER_H
#define ROOM_WATER_H

// How a water place lets one swim against its current (any way but the
// current's own exit). See /lib/room/water.c.
#define WATER_UPSTREAM_FREE  "free"
#define WATER_UPSTREAM_HARD  "hard"
#define WATER_UPSTREAM_BLOCK "block"

// Seconds a living stays in moving water, not swimming, before the current
// carries it off: long enough to choose where to swim.
#define WATER_DRIFT_TIME 20

// The chance of a stroke starts here and adds the swimmer's ability.
#define WATER_BASE_CHANCE 50

// Added to a stroke along the current; taken per point of strength from one
// across it; and per point of strength plus one from one against it.
#define WATER_DOWNSTREAM_BONUS 20
#define WATER_ACROSS_MALUS     5
#define WATER_UPSTREAM_MALUS   10

#endif
