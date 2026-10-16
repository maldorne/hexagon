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
phases, `query_death_keep_percent`, `query_pursuit_give_up`,
`query_responders`, `query_fine_per_point`, `query_jail_seconds_per_point`.

## Later phases

Nothing reacts to `event_crime` yet. Later: guard components answering it
(responders, pursuit, giving up), arrest, fines and jail using `lethal` and
`prison`, `reduce` when a guard kills a player, and reporting the other
offences (assault, murder, trespass, guard assault) from combat and the
guarded exits.
