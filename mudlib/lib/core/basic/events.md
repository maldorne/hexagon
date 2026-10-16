# Events

An event is a message from one object to the objects around it: somebody
spoke, somebody arrived, a fight started, the sky changed. The sender does not
know who is listening; every object that defines the matching `event_<name>`
function hears it.

## The mechanism

`event()` is a simul-efun (`/lib/core/efuns/event.c`):

```c
static void event(mixed obs, string event_name, mixed arg...)
```

It calls `"event_" + event_name` with `(who, arg...)` on each receiver, where
`who` is the object that called `event()` (`this_object()`). The first
argument of every handler is therefore the sender, which a handler usually
ignores or compares with itself.

`obs` selects the receivers:

- **An object** (usually a room or location): the object itself plus its whole
  `all_inventory()`, so the event reaches the room, the players and monsters in
  it, and the items on the floor; what players and monsters carry is not
  visited. If `obs` is not the sender, the sender is
  taken out of the list: whoever fires an event does not hear it. If `obs` is
  the sender itself, the sender is told too.
- **An array of objects** (`users()`, `livings()`, a filtered list): exactly
  those objects. Nothing is removed, the sender included. Destructed entries
  are skipped.
- Anything else: nothing happens.

A receiver without the function is ignored by `call_other`. Handlers return
nothing and nothing is gathered from them.

### Handlers

`/lib/core/basic/events.c` holds the default handler of every common event. It
is inherited by every object through `/lib/core/object.c`, so any object can
receive any of them. Most defaults do nothing, or only print a debug line; an
object overrides the ones it cares about and calls `::event_<name>` when it
wants to keep the default.

`/lib/player/events.c` extends the defaults for players (text on screen for
arrivals, departures and speech, with the player's point of view applied).

### The `avoid` convention

Events that carry text for the screen (`say`, `inform`, `enter`, `exit`,
`login`, `soul`, `death`) end with a varargs `avoid`: an object or an array of
objects that must not be told. Each default handler returns at once when
`this_object()` is `avoid` or is in it. It is how `tell_room(room, msg, avoid)`
leaves out the speaker and the target.

### Forwarding to components

Components are not in the inventory of their entity, so `event()` never
reaches them. The entity forwards what its components need with
`run_on_components(func, args...)` (`/lib/core/ecs/entity.c`), which calls the
function, with the event's own arguments, on every component that defines it:

- `/lib/location.c` forwards `enter` and `exit`, and passes `event_climbed` /
  `event_climb_failed` to its bridge component.
- `/lib/npc.c` forwards `enter`, `exit`, `moved`, `weather`, `fight_in_progress`, `revealed` and `crime`.

## Movement

### enter

```c
void event_enter(object ob, varargs string msg, object from, mixed avoid)
```

`ob` has arrived; `msg` is the arrival text (empty or nil: none), `from` where
it came from, `avoid` who must not be told (the mover itself).

Fired by `move()` in `/lib/core/basic/move.c`, after the object is in its new
environment and its light has been moved: `event(dest, "enter", messin,
previous, this_object())`. Told to the destination and its inventory, never to
the mover.

The default only logs. `/lib/player/events.c` prints `msg` to the player when
`ob` is a living in the same place, with the mover's name as the player sees
it, or a generic arrival text if the place is dark. `/lib/monster.c` reacts to
arrivals (aggression check, revenge on a player who attacked it and left).
`/lib/location.c` forwards it to its components; bridges, coasts, water,
triggers and the blacksmith component use it.
`/lib/npc.c` forwards it to its components too (the thief component
schedules its attempt from it, never acting inside the event).

### exit

```c
void event_exit(object ob, varargs string msg, object dest, mixed avoid)
```

`ob` is about to leave; `msg` is the departure text, `dest` where it goes.
Fired by `move()` before the move, to the place being left and its inventory:
`event(previous, "exit", messout, dest, this_object())`. Same implementers as
`enter` (players print the text; `/lib/monster.c` follows a fleeing enemy;
location components get it through `/lib/location.c`, NPC components through
`/lib/npc.c`).

### moved

```c
void event_moved(object ob, object from, object dest)
```

The object has just moved. Not sent through `event()`: `move()` calls
`this_object()->event_moved(this_object(), previous, dest)` directly after
`enter`, because `event()` would tell the mover's inventory and `enter` and
`exit` leave the mover out. `ob` is always the mover, `from` is nil when it was
nowhere. The default does nothing. `/lib/npc.c` forwards it to its components,
and the schedule component uses it to tell its arrival line once the trip ends.

### login

```c
void event_login(object ob, varargs mixed avoid)
```

A player has appeared in a place after logging in. Fired by
`/lib/player/start.c` on the player's environment (not for a player who is
invisible at level 2): `event(environment(), "login", this_object())`. The
default only logs for interactive receivers; `/lib/room.c` overrides it to move
the player to the room's login room (`set_login_room`), if it has one.

### dest_me

`move.c::dest_me()` fires `event(environment(), "dest_me")` when an object in a
place is destructed. No object in the mudlib defines `event_dest_me`.

