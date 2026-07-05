# Mazes — storage and API reference

How the maze flag propagates from the source room down to the sector
index, and which accessors to use for pathfinding / cartography.

## 1. Room source

Builders declare a labyrinth room by adding `MAZE_PROP` to its
properties. Nothing else on the room side.

```c
inherit "/lib/room.c";

void setup()
{
  set_short(...);
  set_long(...);
  add_property(MAZE_PROP, 1);
  add_exit(DIR_WEST,      ROOMS + "other.c", "forest");
  add_exit(DIR_NORTHWEST, ROOMS + "sibling.c", "forest");
}
```

`MAZE_PROP` lives in `include/room/room.h`. Exits stay
geographically coherent — the maze **is not** randomised at runtime;
disorientation is achieved through the intentionally contradictory
exit graph the builder declares (see `dev/maze-orientation.md`).

## 2. Conversion to a location

`room2loc` runs `LOCATION_HANDLER::convert_room_to_location`, which
detects `room->query_property(MAZE_PROP)` and attaches the
`LOCATION_COMPONENT_MAZE` component to the new location. The
component itself carries no data today — it is a marker.

```c
#define LOCATION_COMPONENT_MAZE "maze"   // include/room/location.h
```

Location-side accessor:

```c
nomask int query_maze() { return !undefinedp(component_info[LOCATION_COMPONENT_MAZE]); }
```

Coordinates are inferred the normal way through `guess_coordinates`
walking the exit graph — the maze is NOT excluded from coord
inference. l42, l43, l44, 41 all get real `(x, y, z)` slots.

## 3. Sector index

`MAPS_HANDLER::add_location` lifts the maze flag off the location and
forwards it in the `location_data` mapping passed to the sector:

```c
mapping location_data = ([ ]);
if (location->query_component_by_type(LOCATION_COMPONENT_MAZE))
  location_data["maze"] = 1;
sector_storage->add_location(file_name, x, y, z, location_data);
```

The sector stores maze coords in a parallel mapping:

```c
mapping positions;       // "x_y_z" -> file_name    (all locations)
mapping maze_positions;  // "x_y_z" -> 1            (only maze locations)
```

Accessors:

```c
sector->query_positions();
sector->query_maze_positions();
sector->is_maze_at(x, y, z);   // returns 1 if the coord is a maze
```

`remove_location` keeps both mappings in sync.

## 4. Area accessor

The area exposes a derived query — no separate storage, filters
`loaded_locations` at call time:

```c
area->query_maze_locations();  // object * of loaded maze locations
```

## 5. Collision warning

When `add_location` in `MAPS_HANDLER` writes a coord pointer file and
finds an existing pointer to a different `file_name`, it logs to
`log_file("maps_collision", ...)` and emits a stderr warning. See
`lib/handlers/maps.c::add_location`. A data bug on the room side is
the usual cause.

## 6. `guess_coordinates` guard

`location.c::guess_coordinates` early-returns when the location
already has coordinates. Re-imports do NOT clobber builder-set or
previously-inferred coords.

## 7. Consumer contract for pathfinding

An A* / BFS over the sector graph should treat maze coords as opaque
by default:

```c
// pseudo
walkable(key) = positions[key] && !maze_positions[key];
```

Callers that DO want to route through mazes (e.g. an NPC bound to a
specific maze) opt in explicitly by removing the maze filter.

## 8. Files

- `include/room/room.h` — `MAZE_PROP`.
- `include/room/location.h` — `LOCATION_COMPONENT_MAZE`.
- `lib/location/components/maze.c` — marker component.
- `lib/location.c::guess_coordinates` — inference with clobber guard.
- `lib/location.c::query_maze` — location-side flag accessor.
- `lib/location/area.c::query_maze_locations` — area-side derived accessor.
- `lib/handlers/maps.c::add_location` — lifts flag, forwards to sector, collision warning.
- `lib/maps/sector.c` — `maze_positions` mapping, `is_maze_at`, accessors.
- `dev/maze-orientation.md` — future skill mechanics (not yet implemented).
