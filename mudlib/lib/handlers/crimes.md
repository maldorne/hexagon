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
`CRIME_TRESPASS`, `CRIME_GUARD_ASSAULT`. `report_crime` is the general entry;
assault, murder and trespass reach it as described in "Offences" below. In
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

Stored per citizenship in the diplomacy graph, next to its parent and deities:
`law` (offence -> points), `lethal` (1 if its guards kill) and `prison`
(the list of cells prisoners are taken to; a save that holds a single location
file is read as a list of one). A key a citizenship does not set is
inherited from its parent chain; if nobody up the chain sets it, the crimes
handler falls back to `CRIME_DEFAULT_LAW`, `CRIME_DEFAULT_LETHAL` and no
prison. Edit it with the admin `crimes law` and `crimes set` commands:

    crimes set <cit> prison <file> [<file> ...]   replace the list of cells
    crimes set <cit> prison add|remove <file>     add a cell, or take one out
    crimes set <cit> prison none                  drop them, so they are inherited

Handler API: `query_law(cit)` (the whole code in force), `query_law_points(cit,
offence)`, `query_lethal(cit)`, `query_prisons(cit)`. In the diplomacy handler,
`query_prisons(cit)` (inherited), `query_own_prisons(cit)` and
`set_prisons(cit, files)`.

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
`query_records_npcs`, `query_witness_min_light`, `query_fight_window`,
`query_fight_memory`, `query_death_keep_percent`, `query_pursuit_give_up`
(seconds a responder pursues), `query_responders` (guards sent),
`query_fine_per_point`, `query_jail_seconds_per_point`, and the ones of
"Justice" below.

## The guards' response

Where the place has law, the `guard` component (`lib/npc/components/guard.c`)
reacts in three ways. Every reaction is an attack; whether the fight ends in an
arrest or in a death is decided by `lethal` (see "Justice").

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
- The guards are picked with `pick_nearest_npcs(place, uuids, wanted)` of the
  community (`lib/location/area/census.c`): free ones only (alive, standing
  somewhere, not fighting, not already responding), the nearest to the crime by
  coordinates first, loaded or not. A guard in an unloaded location is brought
  in by loading it.

Each chosen guard gets the `responder` component
(`lib/npc/components/responder.c`) and `respond(criminal, place, seconds)` with
`query_pursuit_give_up()` seconds. Every two seconds it looks at the criminal: in
the same place, it attacks if the criminal is not hidden; elsewhere in the
community, it walks (`travel_to`) to where they are; elsewhere out of the
community, or with no route, it gives up. When the criminal is dead or gone,
the time is up, or the fight it had with them was stopped (`peace`, an arrest),
it stands down: back to its schedule (`resume_schedule`), or to
the place it was sent from, and the component removes itself. A responder is
never saved: one found in a restored npc.o removes itself.

## Offences

- **Theft**: reported by the steal skill.
- **Assault**: `report_attack(aggressor, victim)`, called by `attack_ob`
  (`lib/living/combat.c`) the first time a living starts attacking another (not
  every round, and not when the victim already attacks the aggressor). Called
  with a short `catch`, so a failure never breaks combat.
- **Guard assault**: the same, when the victim has the `guard` component; it
  replaces assault.
- **Murder**: `report_death(killer, victim)`, called by `do_death`
  (`lib/living/death.c`) before the victim leaves the place; the place is where
  it died. The witness rule is the usual one.
- **Trespass**: `lib/room/handlers/exit_handler.c`, when the guard of an exit
  refuses a mover who then slips past by luck ("te cuelas"); the guard is the
  victim. Only locations have law, so a static room reports nothing.

Who is **protected**: any player, and any NPC with a citizenship that is not an
enemy (diplomacy `is_enemy`) of the place's jurisdiction. Fauna and monsters
without a citizenship are not. Never protected: a player who is `wanted` or
`outlaw` there, nor anyone the guards are already attacking (a criminal being
pursued).

**Self-defence**, none of these is a crime:

