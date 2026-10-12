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
harbor, bank).

## Swum exits

The ways into, out of and across water are swum exits, declared with the
`swim` exit option: `add_exit(DIR_EAST, <water>, "standard", nil,
([ "swim": 1 ]))` on a coast, while every exit of a water room is swum by
itself. A swum exit is part of the map and of the coordinate graph, and the
map draws water as `[~]`, but it is not listed and typing its direction does
nothing; the path finder leaves it out unless the walker swims. Unlike a sealed
exit, nothing ever reveals it.

## Swimming

The `swim` skill (`nadar`) takes the swum exits, and whoever reaches water or a
coast learns it. On a coast, `nadar` alone goes into the water (`nadar <dir>`
when there is water more than one way), and a line under the description says
so. In water there are no exit lines: one sees where the shore is ("Puedes ver
la orilla hacia el oeste") and swims with `nadar <dir>`; `nadar` alone says
where the shore is again.

Going in from a coast always works. Every stroke inside water is rolled: a
base of 50 plus the swimmer's ability, plus 20 with the current, minus 5 per
point of strength across it, and minus 10 per point of strength plus one
against it (straight back where it comes from) when upstream is `hard`;
against a `block` current nobody swims at all. Deep water takes 10 more, and
so does whatever they carry beyond half of what they can, point by point. A
chance of 100 or more is not rolled.

Whoever arrives in water, swimming or carried by the current, is told in bold,
between blank lines, what applies right now: where the shore is ("Puedes ver
la orilla hacia el este.", when a swum exit leads to a coast), and in a
reachable bridge cell that it may climb.

Whoever fails swallows water, a bite of life that never takes the last of it,
and the current carries them out by `swept_to`; in still water they stay.
Whoever stays in moving water without swimming is carried off the same way
after twenty seconds.

## Creatures

An NPC template with `swims` (a fish) walks swum exits like ordinary ones and
finds paths through them, and is the only kind the area materializes in a
water location, whatever the rooms declared. Every other NPC stays out of the
water.

## Bridges

A bridge cell is one location at one coordinate that is a bridge and water at
once: the water rules above (current, strength, upstream, swept_to, depth,
drift) plus exits of type `bridge`, which are walked. Every other exit of the
cell is swum, as in water. The rules live in `/lib/room/bridge.c`, the room
base is `/lib/bridge.c` and a location gets a `bridge` component from
`room2loc` in place of the water one, with the same water settings.

Each living in the cell is on the bridge or in the water. It arrives on the
bridge unless it comes through a swum exit (swimming, carried by the current),
or is put in the water by a jump or a fall; a login, a teleport or a room load
leave it on the bridge.

- On the bridge there is no drift, the exit lines are the ordinary ones and
  only the bridge exits work; a swum direction or `nadar` answers that one is
  on the bridge and must jump.
- In the water the cell reads as water (shore line instead of exits, drift),
  `nadar <dir>` takes the swum exits, and a bridge exit answers that one
  cannot get up onto the bridge from the water.
- `saltar` (`saltar al río`, `saltar al agua`, `jump`) goes from the bridge
  into the water of the same cell and starts the drift timer.
- `reachable` (default 1; `set_bridge_reachable(0)` for a high bridge): when
  set, `trepar` (`climb`) from the water gets back onto the bridge, rolled as
  the climb skill (ability plus `BRIDGE_CLIMB_MODIFIER`, minus
  `WATER_ACROSS_MALUS` per point of the current's strength; a failure swallows
  water), and livings on the bridge and in the water can fight each other.
  When not set there is no climbing ("El puente está demasiado alto.") and
  combat does not start across the two positions, neither by the attack
  command nor by an aggressive NPC: `attack_ob` and `attack_by` ask the place
  `query_out_of_reach(attacker, victim)`, which answers why not or nil.
  Fights already under way are not stopped.

Other code asks `query_in_water(who)` of the place (true everywhere in a plain
water place, per position in a bridge cell, false on dry land), and forces
somebody into the water, a breaking plank, with
`drop_into_water(who, message, room_message)`: `message` is read by the one who
falls, `room_message` by the others (`$N` is the one who falls); both are
optional.

A room declares it like a water room, with its bridge exits of type `bridge`:

    inherit "/lib/bridge.c";

    set_water_current(DIR_SOUTH);
    set_water_strength(1);
    set_bridge_reachable(0);                  // optional, a high bridge
    add_exit(DIR_EAST, <bank>, "bridge");     // walked
    add_exit(DIR_NORTH, <water>, "standard"); // swum, as in any water place

and a location needs only the `bridge` component (`LOCATION_COMPONENT_BRIDGE`)
with the water settings and `bridge_unreachable`; `room2loc` attaches it from a
room built on `/lib/bridge.c`. The map draws the cell as `[=]` (yellow), and
the viewer's brackets turn blue while in the water.
