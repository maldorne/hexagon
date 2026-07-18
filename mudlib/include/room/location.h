
#ifndef LOCATION_H
#define LOCATION_H

#define BASE_LOCATION_OBJ "/lib/location.c"
#define LOCATION_HANDLER "/lib/room/handlers/location_handler.c"

// location component types

#define LOCATION_COMPONENT_SHOP        "shop"
#define LOCATION_COMPONENT_PUB         "pub"
#define LOCATION_COMPONENT_OUTSIDE     "outside"
#define LOCATION_COMPONENT_MAZE        "maze"
#define LOCATION_COMPONENT_PROPS       "props"

// Cartography-contributing components. A location that carries any of
// these tells the sector storage what kind of terrain it occupies, so
// the sector can classify itself (query_sector_type) and the world map
// renderer can pick a glyph. See include/maps/sectors.h.
#define LOCATION_COMPONENT_CITY        "city"
#define LOCATION_COMPONENT_ROAD        "road"
#define LOCATION_COMPONENT_FOREST      "forest"
#define LOCATION_COMPONENT_COAST       "coast"
#define LOCATION_COMPONENT_UNDERGROUND "underground"

// ************************************************************
//  Location component hook system
//
//  Components register interest in a hook via query_hooks() returning
//  ([ "func_name": HOOK_PRIORITY_*, ... ]). Lower priority runs earlier.
//  Full design in the Nottario doc `dev/location-hook-system.md`.
// ************************************************************

// Text-augmenting hooks (long, short, extra_look). Output appears in
// this order in the final text — structure first, atmosphere on top,
// dynamic last. Matches how a person describes a place verbally.
#define HOOK_PRIORITY_STRUCTURE   100
#define HOOK_PRIORITY_ATMOSPHERE  200
#define HOOK_PRIORITY_DYNAMIC     300

// Movement pipeline buckets. Gate first (validate before mutating
// args), then transforms (alter the verb), then trace (observe the
// final verb).
#define HOOK_PRIORITY_GATE        100
#define HOOK_PRIORITY_TRANSFORM   200
#define HOOK_PRIORITY_TRACE       300

// Marker returned by a reduce hook to claim authority: the combinator
// discards what it had accumulated so far and uses only this value.
// Used as ({ HOOK_EXCLUSIVE, value }).
#define HOOK_EXCLUSIVE  "__hook_exclusive__"

#endif // LOCATION_H
