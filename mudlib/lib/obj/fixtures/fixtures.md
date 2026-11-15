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

## Ropes

A rope tied to something opens an exit that is sealed without it. The room
declares the exit as usual, with the type `"rope"` and the option `sealed`, and
has no code for the rope:

    add_exit(DIR_DOWN, ROOMS + "ravine.c", "rope", nil, ([ "sealed": 1 ]));

The exit at the other end of a climb (the way back up) is a plain `"rope"` exit
and stays open. The `rope` component
(`/lib/location/components/rope.c`) is attached to the location by an admin,
standing in it:

    components add rope down a tree

The words after the direction say what the rope is tied to (optional). The
component is saved with the location, and `room2loc reload` keeps it. "tie
rope" takes the first thing the player carries that answers `query_rope()` (`/lib/obj/rope.c`), destroys it and
reveals the exit; "untie rope" clones it again into the player (or the floor)
and seals the exit. The file of the tied rope is saved with the location, so the
exit is open again on every load while a rope is tied. While tied, the look of
the location gets a line about it and `look rope` describes it.

## Fixtures

A fixture is an object that belongs to the place: it cannot be taken, weighs
nothing and answers `query_fixture()`. Every fixture inherits
`/lib/obj/fixtures/fixture.c`.

A location keeps the list of its fixtures (`query_fixtures`, blueprint ->
count) and clones them again every time it loads. `room2loc` fills the list
from the room's `add_clone` of anything that is a fixture, so a room that
clones its tree converts into a location that has it. `locations fixture` lists,
adds and removes them by hand.

A fixture declares the actions it answers to with
`add_fixture_action(function, verbs)` in its `setup()`, never with a bare
`add_action`: the base registers them on whoever comes near, and looking at the
fixture ends with the same line a prop shows ("You can: fill, drink."), with the
first verb of each action. When what somebody can do depends on them or on the
fixture's state, it redefines `query_usage_verbs(viewer)`; `query_actions_hint`
gives the line, for a fixture that builds its own long.

`force_exit(dir, who, leave, arrive)` takes two optional messages that replace
the usual departure and arrival: `leave` is what the place left reads (`$N` who,
`$T` the way they go), `arrive` what the place reached reads (`$N`).

Three bases put both pieces together:

- `/lib/obj/fixtures/climbable.c`: climbed with the climb skill (`set_climb_modifier`);
  reaching the top takes the climber through `set_climb_exit`. Climbing is the
  skill's, not an action of the fixture, so its verb shows in the line only to
  whoever knows the skill.
- `/lib/obj/fixtures/movable.c`: moved with `mover` / `move`; it reveals
  `set_reveal_exit`, with the messages given to `set_move_messages`. With
  `set_pass_through(leave, arrive)` it reveals nothing: it takes the mover alone
  through the sealed exit, which stays sealed, with those messages in the place
  left and the place reached.

- `/lib/obj/fixtures/receptacle.c`: receives an item from the player (see below).

Each place has its own file that inherits one of them, gives the description
and, if it needs to, redefines `event_climbed` or `do_move` to do something of
its own.

## Receptacles

`/lib/obj/fixtures/receptacle.c` is the fixture for a thing that takes an item:
a pedestal that takes an idol, a chest opened with its key, an altar that takes
a sacrifice. Its vocabulary is the one of the NPC component `receive`
(`/lib/npc/components/receive.c`). The fixture gives its description and calls
`set_receptacle(mapping)` in `setup()`; every attribute is optional:

- `items`: what is accepted, full file paths with `.c` (compared exactly) or
  ids; nothing listed accepts any item.
- `verbs`: the verbs, with the syntax `<verb> <item> [in <fixture>]`.
- `carried`: 1 for a key: the syntax is only `<verb> <fixture>` ("open chest")
  and the item is looked for in what the player carries.
- `consume`: 1 destroys the item. `once`: `"player"` (kept as a property of the
  player) or `"fixture"`.
- `requires_quest`: `"<game>:<quest id>"`, only for who is doing it.
- Texts: `message` (to the giver), `room_message` (to the others), `refusal`
  (nothing accepted is carried), `refusal_room`, `spent` and `denied` (quest not
  being done). They may carry `$player$` and `$item$`; without them the base
  has a stock line for `refusal`, `spent` and `denied`.
- Effects, in this order: `property` (a name, or `({ name, value })`, set on the
  giver), `give` (files cloned for the giver), `quest` (`([ "id", "action",
  "source" ])` as in `receive`), `reveal` (a sealed exit; `reveal_time` seals
  it again after that many seconds) and `pass` (the giver alone goes through a
  sealed exit; `pass_leave` and `pass_arrive` are the messages of
  `force_exit`).

For what the attributes cannot say, the inheritor redefines:

- `int can_receive(object item, object who)`: a condition, called with the item
  found or nil when the player carries nothing accepted; it tells the player
  itself when it stops (returns 0).
- `void received(object item, object who, string target)`: called after the
  item is taken and the effects are done, for the puzzles that keep their own
  logic. `query_receive_target()` is the fixture phrase of the command while
  `can_receive` runs.
- `is_wanted`, `find_item`, `is_target` and `parse_receive` change how the item,
  the fixture and the command words are matched (an item known by a method
  instead of an id, a fixture phrase such as "head 1").
- `take_item(item, who, target)` does the taking for a fixture that learns of
  the item by other means (an item put on it with the common command).

The player texts of the base (the questions and stock lines) live in
`.lang.<lang>.h`, as in every lib object.
