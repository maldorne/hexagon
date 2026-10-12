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

// A living dragged by a current may be hurt (rocks, being pulled under): the
// chance is the base plus the cell's strength times the second, less a tenth
// of the swim ability (the divisor). The damage is one die per point of
// strength plus a percentage of the victim's maximum life per point.
#define WATER_HURT_BASE_CHANCE     10
#define WATER_HURT_STRENGTH_CHANCE 10
#define WATER_HURT_SKILL_DIVISOR   10
#define WATER_HURT_DIE             6
#define WATER_HURT_HP_PERCENT      3

// What the current does to each thing it drags (not a living) in a roll of
// 100: lost for good below the first number, then washed onto a coast the
// cell has a swum exit to for the next, and otherwise carried downstream.
// A unique object is never lost.
#define WATER_OBJECT_LOST_CHANCE  15
#define WATER_OBJECT_SHORE_CHANCE 25

#endif
