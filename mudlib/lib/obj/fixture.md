# Fixtures and sealed exits

A place often hides a way on that only an action opens: a tree to climb, a
stone to move, a bookcase that swings aside. Two pieces make that work the same
in rooms and in locations.

## Sealed exits

An exit declared with the option `sealed` is a real exit:

    add_exit(DIR_UP, ROOMS + "crown.c", "stair", nil, ([ "sealed": 1 ]));

It gives the place on the other side its coordinates when the area is
converted, and it is part of the sector graph. But nobody sees it in the exit
list, and typing its direction does nothing. Three functions of every room and
location (`/lib/room/exits.c`) use it:

- `force_exit(dir, who)` moves somebody through it, with the usual checks and
  messages.
- `reveal_exit(dir)` opens it: from then on it is listed and works like any
  other exit. `seal_exit(dir)` closes it again. Either way, it is sealed again
  when the place loads, as a door goes back to its default state.
- `query_sealed_exit(dir)` says whether it is sealed right now.

NPCs walking a route (`find_path`) leave sealed exits out unless asked to take
them.

## Fixtures

A fixture is an object that belongs to the place: it cannot be taken, weighs
nothing and answers `query_fixture()`. Every fixture inherits
`/lib/obj/fixture.c`.

A location keeps the list of its fixtures (`query_fixtures`, blueprint ->
count) and clones them again every time it loads. `room2loc` fills the list
from the room's `add_clone` of anything that is a fixture, so a room that
clones its tree converts into a location that has it. `build fixture` lists,
adds and removes them by hand.

Two bases put both pieces together:

- `/lib/obj/climbable.c`: climbed with the climb skill (`set_climb_modifier`);
  reaching the top takes the climber through `set_climb_exit`.
- `/lib/obj/movable.c`: moved with `mover` / `move`; it reveals
  `set_reveal_exit`, with the messages given to `set_move_messages`.

Each place has its own file that inherits one of them, gives the description
and, if it needs to, redefines `event_climbed` or `do_move` to do something of
its own.
