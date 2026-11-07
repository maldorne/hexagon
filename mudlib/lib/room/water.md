# Water and coasts

Some places are swum, not walked: a river, a lake, the sea, an underground
stream. Their edges, where one goes in from dry land, are coasts: a beach, a
cliff, a port, a river bank.

## Building them

The rules live in two shared files, `/lib/room/water.c` and
`/lib/room/coast.c`. A game on rooms builds on the room bases `/lib/water.c`
and `/lib/coast.c`; a game on locations gets the same behaviour from the
`water` and `coast` components, which `room2loc` attaches from those bases
with the room's settings. The room bases (and `/lib/bridge.c`, below) are open
to the sky: they build on `/lib/outside.c`. Water or a shore below the surface is a location with the
`underground` component as well: its room answers `query_outside()` with 0 and
`query_underground()` with 1, so `room2loc` attaches `underground` instead of
`outside`.

A water place declares:

    set_water_kind(WATER_KIND_RIVER);        // river | sea | lake
    set_water_current(DIR_SOUTHEAST);        // the exit the water flows out by
    set_water_strength(2);                   // 0 still .. 3 rapids
    set_water_upstream(WATER_UPSTREAM_HARD); // free | hard | block
    set_water_swept_to(DIR_SOUTH);           // where a failed swimmer ends,
                                             // the current's exit by default
    set_water_depth(2);                      // 1 surface, 2 deep
    set_water_submerged(1);                  // 0 surface, 1.. below it

The kind names the body of water the place is part of (`WATER_KINDS` in
`<room/water.h>`): a stretch of a river (an underground stream too), the open
sea, or a lake. It travels with the other settings into the water component.

A coast may declare its kind: `set_coast_kind("bank")` (beach, cliff, port,
harbor, bank, marsh). Without one it is a plain coast; only a beach (sand
colour), a marsh (a swamp meeting the sea, swamp colour) and a port or harbor (a dock: its sector draws an arm out to sea on the world map)
change how the map draws its sector.

## Swum exits

The ways into, out of and across water are swum exits, declared with the
`swim` exit option: `add_exit(DIR_EAST, <water>, "standard", nil,
([ "swim": 1 ]))` on a coast, while every exit of a water room is swum by
itself. A swum exit is part of the map and of the coordinate graph, and the
map draws water as `[~]`, but it is not listed and typing its direction does
nothing; the path finder leaves it out unless the walker swims. Unlike a sealed
exit, nothing ever reveals it.

## Under the surface

A cell with `set_water_submerged(n)` (n = how far below the surface, 0 at the
surface; `query_water_submerged()`, travelling with the other settings into the
water component and answered by a location too) is underwater. It is built like
any water place (rooms on `/lib/water.c`, locations with the `water` component),
and the ways up and down are ordinary swum exits (`DIR_UP`, `DIR_DOWN`; `nadar
arriba`). `set_water_depth` stays the difficulty of the stroke and is separate:
a lake bottom is `set_water_depth(2)` and `set_water_submerged(2)`. The look line
reads "Estás bajo el agua". A place that wants the cell not to be open to the
sky adds the `underground` component as before.

Drowning is not a trait: the water code does it on the heart_beat of the livings
in the cell (`event_heart_beat`, see `lib/core/basic/events.md`), with no
timer on the place. Each beat in a submerged cell, `breathe_step` in
`/lib/room/water.c` works on whoever is in the water there (on a bridge above
it, nobody) and cannot breathe (`breathes_under_water(who)`: `query_swims()`,
`WATER_BREATHING_PROP` on the living or on something it wears or holds). The
breath is kept on the living in the static property `WATER_BREATH_PROP`,
`({ beats left, time() of its last submerged beat })`. Without the property, or
when the last beat is more than three `HEART_BEAT_TIME` old (it surfaced, left,
or its heart_beat was off), the breath starts full: `WATER_BREATH_BASE` plus
`WATER_BREATH_PER_CON` per point of constitution heart_beats (`<room/water.h>`).
Each beat takes one, with a warning at half and near the end; with none left
the living loses `WATER_DROWN_HP_PERCENT` of its maximum life per beat (and can
die). Whoever breathes there has the property removed, a full lungful. A monster
whose heart_beat is off (nobody around) neither drowns nor is dragged.

