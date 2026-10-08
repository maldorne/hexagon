# Light

How much a player sees of the place they are in depends on three things: how
much light the place has, the hour if the place is outdoors, and the eyes of the
player's race. This file follows that chain from the bookkeeping in `light.c` to
what `look` prints.

## The bookkeeping

Every object inherits `/lib/core/basic/light.c` and keeps two numbers:

- `light`, the object's own light: what a torch gives off, what a room is lit
  with. Set with `set_light(n)`, read with `query_my_light()`.
- `int_light`, the light of everything inside it, added up. Read with
  `query_int_light()`.

`query_light()` is the sum of both. A player carrying a lit torch has an
`int_light` of 50, and so does the room the player stands in.

Neither number is saved: light is rebuilt every time an object is created and
moved, so a torch comes back unlit after a reboot.

The totals are kept up to date incrementally, never recalculated:

- `adjust_light(i)` adds `i` to an object's `int_light` and passes the same
  `i` up to its environment, so a change deep in an inventory reaches the room.
- `set_light(n)` passes the difference between the old and the new own light
  up to the environment.
- `move()` (`/lib/core/basic/move.c`) takes the object's whole `query_light()`
  out of the place it leaves and puts it into the place it reaches, after the
  move itself and before the `enter` event, so whoever is already there sees
  by the newcomer's torch when told of the arrival.
- `destruct()` (the efun in `/lib/core/efuns/move.c`) gives the light and the
  weight of the object back to its environment. `dest_me()` ends in it, so both
  ways of removing an object leave nothing behind.

## Rooms

`/lib/room.c` starts every room at `BASE_ROOM_LIGHT_VALUE` (50,
`<basic/light.h>`); a room changes it in its `setup()` with `set_light(n)`. A
room indoors is that light, plus whatever is carried in, at any hour.

An outdoor room (`/lib/outside.c`) is dimmed by the hour and the weather:

```
query_light() = query_my_light() * darkness / 100 + query_int_light()
```

`darkness` is a percentage from the weather handler, `query_darkness(room)`
(`/lib/handlers/weather.c`):

| when | percentage |
|---|---|
| day | 100, minus a quarter of the rain and a fifth of the wind of the zone |
| the hour after dawn and the hour before nightfall | 10 less |
| night | 50 |
| night, moon partly showing | 60 |
| night, full moon | 70 |

Only the place's own light is dimmed. Light carried in, a torch, shines the same
outdoors at night as it does indoors.

## Locations

A location (`/lib/location.c`) keeps its own light the same way and also starts
at 50. What a room sets in `setup()` a location keeps as its base light:

- `set_base_light(n)` stores it and applies it; `query_base_light()` reads it
  (50 when nothing was stored).
- It is saved with the location as a one-element array, so that a light of 0,
  a cave, is saved too, and it is applied again when the location loads.
- Converting a room into a location (`room2loc`, the location handler) copies
  the room's own light, `query_my_light()`. Not `query_light()`: an outdoor room
  answers that already dimmed by the hour.

`query_light()` on a location runs the `query_light` hook of its components,
summing what they return over the location's own total. The `outside`
component answers that hook with the same formula as an outdoor room, using the
weather handler resolved against the location, and takes authority over the
sum (`HOOK_EXCLUSIVE`). A location without it is lit like an indoor room.

## The viewer

Only players judge how dark a place is. `check_dark(light)`
(`/lib/player/dark.c`) asks the player's race, `query_dark(light)` in
`/lib/race.c`, which compares the light with the race's two limits, `low` and
`high`, set with `set_light_limits(low, high)`:

| level | light | meaning |
|---|---|---|
| 0 | from `low + 5` to `high` | normal sight |
| 3 | from `low` to `low + 4` | dim |
| 2 | from `low - 5` to `low - 1` | dark |
| 1 | below `low - 5` | absolute darkness |
| 4 | from `high + 1` to `high + 20` | dazzled |
| 5 | from `high + 21` to `high + 40` | glaring |
| 6 | above `high + 40` | blinded |

The usual limits are `LIGHT_STD_LOW` / `LIGHT_STD_HIGH` (20 / 200,
`<living/races.h>`): a human sees normally from 25. A race that sees in the
dark uses `LIGHT_NIGHT_VISION_LOW` (10) as its lower limit and sees normally
from 15.

A dead player always sees (level 0). NPCs do not check darkness at all.

## What each level shows

The levels are interpreted in `/lib/room/dark.c`, which rooms and locations both
inherit:

- **0**: everything: name, description, weather, exits, props, contents.
- **2 to 5**: the description is replaced by a message saying the details
  cannot be made out (`query_dark_mess(level)`); the name, weather, exits,
  props and contents are still seen. Looking at something in particular also
  answers with the message.
- **1 and 6**: only the message. `query_dark_hides_place(level)` is true for
  these two.

`look` (`/lib/cmds/player/look.c`) prints the name unless the level hides the
place, then the place's `long()`, which applies the rules above. `glance`, the
short view shown on arrival, never shows the description, so at levels 2 to 5
it only adds the message above its usual line.

Rooms, outdoor rooms and locations implement this in their own `short()` and
`long()`: a location does it itself, whatever components it has.

A blind player (the `blind` property, `BLIND_PROP`, timed or not) sees nothing
in `look` and `glance` regardless of the light. Coders are not affected by
blindness, but they see darkness like anybody else.

## Light sources

`/lib/obj/torch.c` gives a light of 50 while it is lit. Lighting and putting it
out call `set_light`; it goes out when it stops being held, so a lit torch is
never dropped or given away.

Anything can be a light source the same way: `set_light(n)` on the object and
the bookkeeping above carries it to the room.

## Writing a place

- Set the light in the room's `setup()`: 50 is a normal place, 60 a well-lit
  one, 30 or 40 a gloomy one (a forest, a cellar), 0 a place that needs a torch.
- Outdoors, remember the night halves it: 60 is still seen at night, 40 loses
  its description for a human unless the moon is full, 20 or less is dark
  without a torch.
- For a location already converted, change its light with `set_base_light(n)`
  and save it.