- The victim was already attacking the aggressor (the aggressor is in the
  victim's attacker list).
- The aggressor is a guard or a responder (it enforces the law).
- The victim is not protected, as above.
- For murder, the victim started the fight against the killer. The handler
  remembers who started each fight (`fights`, not saved) for
  `query_fight_memory()` seconds; an unknown start counts as murder.

The same aggressor against the same victim is reported once per
`query_fight_window()` seconds. Whoever the aggressor is (player or NPC) the
event fires, the present guards attack it and the dispatch sends help; NPC
records are not stored unless `query_records_npcs()`.

## Justice: beating, arrest, fines

What follows a fight with the law depends on the guard's citizenship
(`query_lethal`, inherited up the parent chain; `CRIME_DEFAULT_LETHAL` is 1: a citizenship has to choose non-lethal guards).

### Lethal citizenships

Their guards kill as any other monster. When a guard (or a responder) kills a
player, `report_death` is never murder and keeps only
`query_death_keep_percent()` of the victim's points in the guard's country
(`reduce`). Any death also ends a sentence being served.

### Non-lethal citizenships: the beating

`adjust_hp` (`lib/living/health.c`) asks the crimes handler, only when a blow
leaves a living at 0 hp or less and its dealer is an NPC, `spare_blow(guard,
victim)`. It answers 1, and the living is left at 1 hp, when all of these hold:

- the dealer is a guard or a responder (`guard`/`responder` component) whose
  citizenship is not lethal;
- the victim is a player, or an NPC with a citizenship that is not an enemy
  (diplomacy `is_enemy`) of the guard's. Fauna, monsters and the guard's
  enemies in war die as always.

Then every guard fighting the victim stops, on both sides, the victim is passed
out for `query_passed_out_seconds()` (`PASSED_OUT_PROP`) and a second later
`arrest` runs. Nobody else's blows change: any other dealer, safe combat and
non-guards go through the usual death path.

### Arrest

`arrest` counts the victim's points in the guard's country (an NPC with no record
counts `query_npc_arrest_points()`). With no points the guards leave the victim
lying. Otherwise:

- **The citizenship has a prison** (`query_prisons`, inherited): the victim is
  moved to one of its cells, the least occupied (`query_cells`; `move_living("X",
  cell)`; player or NPC), and the sentence remembers which. A cell that does not load counts
  as no prison. The sentence is
  `points * query_jail_seconds_per_point()` seconds, at most
  `query_jail_max_seconds()`, in real time. It is kept in the handler: players'
  by name and saved in the crimes file, NPCs' in memory only.
- **No prison**: the guards take the fine on the spot (`query_fine_of`) if the
  victim is a player who carries it, and the points are wiped. If not, they leave
  the victim lying after the beating; the points stay.

A prisoner is never attacked by guards or responders while serving (`guard`
checks `query_jail_left`).

### Serving the sentence: the jail cell

The cell is `lib/room/justice/jail-cell.c` for a room (inherit it), or the `jail`
component (`lib/location/components/jail.c`) for a location; both are thin
wrappers over `lib/room/justice/jail-actions.c`, which holds the commands and
checks, so they behave the same. Every cell where prisoners land (each file in
the citizenship's `prison`) carries it. It refuses any typed exit (`query_exit_refusal`: the
room hook of `lib/room/exits.c`, the location's own) to a living whose sentence
has not ended and says how long is left. There is no heart beat:

- the clock is checked lazily by `query_jail_left(who)` -- on every exit attempt
  and when a player logs in (`watch_login`, from `start_player`);
- a sentence that has run out is ended there by `release`: the points drop to
  `query_release_keep_percent()` of what they were and the prisoner is told;
- a player who logs in while still serving anywhere but in their cell is put
  back in it (the cell the sentence remembers).

Only typed exits are blocked: teleports and forced moves are not.

The cell adds `sobornar` / `bribe`: the price is `query_bribe_cost()`
(`query_bribe_cost_per_point()` for each point owed, paid win or lose); with
`query_bribe_chance()` percent the prisoner is released as if the time were
served, otherwise `query_bribe_fail_points()` points and the time they are worth
(within the maximum) are added.

### Paying: the fines office

The office is `lib/room/justice/fines-office.c` for a room (inherit it), or the
`fines` component (`lib/location/components/fines.c`) for a location; both are
thin wrappers over `lib/room/justice/fines-actions.c`. The law is the place's
jurisdiction; a room is not a location and has none, so a room office names its
citizenship with `set_fines_law("<citizenship>")` (a location takes its area's,
unless the converted room named one). Commands (Spanish and English):

- `perdon` / `owe`: points and fine. The fine is `points *
  query_fine_per_point()`, plus `query_fine_level_percent()` per level of the one
  who owes.
- `pagar` / `pay`: pays all of it (`pay_fine`): the points of that country are
  wiped (logged in the `crimes` log) and the sentence for it ends.
- `fianza <player>` / `bail`: the same for a prisoner who is online, paid by
  the one who types it (`bail`); the prisoner is freed at once.

The office puts up a sign with these instructions (help document
`help/world/fines`, read on every load).

**Place the office outside the city walls.** The guards bar the wanted from the
gates (see "At a door"), so somebody wanted cannot walk in to pay; the office
stands beyond the walls, in an area whose law is the city's, or names it with
`set_fines_law`.

Conversion (`room2loc`) gives a room that inherits `fines-office.c` the `fines`
component (keeping the law it named) and one that inherits `jail-cell.c` the
`jail` component.

## Capture: a guard takes a beaten target to prison

A quest can ask the player to beat an NPC and have it jailed: an `OBJECTIVE_CAPTURE`
on the NPC's template id, with `set_capture_citizenship("<citizenship>")` naming whose
guards take it. The objective advances when the NPC is jailed.

`adjust_hp` (`lib/living/health.c`), when a player wins a non-lethal fight against an
NPC, calls out `report_beaten`, which calls `capture(winner, creature)` of the
crimes handler. In order, it stops quietly unless the quests handler gives a capture
citizenship (`query_capture_citizenship`: the winner has such an objective on that
NPC, not met yet) and no arrest is already under way for it. Then:

1. **Law and cells.** The law of the place (`query_jurisdiction`) must be the capture
   citizenship or one under it (the same root of the diplomacy graph). The cells are
   those of the place's law, or, when it has none, those of the capture citizenship
   (`query_cells`: the cells of `query_prisons` that load, the least occupied first,
   equal ones in random order). Without such a law or a cell, the player is told that
   nobody will take the NPC and that the victory does not count.
2. **A free guard.** `pick_guards` over the census of the place's community, with
   `query_arrest_guard_range()` (`CRIME_ARREST_GUARD_RANGE`, 30): a person whose
   template carries the `guard` component, not posted on a door (`direction` or a
   `town_entrance` `guard_dir`), alive, not fighting, not responding or escorting. The
   range is the distance of `pick_nearest_npcs`: the larger of the differences in x
   and y between the guard's place and the NPC's, plus the difference in z. With none,
   the player is told that no free guard is near enough, nothing counts and the NPC
   comes round.
3. **The escort.** The chosen guard gets the `escort` component
   (`lib/npc/components/escort.c`) and `escort(criminal, winner, citizenship,
   seconds)` with `query_arrest_seconds()` (`CRIME_ARREST_SECONDS`, 900). The NPC is
   held unconscious: `hold_captive` renews its `PASSED_OUT_PROP` (`CRIME_CAPTIVE_HOLD`
   heart beats) on every look of the escort, so it does not wake and fight while the
   guard walks. A `capture_expired` call out at the end of the time lets it go if the
   guard died on the way.
4. **Custody.** Every two seconds the escort walks to where the NPC lies (`travel_to`).
   When it stands there it announces the arrest and calls `take_custody`, and the player
   who beat the NPC is told (if they still play). A broken escort (the NPC dies or is
   gone, no route, the time is up, the guard dies) counts nothing: `release_captive`
   lets the NPC wake and tells the player the arrest failed.
5. **To the cell.** The escort picks the first cell of `query_cells` it has a route to
   and walks there; on every look the NPC is moved (`move_living`) to where the guard
   stands, with a line in each room. When both stand in the cell, `deliver_prisoner`
   jails the NPC (`imprison`, shared with `arrest`: `query_npc_arrest_points()` worth
   of sentence), the quests handler's `captured` advances the winner's objective (if
   they still play) and the guard goes back to its schedule (`resume_schedule`).
6. **The end.** When the sentence is up (`end_sentence`, a call out), a person of the
   census is retired from it with `retire_npc`, so the population brings somebody
   else; any other NPC is removed. If it dies in the cell the sentence ends as any
   death does.

The cells must be locations (the escort walks to them with the pathfinder). Tunables:
`query_arrest_guard_range`, `query_arrest_seconds`.

### Handler API

`capture(winner, creature)`, `query_cells(cit)`, `hold_captive(creature)`,
`take_custody(guard, creature)`, `deliver_prisoner(guard, creature, prison)`,
`release_captive(creature)`, `spare_blow(guard, victim)`, `query_fine_of(who, cit)`, `pay_fine(payer, who,
cit)`, `bail(payer, prisoner, cit)`, `bribe(who)`, `query_bribe_cost(who)`,
`query_jail_left(who)`, `query_jail_prison(who)`, `release(who)`,
`watch_login(who)`. Tunables, in `include/crimes.h` and overridable per game:
`query_passed_out_seconds`, `query_jail_max_seconds`,
`query_release_keep_percent`, `query_npc_arrest_points`,
`query_fine_level_percent`, `query_bribe_cost_per_point`, `query_bribe_chance`,
`query_bribe_fail_points`, plus the earlier `query_fine_per_point`,
`query_jail_seconds_per_point`, `query_death_keep_percent`.
