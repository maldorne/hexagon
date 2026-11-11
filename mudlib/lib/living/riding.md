# Riding: mounts and vehicles

A **mount** is anything a living can be carried by: a horse, a dragon, a motorbike, a boat. A **rider** is any living on a mount; seat 0 is the **driver** and the others are passengers. Everything a ride does is data plus one handler: nothing is shadowed or cloned onto the rider.

| piece | file |
|---|---|
| the rider's state (`query_riding`, `query_mount`, `query_seat`) | `/lib/living/riding.c`, inherited by `/lib/living/living.c` |
| the rules, the movement, the falls | `/lib/handlers/riding.c`, per game: `handler("riding", ob)` |
| the kinds and every text | `/lib/tables/mounts.c`, per game: `table("mounts", ob)` |
| an NPC mount | component `rideable` (`/lib/npc/components/rideable.c`) |
| an item mount | `/lib/obj/vehicle.c` |
| the mounts a player owns | one save file each, below |
| a place that sells mounts | location component `mount-shop` (`/lib/location/components/mount-shop.c`) |
| the player's list | the `mounts` / `monturas` command (`/lib/cmds/player/mounts.c`) |
| spec keys | `<living/riding.h>` |

A game's handler inherits the shared one and redefines what differs; a game's table inherits the shared one and adds kinds with `add_kind` (rl-aeternum: `horse`).

## A mount and its spec

A mount answers `query_rideable()` with a mapping (nil: it cannot be ridden). The NPC gets it from its `rideable` component, whose attributes are the mapping; a vehicle holds it from `set_rideable(spec)` (kept in its auto-load attributes). `query_spec(mount)` of the handler lays three mappings over each other, the last one winning: the default row of the table (`mount`), the row of the mount's `kind`, the mount's own keys. So a template says `"rideable": { "kind": "horse" }` and may add any key to differ from the kind.

| key | meaning |
|---|---|
| `kind` | row of the mounts table; unknown kind: the default row |
| `seats` | how many ride at once (default 1) |
| `medium` | `ground`, `water`, `air`: what a place beyond an exit may be. A water place needs `water`; any other needs `ground` or `air` |
| `terrain_deny` | `indoors` (a destination that is not `query_outside()`), `underground` (`query_underground()`) |
| `terrain_only` | exit types it is limited to; empty: any |
| `exits_deny` | exit types it cannot use (`rope`, `stair`, `door`...). The refusal is `refuse_<type>` of the row if it has one, else `refuse_exit` |
| `exit_size` | smallest exit size it fits (the size its exit type has in `room_handler.c`: door 5, corridor 10, gate 15, road 30). An exit whose type has no size does not hold it back |
| `requires` | what the driver needs: `"skill": ({ id, minimum })`, `"body_size": ({ smallest, biggest })` (race body size) |
| `swamp_walker` | 1 when it crosses a bog without sinking (`RIDE_SWAMP_WALKER`); a mount whose race answers `query_swamp_walker()` does too. Read by the `swamp` component, which judges a rider by its mount |
| `combat` | `linked` (default) or `none` (`RIDE_COMBAT`): whether the mount takes part in its rider's fights, see Combat |
| `protect` | percent chance (default 0) that the mount takes the blows aimed at its rider (`RIDE_PROTECT`) |
| `wimpy` | percent of its hit points under which a ridden mount flees carrying its rider (default 0, `RIDE_WIMPY`) |
| `fears` | list of names, race ids or properties that make a ridden mount flee, in sight or when they fight it (`RIDE_FEARS`) |
| `learn` | skill id the driver is given at 10% the first time they take the driver's seat without knowing it. It counts as known at 10 for `requires`, so a first ride is never refused for lack of it |

Only the driver's seat asks for `requires` and `learn`; any seat may be taken by whoever fits.

## The row: texts

Besides the spec keys a row holds every text a ride says. `$N` is the rider's name, `$S` the mount's short in lower case, `$T` the direction and `$F` where the rider comes from (these two for the step lines, filled in by `move_living`). A key a kind leaves out is read from the default row.

- `verbs_board`, `verbs_dismount`: the verbs the mount answers to (an NPC through its component, a vehicle in `init`). `prepositions`: little words dropped after the verb (`montar sobre el caballo`).
- `board_me` / `board_room`, `dismount_me` / `dismount_room`, `fall_me` / `fall_room`.
- `leave`, `arrive`: replace the race's move lines while mounted; they start with `@`, the marker `move_living` strips. An exit that has its own message keeps it.
- `listing`: follows the rider's name in a room listing. `look`: the line `look` adds to the rider.
- `protect_room`, `flee_me` / `flee_room`: the mount covers its rider, the mount bolts with its rider.
- `refuse_<reason>`: `attack` (a rider tries to attack its own mount), `dead`, `passed_out`, `already`, `full`, `skill`, `body_size`, `passenger`, `exit`, `exit_size`, `terrain`, `indoors`, `underground`, `water`, `ground`, plus one per exit type a kind denies.

