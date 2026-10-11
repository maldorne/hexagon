# Water and coasts

Some places are swum, not walked: a river, a lake, the sea, an underground
stream. Their edges, where one goes in from dry land, are coasts: a beach, a
cliff, a port, a river bank.

## Building them

The rules live in two shared files, `/lib/room/water.c` and
`/lib/room/coast.c`. A game on rooms builds on the room bases `/lib/water.c`
and `/lib/coast.c`; a game on locations gets the same behaviour from the
`water` and `coast` components, which `room2loc` attaches from those bases
with the room's settings. A water room or a coast below the surface calls
`set_underground_kind()` and counts as underground as well.

A water place declares:

    set_water_current(DIR_SOUTHEAST);        // the exit the water flows out by
    set_water_strength(2);                   // 0 still .. 3 rapids
    set_water_upstream(WATER_UPSTREAM_HARD); // free | hard | block
    set_water_swept_to(DIR_SOUTH);           // where a failed swimmer ends,
                                             // the current's exit by default
    set_water_depth(2);                      // 1 surface, 2 deep

A coast declares its kind: `set_coast_kind("bank")` (beach, cliff, port,
harbor, bank). Its exits into water are found by looking at where they lead.

## Swimming

The exits of a water place stay real, for the map and the paths, but nobody
walks them, and nobody walks from a coast into water: the move is refused and
one swims instead, with the `swim` skill (`nadar <dirección>`). Whoever reaches
water or a coast learns it. In water the exit line lists where one can swim
(`[nadar: ne,o,se]`); on a coast a line under the description says it, and
`nadar` alone says it anywhere.

Going in from a coast always works. Every stroke inside water is rolled: the
swimmer's ability, plus a bonus with the current, minus a malus across or
against it by its strength (more when upstream is `hard`; impossible when it
is `block`), minus deep water, minus whatever they carry beyond half of what
they can. A chance of 100 or more is not rolled.

Whoever fails swallows water, a bite of life that never takes the last of it,
and the current carries them out by `swept_to`; in still water they stay.
Whoever stays in moving water without swimming is carried off the same way
after a few seconds.

## Creatures

An NPC template with `swims` (a fish) walks water with ordinary exits, and is
the only kind the area materializes in a water location, whatever the rooms
declared. Every other NPC is refused at a coast like a player.
