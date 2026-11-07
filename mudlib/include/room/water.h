#ifndef ROOM_WATER_H
#define ROOM_WATER_H

// How a water place lets one swim against its current (any way but the
// current's own exit). See /lib/room/water.c.
#define WATER_UPSTREAM_FREE  "free"
#define WATER_UPSTREAM_HARD  "hard"
#define WATER_UPSTREAM_BLOCK "block"

// What body of water a place is part of. See /lib/room/water.c.
#define WATER_KIND_RIVER "river"
#define WATER_KIND_SEA   "sea"
#define WATER_KIND_LAKE  "lake"
#define WATER_KINDS ({ WATER_KIND_RIVER, WATER_KIND_SEA, WATER_KIND_LAKE })

// Seconds a living stays in moving water, not swimming, before the current
// carries it off (counted in heart_beats): long enough to choose where to
// swim.
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

// A thing that falls into moving water is carried on at once, cell by cell,
// until a coast takes it out; at most this many cells (a current that loops
// back on itself never ends).
#define WATER_CARRY_MAX_CELLS 20

// Static property of a thing being carried: the cells it has crossed in this
// carry (never saved).
#define WATER_CARRY_PROP "water_carry_cells"

// Static property of a living under the surface holding its breath:
// ({ heart_beats of breath left, time() of its last submerged heart_beat }).
// Not there, or too old, it is a full lungful. See /lib/room/water.c.
#define WATER_BREATH_PROP "water_breath"

// Below the surface (set_water_submerged): a living without a way to breathe
// holds its breath for this many heart_beats plus the second number for each
// point of constitution, then drowns: each heart_beat it loses a percentage of
// its maximum life (at least one point). See /lib/room/water.c.
#define WATER_BREATH_BASE        10
#define WATER_BREATH_PER_CON     2
#define WATER_DROWN_HP_PERCENT   5

#endif
