#ifndef ROOM_BRIDGE_H
#define ROOM_BRIDGE_H

// The exit type walked on a bridge; every other exit of a bridge cell is
// swum. See /lib/room/bridge.c.
#define BRIDGE_EXIT_TYPE "bridge"

// Added to the climber's ability to get back onto the bridge from the water;
// the current takes WATER_ACROSS_MALUS per point of strength from it.
#define BRIDGE_CLIMB_MODIFIER 50

#endif
