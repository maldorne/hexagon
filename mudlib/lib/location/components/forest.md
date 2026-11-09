# Forests

A location with the `forest` component is woodland. Through its area it has a
woodland **kind** and a **depth** (garden < thicket < forest < jungle).

## Kind and depth

The area keeps both (`set_forest_kind`, `set_forest_depth`, saved with the
area); `query_forest_kind()` and `query_forest_depth()` answer the area's own or,
when it names none, the nearest parent's. A location may carry a depth of its
own (`set_depth` on the component, saved as `own_depth`), which wins over the
area's; the component's `query_depth()` answers the one that applies ("forest"
when nobody names one). The sector tally of the world map only counts forest
locations and does not read the depth.

The admin command `forest` shows them: `forest kind <kind>|none` and
`forest depth <depth>|none` set them on the area. Neither touches the trees
already planted; they count the next time trees are rolled. `forest replant`
takes away the trees of the location you stand in and rolls them again;
`forest replant area` does it in every loaded forest location of the area (the
unloaded ones keep their saved trees). The kinds are the game's: its `forests`
table.

## What grows

| depth | locations with trees | trees and shrubs |
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

A tree that is gone is forgotten: the saved amounts are recounted from the plants
that still stand in the location every time it saves, and `remove_plant(ob)` (for
cutting a tree down) destroys it, forgets it and saves at once.

Herbs: a player who knows herbalism walking in may find one growing, at most one
at a time and once a minute per location. The table decides which and how often
by depth (`query_herb_to_grow`); the herb is a growing herb that the gather
skill picks.

## The game's forests table

`table("forests", location)` must answer:

- `query_kinds()` -> the kinds of the game
- `query_plants(kind)` -> object paths of trees and shrubs, each repeated as
  many times as its weight
- `query_seeks_herbs(who)` -> whether `who` can see herbs
- `query_herb_to_grow(who, kind, depth)` -> a herb ready to be placed, or nil

A game without the table, or an area without a kind, grows nothing.