## Breathing

One property, `WATER_BREATHING_PROP` (`"water_breathing"`, `<common/properties.h>`),
read with `query_property`: while the living has it, the breath is full and
nothing hurts. Its sources:

- an item worn or held: give the item `add_property(WATER_BREATHING_PROP, 1)`;
  `set_in_use` of `lib/item.c` sets it as a static property of whoever uses the
  item and takes it away when the item stops being used (two such items share
  the one property, so taking one off drops it);
- a spell or any effect: `add_timed_property(WATER_BREATHING_PROP, 1, ticks)` and
  `remove_timed_property` when its lasting effect ends. A spell that is cast
  underwater needs `allow_on_water = 1`, as the base refuses casting in the water;
- a nature: an NPC with `swims`, or any NPC given the property in `setup`.

## Swimming

The `swim` skill (`nadar`) takes the swum exits, and whoever reaches water or a
coast learns it. On a coast, `nadar` alone goes into the water (`nadar <dir>`
when there is water more than one way), and a line under the description says
so. In water there are no exit lines: one sees where the shore is ("Puedes ver
la orilla hacia el oeste", or that it is nowhere in sight) and every way one can
swim ("Puedes nadar hacia el norte y el oeste"), and swims with `nadar <dir>`;
`nadar` alone says both again.

Going in from a coast always works. Every stroke inside water is rolled: a
base of 50 plus the swimmer's ability, plus 20 with the current, minus 5 per
point of strength across it, and minus 10 per point of strength plus one
against it (straight back where it comes from) when upstream is `hard`;
against a `block` current nobody swims at all. Deep water takes 10 more, and
so does whatever they carry beyond half of what they can, point by point. A
chance of 100 or more is not rolled.

Whoever arrives in water, swimming or carried by the current, is told in bold,
between blank lines, what applies right now: where the shore is ("Puedes ver
la orilla hacia el este.", when a swum exit leads to a coast), where one can
swim, and in a reachable bridge cell that it may climb.

Whoever fails swallows water, a bite of life that never takes the last of it,
and the current carries them out by `swept_to`; in still water they stay.
Whoever stays in moving water without swimming is carried off the same way
after twenty seconds.

## The current drags

Whoever is dragged (the drift step that carries off a living who is not
swimming) may be hurt: rocks, being pulled under. The chance is
`WATER_HURT_BASE_CHANCE` (10) plus `WATER_HURT_STRENGTH_CHANCE` (10) per point
of strength, less a tenth of the swim ability. The damage is one d6
(`WATER_HURT_DIE`) per point of strength plus 3% (`WATER_HURT_HP_PERCENT`) of
the victim's maximum life per point. The victim and the room are told, and it
can kill: the death happens in the water. Ghosts take no damage. The wait is
counted in beats: each `event_heart_beat` of a living in moving water that does
not swim adds one to a count kept by the cell for that living, and at
`WATER_DRIFT_TIME / HEART_BEAT_TIME` beats it is dragged (a stroke still on its
way delays it to the next beat); the count restarts after the drag and is
forgotten when the living leaves (`event_exit`).

A ghost is dragged like anybody, cannot swim (the skill is refused to the
dead) and walks the swum exits by typing the direction, in water or from a
coast, as a spirit.

## Things in the current

Anything that is not a living and can be picked up is carried off the moment
it falls into a cell with a current (`query_water_flows`: strength above 0 and
the exit it flows out by exists), in its `event_enter`, with no timer
(`water_carry(ob)` in `/lib/room/water.c`):

- if the cell has a swum exit to a coast, the thing is washed onto that coast
  (one of them at random), out of the water;
- otherwise it goes on to the cell the current flows out by, whose own
  `event_enter` carries it on again, cell after cell, until a coast takes it.
  The cells crossed are counted in the static property `WATER_CARRY_PROP` of
  the thing; at `WATER_CARRY_MAX_CELLS` (20) it stops, so a loop ends.

The room it leaves and the one where it lands are told. Where the current has
no exit to flow out by, or the water is still, things stay. A bridge cell
carries nothing by itself: what lies on the bridge stays, and only what the
water takes (a corpse and gear, below) is carried with `water_carry(ob)`.

## Corpses

Whoever dies where the current would drag them (`query_water_drags(who)`: in the
water of a cell with a flowing current, in a bridge cell only if in the water)
leaves their gear in the cell instead of in the corpse, money and worn items
included, and each piece drifts on its own like the corpse itself. On dry land
and in still water the gear stays in the corpse.

## Creatures

An NPC template with `swims` (a fish) walks swum exits like ordinary ones and
finds paths through them, and is the only kind the area materializes in a
water location, whatever the rooms declared. Every other NPC stays out of the
water.

## Bridges

A bridge cell is one location at one coordinate that is a bridge and water at
once: the water rules above (current, strength, upstream, swept_to, depth,
drift) plus exits of type `bridge`, which are walked. Every other exit of the
cell is swum, as in water, except a door or a gate across the bridge and a
road or a path (the deck, which still counts as a road on the world map), which
are walked. The rules live in `/lib/room/bridge.c`, the room base is
`/lib/bridge.c` and a location gets a `bridge` component from `room2loc` in place of the water one, with the same water settings.

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
  into the water of the same cell and starts counting the beats of the drift.
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

## On the world map

Rivers are drawn per sector, like roads, in blue box lines (double lines by
default). The sector records, for every water cell with a flowing current (a
bridge cell counts), its swum exits (`river_exits`, beside `way_exits`), and
flags each water and bridge cell in its nodes. An exit is a river line only when
it joins two water cells; still water (no current) is a zone and draws nothing.
`query_border_rivers()` gives the borders of the sector a river crosses, by the
same rules as `query_border_ways()` except that a diagonal that changes sector
on both axes is recorded on both borders. The renderer
(`/lib/handlers/worldmap.c`) ranks a sector: city, then road, then forest,
then river or lake, then the rest of the terrain, so a river is drawn over farmland but
a forest keeps its size.

A lake is drawn apart. The sector flags every water cell whose kind is `lake`
(`nodes[...]["lake"]`, lifted by `/lib/handlers/sectors.c` from the water
component's `query_water_kind()`), flowing or still; `count_lake_cells()`
counts them and `has_lake()` says whether there is any.
A sector with a lake cell draws the lake glyph (`MAP_GLYPH_LAKE`): the white
circle of an islet (`GLYPH_SHORE_SPOT`) in the blue of the rivers
(`COLOUR_RIVER`). It ranks where the river does, so a coast still outranks it,
and it replaces the river line even when a river flows out of the lake; a road
through a lake sector keeps the road glyph. A big lake, `LAKE_OVERRIDE_MIN` (10)
cells or more, draws the lake whatever the sector's road or terrain, as many
ruins do, so a lake inside a forest is not hidden by the trees. The world map legend lists it as
"lake" (`lago`).

Where a road and a river share a sector, a straight road across a straight
river shows a crossing glyph in blue, and any other shape the road glyph.
`WORLDMAP_RIVER_DOUBLE` switches rivers between double and single lines.
Each sector keeps the code of its glyph, worked out whenever the sector
changes (`save_me()` of `/lib/location/sector.c`, which also redoes its four
neighbours); after changing how the map is drawn, run `sectors glyphs`.