## Speech and messages

### say

```c
void event_say(object caller, string msg, varargs mixed avoid)
```

Plain text for the screen. Fired by `tell_room()` and `say()`
(`/lib/core/efuns/output.c`, `output/say.c`) to a room and its inventory; `say()`
also leaves out the player and the caller through `avoid`. `shout()` calls
`event_say` directly on every player who has no earmuffs. The default does
something only for interactive objects: it applies `fix_string()` and hands the
text to `catch_tell`.

### inform

```c
void event_inform(object caller, string msg, string type, varargs mixed avoid)
```

A bracketed notice on the screen of the players who subscribed to its `type`
(`logon`, `logoff`, `boards`, `deaths`, `calls`, `snoop`, ... the
`INFORM_EVENT_*` constants). Always fired at an array: `event(users(), "inform", msg,
type [, avoid])`, or a filtered list of coders. Senders include
`/lib/core/login.c`, `/lib/player/start.c` and `quit.c`, `/lib/player/death.c`,
`/lib/handlers/boards.c`, `/lib/handlers/channels.c`, `/lib/living/force.c`,
`/lib/cmds/coder/snoop.c` and the coder utilities. The default drops the
notice for players who set `NO_INFORM` or did not subscribe to `type`; an
invisible sender is hidden from everyone but coders, and at invisibility level 2
from everyone but administrators.

### soul

```c
void event_soul(object ob, string msg, varargs mixed avoid)
```

The text of a soul (emote). `/lib/handlers/souls.c` tells the target directly,
`target->event_soul(me, msg)`, and then everyone else in the target's room:
`event(environment(target), "soul", msg, told)`. The default prints `msg` to
the sender and behaves like `say` for the rest.

### person_say

```c
void event_person_say(object ob, string start, string msg, string lang)
```

Somebody spoke in the room. Fired by `/lib/living/communicate.c` (`say`) to the
room: `start` is the speaker's name and verb, `msg` the speech, `lang` the
language. The player implementation (`/lib/player/events.c`) garbles the speech
through the languages handler when the listener does not know `lang`, adds it
to the past-conversation buffer and prints it, wrapped to the screen. The
default does nothing.

### person_whisper

```c
void event_person_whisper(object ob, string start, string msg, object * obs,
                          string lang)
```

A whisper in the room. Fired by `/lib/living/communicate.c` to the room;
`obs` are the intended listeners. Only they get the text (garbled when they
lack `lang`); the others see that a whisper took place. The sender is not told.

### person_shout

```c
void event_person_shout(object ob, string start, string msg, string lang)
```

A shout. Fired at `livings()`, so it reaches the whole mud. The default
discards it for the shouter, players with earmuffs and players who block the
shouter, garbles it by language and prints it.

### creator_tell, inter_creator_tell

```c
void event_creator_tell(object ob, string start, string msg)
void event_inter_creator_tell(object ob, string mname, string pname,
                              string msg, object ig, int emote)
```

Hooks for coder chat. The defaults are empty and nothing fires them.

### player_echo, player_echo_to, player_emote_all

```c
void event_player_echo(object ob, string msg)
void event_player_echo_to(object ob, string msg)
void event_player_emote_all(object ob, string msg)
```

Text an administrator or coder sends to the screen of others with `echo`,
`echoto` and `emoteall`. Not broadcast: the commands call the function directly
on each target (`/lib/user/roles/coder_communicate.c`,
`/lib/cmds/admin/emoteall.c`). The default prints `msg` to interactive objects,
with a header naming the sender when the receiver is an administrator.

## Combat

### fight_in_progress

```c
void event_fight_in_progress(object ob, object * fighters)
```

A blow was exchanged. Fired by `attack()` in `/lib/living/combat.c` on the
attacker's room: `event(environment(), "fight_in_progress", ({ attackee,
this_object() }))`. `fighters` are the two combatants.

Monsters that `join_fights` pile in against the aggressor
(`/lib/monster.c`). `/lib/npc.c` does the same and forwards it to its components
(a bodyguard defends whoever it stands by). `/lib/ventures/pub.c` turns it into
a `pub_brawl` event on the neighbouring locations, and the undead base
(`/lib/obj/death/base-undead.c`) reacts to it.

### pub_brawl

Fired by `/lib/ventures/pub.c` at each neighbouring location with the pub as
argument: `event(neighbour, "pub_brawl", this_object())`. Meant for things next
door that want to join a pub fight; no object in the mudlib defines
`event_pub_brawl`.

### death

```c
void event_death(object caller, varargs object killer, object * attackers,
                 mixed avoid)
