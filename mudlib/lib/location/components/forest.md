# Forests

A location with the `forest` component is woodland. It has a `depth` (garden <
thicket < forest < jungle) and, through its area, a woodland **kind**.

## Kind

The area keeps the kind (`set_forest_kind`, saved with the area);
`query_forest_kind()` answers the area's own or, when it names none, the
nearest parent's. The admin command `forest` shows it and `forest kind <kind>`
(or `none`) sets it, then plants again the trees of the loaded locations of the
area. The kinds are the game's: its `forests` table.

## What grows

| depth | locations with trees | trees and shrubs |
|---|---|---|
| garden | 30% | 1 |
| thicket | 60% | 1-2 |
| forest | 90% | 2-3 |
| jungle | 100% | 3-4 |

The trees and shrubs are planted the first time a living walks into the location
(not when it loads, so loading a whole map clones nothing). They are fixtures,
chosen from `query_plants(kind)` of the table, and go with the location.

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
