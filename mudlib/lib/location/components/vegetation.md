# Vegetation

Four components grow plants: `forest`, `taiga`, `tundra` and `swamp`. All of
them inherit `/lib/location/components/vegetation.c`, which holds everything
described here; each one only sets its own type, so the sector tallies and the
world map are unchanged (a location votes forest, taiga, tundra or swamp as
before). `swamp.c` adds its own behaviour on top (it holds back whoever tries to
leave). Through its area a location has a vegetation **kind** and a **depth**
(garden < thicket < forest < jungle).

## Kind and depth

The area keeps both (`set_vegetation_kind`, `set_vegetation_depth`, saved with
the area); `query_vegetation_kind()` and `query_vegetation_depth()` answer the
area's own or, when it names none, the nearest parent's. Areas saved before the
rename hold the values as `forest_kind` and `forest_depth`; they are read back
into the new fields when the area is restored.

The component's `query_vegetation_kind()` answers the area's kind; when no area
up the chain names one, the kind named like the component's own type (`swamp`,
`tundra`, `taiga`, `forest`) if the game's table knows a kind of that name,
else "" and nothing grows. A location may carry a depth of its own
(`set_depth` on the component, saved as `own_depth`), which wins over the
area's; `query_depth()` answers the one that applies ("forest" when nobody names
one). The sector tally of the world map does not read the depth.

The admin command `vegetation` shows them: `vegetation kind <kind>|none` and
`vegetation depth <depth>|none` set them on the area. Neither touches the plants
already planted; they count the next time plants are rolled. `vegetation
replant` takes away the plants of the location you stand in and rolls them
again; `vegetation replant area` does it in every loaded location of the area
that has a vegetation component, a few per call_out since each one saves its
location (the unloaded ones keep their saved plants). A location with more than
one vegetation component is acted on in all of them. The kinds are the game's:
its `vegetation` table.

A location where nothing can take root (a walkway in the branches, a bridge, a
pier, the canopy) keeps the component, so it still counts as its terrain on the
map, and is marked as growing no plants: `vegetation plants off`
(`set_grows_plants(0)` on the component, saved as `grows_plants`) takes its
plants away and no trees, shrubs or herbs grow there; `vegetation plants on`
undoes it.

## What grows

| depth | locations with plants | trees and shrubs |
|---|---|---|
| garden | 30% | 1 |
| thicket | 60% | 1-2 |
| forest | 90% | 2-3 |
| jungle | 100% | 3-4 |

The trees and shrubs are fixtures chosen from `query_plants(kind)` of the table.
They are planted when the location loads (`components_ready`), and the component
saves what it planted as `plants` (path -> amount) in the location's file. Every
later load clones exactly those again. New ones are rolled only when none is
saved and no plant (`query_plant()`) stands in the location; a roll that gives
nothing is rolled again at the next load. The location is saved
(`components_changed`) whenever the plants change.

A plant that is gone is forgotten: the saved amounts are recounted from the
plants that still stand in the location every time it saves, and
`remove_plant(ob)` (for cutting a tree down) destroys it, forgets it and saves at
once.

Herbs: a player who knows herbalism walking in may find one growing, at most one
at a time and once a minute per location. The table decides which and how often
by depth (`query_herb_to_grow`); the herb is a growing herb that the gather
skill picks.

## The game's vegetation table

`table("vegetation", location)` must answer:

- `query_kinds()` -> the kinds of the game
- `query_plants(kind)` -> object paths of trees and shrubs, each repeated as
  many times as its weight
- `query_seeks_herbs(who)` -> whether `who` can see herbs
- `query_herb_to_grow(who, kind, depth, place)` -> a herb ready to be placed, or nil;
  `place` is the location, so the table may also look at its other components

A game without the table, or a location with no kind, grows nothing.
