# Riding: mounts and vehicles

A **steed** is anything a living can be carried by: a horse, a dragon, a motorbike, a boat. A **rider** is any living on a steed; seat 0 is the **driver** and the others are passengers. Everything a ride does is data plus one handler: nothing is shadowed or cloned onto the rider.

| piece | file |
|---|---|
| the rider's state (`query_riding`, `query_mount`, `query_seat`) | `/lib/living/riding.c`, inherited by `/lib/living/living.c` |
| the rules, the movement, the falls | `/lib/handlers/riding.c`, per game: `handler("riding", ob)` |
| the kinds and every text | `/lib/tables/steeds.c`, per game: `table("steeds", ob)` |
| an NPC steed | component `rideable` (`/lib/npc/components/rideable.c`) |
| an item steed | `/lib/obj/vehicle.c` |
| spec keys | `<living/riding.h>` |

A game's handler inherits the shared one and redefines what differs; a game's table inherits the shared one and adds kinds with `add_kind` (rl-aeternum: `horse`).

## A steed and its spec

A steed answers `query_rideable()` with a mapping (nil: it cannot be ridden). The NPC gets it from its `rideable` component, whose attributes are the mapping; a vehicle holds it from `set_rideable(spec)` (kept in its auto-load attributes). `query_spec(steed)` of the handler lays three mappings over each other, the last one winning: the default row of the table (`mount`), the row of the steed's `kind`, the steed's own keys. So a template says `"rideable": { "kind": "horse" }` and may add any key to differ from the kind.

| key | meaning |
|---|---|
| `kind` | row of the steeds table; unknown kind: the default row |
| `seats` | how many ride at once (default 1) |
| `medium` | `ground`, `water`, `air`: what a place beyond an exit may be. A water place needs `water`; any other needs `ground` or `air` |
| `terrain_deny` | `indoors` (a destination that is not `query_outside()`), `underground` (`query_underground()`) |
| `terrain_only` | exit types it is limited to; empty: any |
| `exits_deny` | exit types it cannot use (`rope`, `stair`, `door`...). The refusal is `refuse_<type>` of the row if it has one, else `refuse_exit` |
| `exit_size` | smallest exit size it fits (the size its exit type has in `room_handler.c`: door 5, corridor 10, gate 15, road 30). An exit whose type has no size does not hold it back |
| `requires` | what the driver needs: `"skill": ({ id, minimum })`, `"body_size": ({ smallest, biggest })` (race body size) |
| `learn` | skill id the driver is given at 10% the first time they take the driver's seat without knowing it. It counts as known at 10 for `requires`, so a first ride is never refused for lack of it |

Only the driver's seat asks for `requires` and `learn`; any seat may be taken by whoever fits.

## The row: texts

Besides the spec keys a row holds every text a ride says. `$N` is the rider's name, `$S` the steed's short in lower case, `$T` the direction and `$F` where the rider comes from (these two for the step lines, filled in by `move_living`). A key a kind leaves out is read from the default row.

- `verbs_board`, `verbs_dismount`: the verbs the steed answers to (an NPC through its component, a vehicle in `init`). `prepositions`: little words dropped after the verb (`montar sobre el caballo`).
- `board_me` / `board_room`, `dismount_me` / `dismount_room`, `fall_me` / `fall_room`.
- `leave`, `arrive`: replace the race's move lines while mounted; they start with `@`, the marker `move_living` strips. An exit that has its own message keeps it.
- `listing`: follows the rider's name in a room listing. `look`: the line `look` adds to the rider.
- `refuse_<reason>`: `dead`, `passed_out`, `already`, `full`, `skill`, `body_size`, `passenger`, `exit`, `exit_size`, `terrain`, `indoors`, `underground`, `water`, `ground`, plus one per exit type a kind denies.

## Handler API (`/lib/handlers/riding.c`)

- `query_spec(steed)`, `query_spec_of(steed, own)` (for a steed still being put together), `query_steed(ob)`.
- `query_riders(steed)` (driver first; not stored: the livings in the steed's place whose `query_mount()` is it), `query_seat_taken`, `query_free_seat`, `query_addressed(steed, str)` (whether the words after a verb name the steed).
- `query_board_refusal(who, steed, seat)` (a text for the rider, or nil; seat -1 is the first free one), `board(who, steed)`, `board_seat(who, steed, seat)`, `dismount(who, fall)`, `unseat_riders(steed [, place])`.
- `query_move_refusal(who, room, dir)`: asked by the exit handler of whoever rides; a passenger cannot move the steed.
- `query_travel_line(who, arriving)`, `query_listing(who)`, `query_look(who)`.
- `carry(who, from)`, `rider_moved`, `steed_moved`: movement, below.

Hooks the handler calls on the steed, all optional (they are plain `call_other`: a steed that does not define one answers nil): `steed_can_board(who, seat)` (a refusal text, or nil), `steed_boarded(who)`, `steed_dismounted(who)`. A hand-written NPC or a vehicle defines them.

## Movement

- The exit handler asks `query_move_refusal` before a mounted rider leaves (`do_exit_command`): seat, exit type, size, what lies beyond.
- `move_living` writes the step lines from `query_travel_line` while the living rides, then calls `carry(rider, from)`: the steed and the passengers are moved after the driver with a plain `move(dest)`, with no lines (the driver's line is the room's). If the steed cannot be moved the driver falls.
- A rider moved by anything else (`goto`, a spell, a push) reaches `event_moved` of `/lib/living/movement.c`, which is not its own walk, so `rider_moved` throws it off. A steed moved while it is not following its driver (`event_moved` of the NPC or the vehicle) throws off whoever is on it with `steed_moved`. The two walks that are its own are flagged (`walking` on the rider, the handler's `carrying` list for the steed and the passengers).
- A steed that dies, or is destructed or has its component taken out, unseats its riders (`unseat_riders`). A rider that dies is dismounted.
- Every fall is words only: no damage.
- An NPC steed with somebody on it does not wander (`query_staying_put` of `/lib/npc.c`), and neither steed nor ridden vehicle shows in the room list (`pretty_short` empty): the rider does, with its `listing`.

## Skills

`ride`, `drive` and `pilot` are common passive skills (`lib/obj/skills/`), percentage, ids in `<living/skills.h>`, names and help in `lib/obj/skills/.lang.<lang>.h`. Nothing teaches `drive` or `pilot` yet; a steed that wants one in its `requires` and `learn` gets it granted at 10%.
