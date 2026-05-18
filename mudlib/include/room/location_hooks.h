
// location_hooks.h
//
// Constants for the location component hook system. See
// muds/notes/hexagon/dev/location-hook-system.md for the full design.
//
// Components register interest in a hook via query_hooks() returning
// ([ "func_name": HOOK_PRIORITY_*, ... ]). Lower priority runs earlier.

#ifndef LOCATION_HOOKS_H
#define LOCATION_HOOKS_H

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

#endif // LOCATION_HOOKS_H