## Handler API (`/lib/handlers/riding.c`)

- `query_spec(mount)`, `query_spec_of(mount, own)` (for a mount still being put together), `query_mount(ob)`.
- `query_riders(mount)` (driver first; not stored: the livings in the mount's place whose `query_mount()` is it), `query_seat_taken`, `query_free_seat`, `query_addressed(mount, str)` (whether the words after a verb name the mount).
- `query_board_refusal(who, mount, seat)` (a text for the rider, or nil; seat -1 is the first free one), `board(who, mount)`, `board_seat(who, mount, seat)`, `dismount(who, fall)`, `unseat_riders(mount [, place])`.
- `query_move_refusal(who, room, dir)`: asked by the exit handler of whoever rides; a passenger cannot move the mount.
- `query_travel_line(who, arriving)`, `query_listing(who)`, `query_look(who)`.
- `carry(who, from)`, `rider_moved`, `mount_moved`: movement, below.
- `query_mates(a, b)`, `query_attack_refusal(who, other)`, `rider_fights`, `rider_attacked`, `mount_attacked`, `query_shares_xp(attacker, attackers)`, `query_feared(mount, who)`, `query_wants_to_flee(mount)`, `query_fear_present(mount)`, `run_away(mount)`: combat, below.

Hooks the handler calls on the mount, all optional (they are plain `call_other`: a mount that does not define one answers nil): `mount_can_board(who, seat)` (a refusal text, or nil), `mount_boarded(who)`, `mount_dismounted(who)`. A hand-written NPC or a vehicle defines them.

## Movement

- The exit handler asks `query_move_refusal` before a mounted rider leaves (`do_exit_command`): seat, exit type, size, what lies beyond.
- `move_living` writes the step lines from `query_travel_line` while the living rides, then calls `carry(rider, from)`: the mount and the passengers are moved after the driver with a plain `move(dest)`, with no lines (the driver's line is the room's). If the mount cannot be moved the driver falls.
- A rider moved by anything else (`goto`, a spell, a push) reaches `event_moved` of `/lib/living/movement.c`, which is not its own walk, so `rider_moved` throws it off (a move to the place it already is in, as the login of a link-dead player does, throws nobody off). A mount moved while it is not following its driver (`event_moved` of the NPC or the vehicle) throws off whoever is on it with `mount_moved`. The two walks that are its own are flagged (`walking` on the rider, the handler's `carrying` list for the mount and the passengers).
- A mount that dies, or is destructed or has its component taken out, unseats its riders (`unseat_riders`: the fall lines, no damage). A rider that dies is dismounted with no lines (`dismount(who, 0, 1)` from `death.c`) and the mount stays where it is.
- Every fall is words only: no damage.
- An NPC mount with somebody on it does not wander (`query_staying_put` of `/lib/npc.c`), and neither mount nor ridden vehicle shows in the room list (`pretty_short` empty): the rider does, with its `listing`.

## Combat

A rider and its own mount (it sits on it, or it is the owner of an owned mount, ridden or not) never attack each other: `kill` refuses with `refuse_attack` and `attack_ob` / `attack_by` of `/lib/living/combat.c` drop the blow (`query_mates`).

With `combat` `linked` (every NPC kind by default; a vehicle is no living and never fights) the mount is one more combatant:

- A rider attacks, or is attacked: the mount takes on the foe (`rider_fights` from `attack_ob`, `rider_attacked` from `attack_by`). In `rider_attacked`, with the chance `protect`, the mount steps in instead: the foe stops fighting the rider and takes on the mount.
- A ridden mount is attacked: every rider takes on the foe (`mount_attacked`).
- A ridden mount runs when it is under its `wimpy` percent of hit points or fights something it `fears`: the `rideable` component votes `FLEE_FORCE` in the flee chain of `/lib/npc.c`, and `run_away` of `/lib/living/movement.c` hands the run to the handler's `run_away(mount)`, which picks a random exit the driver may take and walks the driver through it, so the mount and the passengers come along (lines `flee_me`, `flee_room`). It also bolts, a second after something it fears enters its place or it arrives in a place where it stands (`check_fears` of the component). A passenger-only ride (no driver) or a place with no valid exit keeps it fighting.
- Experience: a mount whose rider or owner is among the killers does not share the xp of a kill and gets none (`query_shares_xp` from `death.c`).

rl-aeternum values (`/games/rl-aeternum/tables/mounts.c`): horse `protect` 25, `wimpy` 35, fears wolf, warg and bear; warg 60 / 10 / none; crocodile 50 / 10 / none; donkey 5 / 50 and pony 10 / 50 (they keep the fears of the horse).

## Skills

`ride`, `drive` and `pilot` are common passive skills (`lib/obj/skills/`), percentage, ids in `<living/skills.h>`, names and help in `lib/obj/skills/.lang.<lang>.h`. Nothing teaches `drive` or `pilot` yet; a mount that wants one in its `requires` and `learn` gets it granted at 10%.

## Owned mounts: one file each

A mount a player buys is theirs, up to `MAX_OWNED_MOUNTS` (10; `query_max_owned_mounts`, a game's handler may change it).

- **On disk**: one file per mount, `<player save folder>/mounts/<id>.o` (`query_mount_file(owner, id)`, the folder is `player_save_dir(owner)`). It holds `mount_template` (bestiary template id) or `mount_file` (cloned path), `mount_owner`, `mount_id`, `mount_location` (where it stood: a location by its `.o`, a room by its path), `mount_name` (its short) and `mount_hp` (0 when unhurt).
- **On the player** (`/lib/living/riding.c`): `mount_ids`, the ids it owns, saved with the player (`query_mount_ids`), and `owned_mounts`, the mounts that are out now, not saved (`query_owned_mounts`).
- **On the mount**: only who owns it and under which id, `owner` (`RIDE_OWNER`) and `mount_id` (`RIDE_ID`) in its spec, so in the attributes of its `rideable` component (a vehicle: its `set_rideable` spec). `query_owner(mount)` and `query_mount_id(mount)` read them.
- **The handler keeps nothing of any player in memory**: it only holds the logic (its `mount_*` variables are used for the instant of a save or restore), so reloading it loses and duplicates no mount.

What happens:

- `add_mount(owner, stock, where)`: makes the mount in `where` (a template through the bestiary, a file by cloning), ties it to the owner, writes its file, adds its id to the player and saves the player. The cap and the price are asked by whoever sells.
- A mount going away (`mount_gone`, from the destruction of the component or vehicle) writes its file where it stands: when its owner quits, or when its place unloads. An NPC lets its components go before they clean up, so the component passes its spec along.
- A mount that dies (`mount_died`) deletes its file and leaves the player's ids; it is not written again.
- **Quit** (`lib/player/quit.c`): `owner_quit` takes the player off what they ride, then every mount of theirs that is out is destructed, saved where it stood.
- **Login** (`lib/player/start.c`): `owner_enter` makes again each saved mount where it stood, or beside the player when that place does not load; its `hp` is restored. An id whose file is gone leaves the list. A mount already out for that player is not made twice.
- **The place does not unload while an owned mount stands in it**: an owned mount (NPC or vehicle) answers `query_unique_object()` with 1, and `clean_up` of a location keeps whatever holds a unique object.
- Only the owner may take the driver's seat of an owned mount (`refuse_not_owner`); anybody may still ride as a passenger.
- An owned mount belongs to its player, not to an area: it is made through the bestiary, outside any census.
- `query_owned(owner)` (in the order bought, each `([ "name", "mount" ])`, the mount being the live one or nil) and `query_owned_count(owner)`.
- A retired player's `player.o` is removed but its folder stays, with its mount files.

## Selling mounts: the `mount-shop` component

A location with `mount-shop` sells what its `stock` lists, a list of `([ "template" : id, "price" : value ])` or `([ "file" : path, "price" : value ])`, the price in the base unit of the money; the label of each is the short of the mount. Verbs (`.lang` files of `lib/location/components/`): `list`, `examine <mount>` (short, first paragraph of the description, price, and whether the player cannot afford it) and `buy <mount>`; a mount is named by what it answers to or by its number in the list. A buy is refused over the cap or without the money; otherwise the player pays, the mount is made in the location and saved as theirs, and the player and the room are told. The component puts up a sign that lists the commands.

Adding it to a location from the mud, with its stock (`components add` takes no attributes), standing in the location:

    exec environment(TP)->add_component("mount-shop", ([ "stock" : ({ ([ "template" : "<template id>", "price" : <value> ]) }) ])); environment(TP)->save_me(); return 1;

`components remove mount-shop` takes it away with its sign. More stock afterwards: `exec environment(TP)->query_component_by_type("mount-shop")->add_stock(([ "template" : "<id>", "price" : <value> ])); return 1;`.
