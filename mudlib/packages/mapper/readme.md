# mapper

Dumps a player-safe map of an area to JSON, for a public map viewer: every
location, its name, where it is, how locations connect and which area each one
belongs to. Nothing else leaves the mud.

## Command

Admin level, `mapper`:

```
mapper update <area dir> [all]   one area, or with every sub-area under it
mapper list [<game>]             maps written and when
```

`<area dir>` is an area folder such as `/games/<game>/areas/<area>`, with or
without a trailing slash; a relative path is resolved against the current
folder. Without `all` only the locations whose nearest area (the closest folder
at or above them with an `area.o`) is that area go in; folders of other areas
under it are skipped. With `all` every sub-area is included.

The map is written to `/games/<game>/docs/maps/<area path with - for />.json`,
for instance `<area>.json` or `<area>-<sub-area>.json`. The work is split in
steps (call_outs) and the command says when it is done.

## Where the data comes from

The saved locations, `/save/games/<game>/locations/areas/<area>/**.o`, read
with a helper object (`src/reader.c`) through `restore_object`: no location is
loaded, so no NPC is cloned. A save without `coordinates` or `file_name` is
skipped. Area names and parents come from the `area.o` saves.

## Format

```json
{ "format": "mapper/1", "game": "...", "area": "...", "name": "...", "generated": "YYYY-MM-DD",
  "areas": [ { "id": "...", "name": "...", "parent": "..." } ],
  "levels": [ 0 ],
  "locations": [ { "id": "...", "name": "...", "area": "...", "x": 0, "y": 0, "z": 0,
                   "exits": [ { "dir": "north", "to": "..." } ] } ],
  "external": [ { "from": "...", "dir": "...", "area": "...", "to": "..." } ] }
```

- `format`: `mapper/1`.
- `game`: the game folder name.
- `area`, `name`: the dumped area, as a path under the game's `areas/` folder, and its display name.
- `generated`: the day of the dump (UTC).
- `areas`: every area that has locations in the dump, sorted by `id`. `id` is the path under the game's `areas/` folder; `name` is the area's display name, else its folder name (the folder above when that is `rooms`); `parent` is the parent area's id, or empty. A parent may not be listed when it has no locations in the dump.
- `levels`: the distinct `z` values, ascending.
- `locations`: sorted by `id`, one per line. `id` is the save path inside the dumped area without `.o`. `name` is the `home` component's `home_short`, else the specific short, else the original short, colour codes stripped. `area` is the id of the area the location belongs to. `x`, `y`, `z` are the saved coordinates (`y` grows to the north). `exits` are the visible exits to other locations of the dump: `dir` is the direction as saved (canonical English) and `to` the target `id`.
- `external`: visible exits that leave the dump. `from` is a location `id`; `area` is the target's area id and `to` the target's id inside that area (not inside the dumped one).

Output is sorted so that a regenerated map gives a small diff.

## What is left out

- File paths of any kind (`/save/...`, `/games/...`): ids are relative.
- Hidden and secret exits (exit type `hidden` or `secret`), so a map never reveals a way players are meant to find.
- Locks, keys and door state: a door is a normal exit.
- Exits whose target is not a saved location of the game.
- NPCs, items, quests, components, descriptions and every other internal field.
