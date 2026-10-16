# Crimes

Law and criminal records. A game's offences are detected by the systems that
know about them (the steal skill today) and reported to the crimes handler,
which applies the law of the place, records the culprit and tells the room.

Files: `lib/handlers/crimes.c` (the handler), `include/crimes.h` (every default
number), `games/<game>/handlers/crimes.c` (a game's own copy: it inherits the
shared one and sets its save file), the law stored per citizenship in
`lib/handlers/diplomacy.c`, the commands `record` (player,
`lib/cmds/player/record.c`) and `crimes` (admin, `lib/cmds/admin/crimes.c`).
Reach the handler with `handler("crimes", ob)`, never by path.

## Reporting a crime

```c
int report_crime(object criminal, string type, object victim, object place,
                 varargs int known)
```

`type` is one of `CRIME_THEFT`, `CRIME_ASSAULT`, `CRIME_MURDER`,
`CRIME_TRESPASS`, `CRIME_GUARD_ASSAULT`. Only theft is reported so far. In
order:

1. **Jurisdiction.** `query_jurisdiction(place)` is the effective citizenship of
   the place's area (`query_effective_citizenship`). A place with the `lawless`
   property (`LAWLESS_PROP`), a room that is not a location, a stateless area or
   an area with no citizenship has no law: nothing happens, nothing fires.
2. **Law.** The points the offence costs under that citizenship. 0 means it is
   legal there: nothing happens.
3. **Who knows.** The crime is known if the caller passes `known` (the victim
   knows who did it: the steal skill's caught and noticed outcomes) or a living
   with a citizenship in the place can see the culprit: not dead, asleep,
   paralysed or blind, the culprit not hidden, and enough light (a player's race
   judges it with `check_dark`, an NPC needs `CRIME_WITNESS_MIN_LIGHT`). The
   victim does not count as a witness.
4. **Unknown culprit.** An alert only: `event_crime` fires on the place with a
   nil culprit and 0 points. Nothing is recorded.
5. **Known culprit.** `event_crime(who, criminal, type, victim, points)` fires on
   the place (see `lib/core/basic/events.md`), the record is updated and the
   culprit, and the victim when a player, get a short message
   (`_LANG_CRIMES_SEEN_*`). The points added are returned.

NPC criminals fire the event but are recorded only if `query_records_npcs()` says
so (`CRIME_RECORD_NPCS`, off by default).

## The law

Stored per citizenship in the diplomacy graph, next to its parent, security and
guard: `law` (offence -> points), `lethal` (1 if its guards kill) and `prison`
(the location prisoners are taken to). A key a citizenship does not set is
inherited from its parent chain; if nobody up the chain sets it, the crimes
handler falls back to `CRIME_DEFAULT_LAW`, `CRIME_DEFAULT_LETHAL` and no
prison. Edit it with the admin `crimes law` and `crimes set` commands.

Handler API: `query_law(cit)` (the whole code in force), `query_law_points(cit,
offence)`, `query_lethal(cit)`, `query_prison(cit)`.

## Records

Kept per player name and per **root** citizenship (the country at the top of the
parent chain), so an offence in a town counts in its whole country. Each entry
holds the points, first and last offence time and the recent offences, newest
last: `({ type, time, victim name, place short })`, capped at
`query_recent_max()`. Persisted in `/save/games/<game>/crimes.o`.

Points decay in real time, lazily: `query_decay_points()` points are lost every
`query_decay_period()` seconds, applied from a stored timestamp whenever the
record is read or written. There is no heart beat.

Status from the points: `clean`, `suspect`, `wanted`, `outlaw`, with the
thresholds of `query_threshold(status)`. `query_status(who, cit)` and
`query_status_of(name, cit)` also answer `outlaw` when the player is an outlaw
in a citizenship allied (diplomacy `is_ally`) with this one.

API: `query_points(name, cit)`, `query_status(who, cit)`, `query_status_of(name,
cit)`, `query_record(name)`, `pardon(name, cit)`, `reduce(name, cit, percent)`
(keeps that percentage of the points).

## Tuning

Every default number is in `include/crimes.h` and reaches the handler through a
`query_*` function; a game's handler overrides the function to change it:
`query_threshold`, `query_default_law`, `query_default_lethal`,
`query_decay_points`, `query_decay_period`, `query_recent_max`,
`query_records_npcs`, `query_witness_min_light`, and, for the guards of later
phases, `query_death_keep_percent`, `query_pursuit_give_up` (seconds a responder pursues),
`query_responders` (guards sent), `query_fine_per_point`, `query_jail_seconds_per_point`.

## The guards' response

Where the place has law, the `guard` component (`lib/npc/components/guard.c`)
reacts in three ways. For now every reaction is an attack; whether a guard
arrests or kills (`lethal`) is a later phase.

- **On sight.** A guard attacks whoever `query_status` says is `wanted` or
  `outlaw` in the guard's citizenship: when that person arrives
  (`event_enter`), when the guard itself arrives or is placed
  (`event_moved`, `placed`: it looks over everybody in the place). A hidden
  culprit is left alone. Nothing is done inside the event: the attack is queued
  a second later and rechecked.
- **At a door.** A guard that watches an exit (`check`) refuses it to the wanted,
  with a message, before any other rule.
- **A crime in front of it.** On `event_crime` with a known culprit and points,
  a guard of the country whose law was broken attacks the culprit, wanted yet or
  not.

### Dispatch

After a crime is recorded, `report_crime` queues `dispatch_guards(criminal,
place)` (a call out, so locations are never loaded inside the reporting skill).
It does nothing if a guard is already in the place, `query_responders()` is 0 or
the place has no community. Otherwise it reads the census of the place's
community (root area) and takes `query_responders()` guards:

- A census row is a guard when its template carries the `guard` component.
  Guards posted on a door are never sent: the component has a `direction`, or the
  row's work is a `town_entrance` POI with a `guard_dir`. Square guards are.
- Loaded guards come first, nearest to the crime by coordinates
  (`PATHFIND_HANDLER->query_min_distance`; one that cannot be compared is last).
  Any that is dead, fighting or already responding is skipped.
- If more are needed, the guards the census holds in unloaded locations: those
  in the crime place's own directory first, then the rest. Their location is
  loaded, which materializes them. Their distance is not measured.

Each chosen guard gets the `responder` component
(`lib/npc/components/responder.c`) and `respond(criminal, place, seconds)` with
`query_pursuit_give_up()` seconds. Every two seconds it looks at the criminal: in
the same place, it attacks if the criminal is not hidden; elsewhere in the
community, it walks (`travel_to`) to where they are; elsewhere out of the
community, or with no route, it gives up. When the criminal is dead or gone or
the time is up, it stands down: back to its schedule (`resume_schedule`), or to
the place it was sent from, and the component removes itself. A responder is
never saved: one found in a restored npc.o removes itself.

## Later phases

Arrest, fines and jail using `lethal` and `prison`, `reduce` when a guard kills a
player, and reporting the other offences (assault, murder, trespass, guard
assault) from combat and the guarded exits.