```

A living died. Fired by `/lib/living/death.c` on the place where it died:
`event(environment(), "death", killed_by, attacker_list, this_object())`;
`caller` is the dead one, `killer` the one who dealt the last blow (may be
nil), `attackers` everyone who was fighting it. The default only logs.
`/lib/monster.c` stops protecting anybody when somebody else dies.

## Crimes

### crime

```c
void event_crime(object who, object criminal, string type, object victim, int points)
```

An offence was detected in a place with law. Fired by `report_crime()` in
`/lib/handlers/crimes.c` on the place: `event(place, "crime", criminal, type,
victim, points)`, so `who` is the crimes handler. `type` is one of the
`CRIME_*` values of `<crimes.h>`, `victim` may be nil and `points` is what the
offence is worth under the local law.

When nobody can name the culprit (no victim who knows, no witness who sees)
`criminal` is nil and `points` is 0: an alert, nothing is recorded. Places
with no law, and offences that are legal there, fire nothing.

`/lib/npc.c` forwards it to its components. The `guard` component reacts: with a
known culprit and points under the law of its own country it attacks the
culprit (see `lib/handlers/crimes.md`).

## Skills

### player_search

```c
int event_player_search(varargs mixed args)
```

The `search` skill (`/lib/obj/skills/search.c` and the postponed `search`
command) tells the room: `event(env, "player_search")`. Hidden players carry a
hide shadow (`/lib/obj/skills/hide_sh.c`) that defines the handler and rolls
the searcher (`this_player()`) against the hider. Nothing else implements it.

### revealed

```c
void event_revealed(object finder)
```

Called directly by the hide shadow on the player it hid when a search finds
them (`player->event_revealed(searcher)`). `/lib/npc.c` forwards it to its
components, so an NPC lying in wait can react.

### climbed, climb_failed

```c
void event_climbed(object climber)
void event_climb_failed(object climber)
```

Called directly by the climb skill (`/lib/obj/skills/climb.c`) on the thing
climbed, which only needs to define them when it wants its own messages: when it
does not, the skill prints a default text. Climbable items (`/lib/obj/climbable.c`)
define `event_climbed`; `/lib/location.c` passes both to its bridge component
(`/lib/room/bridge.c` has the old-style room version).

## Weather

### weather

```c
void event_weather(object who, varargs int flag, int * values)
```

The weather handler (`/lib/handlers/weather.c`, `weather_inform`) tells the
observers it keeps (rooms and NPCs) when the sky changes. `flag` is one of the
constants in `<areas/weather.h>`: `FLAG_DAWN`, `FLAG_NIGHTFALL` (sent to every
observer, without `values`), `FLAG_CLIMATE` (a climate change, sent to the rooms
and players of the affected zone with `values`: rain, wind and temperature
before the change, then after) and `FLAG_CHECK`, which players ignore and
nothing sends yet. Being fired at an object, the
event also reaches its inventory, so players and NPCs inside an outdoor room
hear it.

Players (`/lib/player/weather.c`) print the dawn and nightfall texts and
update their wetness and temperature. `/lib/npc.c` forwards it to its components
(a climate component can react to rain or cold).

## Quests

### completed

```c
void event_completed(object who)
```

Called directly on a quest object by the quests handler
(`/lib/handlers/quests.c`) when `who` hands the quest in, after it is accepted
and before the rewards are paid. The default in `/lib/quest.c` does nothing; a
quest overrides it to take back what it asked for.

## Other

### commands

`/lib/user.c` calls its own `event_commands()` (defined in
`/lib/user/communicate.c`) when it starts, to register the terminal commands
(`rows`, `cols`, `terminal`, `earmuffs`, `block`, `unblock`). It is an
initialization hook, not an event in the sense above.

### retire

`event_retire(object ob)` is defined empty in `/lib/core/game.c`, a hook for
the game object to keep its player records accurate. Nothing fires it.

## Summary

| event | fired by | how | receivers |
|---|---|---|---|
| `enter` | `move()` | `event()` on destination | destination + inventory, not the mover |
| `exit` | `move()` | `event()` on previous place | place left + inventory, not the mover |
| `moved` | `move()` | direct call | the mover only |
| `login` | `player/start.c` | `event()` on environment | room + inventory |
| `dest_me` | `move.c::dest_me` | `event()` on environment | nobody implements it |
| `say` | `tell_room`, `say` | `event()` on room | room + inventory |
| `inform` | many | `event()` on `users()` | subscribed players |
| `soul` | `handlers/souls.c` | direct + `event()` on room | target, then room |
| `person_say` | `living/communicate.c` | `event()` on room | room + inventory |
| `person_whisper` | `living/communicate.c` | `event()` on room | room + inventory |
| `person_shout` | `living/communicate.c` | `event()` on `livings()` | every living |
| `player_echo*`, `player_emote_all` | coder / admin commands | direct | each target |
| `fight_in_progress` | `living/combat.c` | `event()` on room | room + inventory |
| `pub_brawl` | `ventures/pub.c` | `event()` on neighbours | nobody implements it |
| `death` | `living/death.c` | `event()` on room | room + inventory |
| `crime` | `handlers/crimes.c` | `event()` on room | room + inventory |
| `player_search` | search skill | `event()` on room | hide shadows |
| `revealed` | `hide_sh.c` | direct | the hidden one |
| `climbed`, `climb_failed` | climb skill | direct | the thing climbed |
| `weather` | `handlers/weather.c` | `event()` on observers | observer + inventory |
| `completed` | `handlers/quests.c` | direct | the quest |
