# Area population

The people and creatures of a game belong to its areas. A room does not clone
its own NPCs: an area keeps them as data (a count, a census row, a savefile) and
a location brings them into the world when it loads. This file follows that
system from the templates that describe a kind of NPC to the hourly round that
walks citizens between work and home.

The code lives in `/lib/location/area/` (one file per concern, all inherited by
`/lib/location/area.c`), `/lib/handlers/areas.c`, `/lib/handlers/population.c`,
`/lib/handlers/bestiary.c`, `/lib/npc.c` and `/lib/npc/components/`. The shared
constants are in `<areas/area.h>`, `<areas/poi.h>`, `<areas/vacancy.h>` and
`<areas/diplomacy.h>`.

## Three layers: template, post, individual

| layer | what it owns | where it lives |
|---|---|---|
| template (the type) | how the people of a kind look and talk: names, descriptions, chatter, temperament, `sentient`, move zones, components | a JSON file per language beside the game's content, read by `/lib/handlers/bestiary.c` |
| vacancy (the post) | the job: seats, workplace, class, kit, timetable, house | the owning area's `area.o` |
| individual | uuid, generated name, gender, level, race, inventory, purse, workplace, components | its own `npc.o`, plus a row in the area's census |

A template is authored by hand and versioned with the game; nothing in the mud
writes one. Its id is the file's place in the game tree without the
`/games/<game>/` prefix and the extension (`areas/<area>/npcs/<type>`), and the
language and `.json` are added back to find the file, so a type with no template
in the running language does not spawn. The bestiary caches decoded templates
and rereads a file when it changes.

Everything an area stores about a type is keyed by that template id. A template
can say `sentient` (an individual, tracked one by one) or not (a creature,
counted). A `sentient` type that also states `proper_name` or `unique` is one
particular person: it keeps its template's name, race and level instead of being
generated.

## Two halves of the population

**Monsters** (fauna, vermin) are counted, not tracked. The area keeps
`monster_census`, `([ location_file : ([ template_id : count ]) ])`, and nothing
else about them: no uuid, no savefile, no state. Every time a location loads,
`restore_location_monsters()` clones whatever its bucket says is missing, with
gender, level and kit rolled fresh. They vanish when the location unloads. One
that cannot swim is not put in water. A death calls `monster_died()` on the
bucket it was counted in.

**People** are census individuals. The census, `npc_census`, is
`([ uuid : ([ "source": template_id, "current_location": file, "savefile": path,
"vacancy": job, "works_at": file, ... ]) ])`. A row stays lean (who, where, what
post); everything individual lives on the person's `npc.o`, under
`/save/games/<game>/npcs/<letter>/<uuid>/`. `create_npc(source, row)` takes
somebody onto the census whole: it draws the uuid, writes the row, builds the
person once (gender, name, race, class, level, nationality, workplace, kit) and
saves the `npc.o`. Nothing is loaded or placed; the person appears when the
location they stand in loads.

The `sentient` flag in a template decides the half. The population sweep treats
a non-sentient type as fauna; a sentient one is placed by a post, by a house, or
by the sweep as an "unplaced person" (see below).

## The roster and its caps

`npc_caps` is what may live in the area and at most how many:
`([ template_id : ([ "max": n, "resident": 1 ]) ])`. It is not typed in. Room
conversion records, per location, how many of each NPC the old room cloned
(`original_npc_sources`, set with `set_location_original_sources()`), and
`rebuild_npc_caps()` sums those counts into the caps. Reconverting a location
overwrites only its own entry, which keeps the conversion idempotent. The
`resident` flag is the one design-time field that survives a rebuild.

Three kinds of source are kept off the roster on every rebuild: a type a vacancy
draws from, the type a job's template names in `extracted_from`, and anything
that is not a living NPC.

`query_total_live_count(source)` is the census count plus the monster buckets;
it is the number compared against a cap.

## The population sweep

`/lib/handlers/population.c` tops an area up toward its caps without loading a
single location. A cron line per game calls `update_population()`; each call
sweeps one area of that game, taking them in turn, and assigns at most ten
(`ASSIGN_PER_TICK`) NPCs, so a large deficit fills over several ticks.
`populate_area(area, limit)` does the work in two passes:

1. **Fauna.** For each source in `query_monster_sources()` (the roster minus
   every sentient kind), the area's share of the cap is the sum of what its own
   locations declared (`query_original_clone_counts()`), the deficit is that
   share minus the area-wide monster count, and `assign_monster()` bumps the
   bucket of a random declaring location.
2. **Unplaced people.** For each source in `query_unplaced_people_sources()`
   (sentient roster types that no vacancy draws from and that are not
   `resident`: travellers, pilgrims), the deficit is the declared share minus
   the census people of this area's share (`query_npc_share_count()`). Each
   missing one is created with `create_npc()` at a random declaring location,
   with a name, a savefile and the area's path as its share (`CENSUS_SHARE`).
   They count against that area wherever they wander, so somebody walking into a
   sibling area of the community does not make room for another. A row written
   before shares existed takes the share of the area it stands in, once.

A death frees a slot at once; the next sweep refills it, usually elsewhere.

**Fewer people than spots.** In both passes the declared share is cut to the
type's `limit` on the roster, when it has one (`npcs limit <source> <n|none>`,
kept in `npc_caps` beside `resident` and carried over every recompute). The
rooms that clone the type then say only where it may appear: a lone thief
declared in six stalls is born in one of them, and only one is alive at once.
Because the caps are shared by a community (see parent areas) but the share is
read from the area's own provenance, a road keeps topping up its own travellers
and does not scatter them through the town. `npcs populate` runs the same pass
for the area you stand in and brings the new arrivals into locations already
loaded.

## Points of interest

A POI marks a location the world cares about. It is not a job and holds none.
An area keeps `pois`, `([ location_file : ([ "kind", "label", "guard_dir",
"admit" ]) ])`, at most one per location.

| kind | meaning |
|---|---|
| `pub`, `shop` | ventures; attached automatically from the location component at conversion |
| `town_entrance` | a gate; carries `guard_dir` (the exit that leads into town) and `admit` |
| `town_square` | a square; its guards stand by, stopping nobody |
| `crossroads`, `shrine` | places of note; a shrine also names its deity |
| `bodyguard` | guards posted here defend citizens attacked in the room |

`add_poi`, `remove_poi`, `set_poi_label`, `set_poi_guard_dir`, `set_poi_admit`
and `query_poi` are the accessors. A schedule entry that sends somebody `out`
picks a random POI of the community.

## Vacancies

A vacancy is a job a settlement offers: the barman's post, the farmers a field
needs, the watch a gate. One record shape for everything, kept by the
community's root area:

| field | meaning |
|---|---|
| `job` | the job's name and identity; one job per type, however many places |
| `count` | seats |
| `works_at` | the location where it is declared and held |
| `source` | template id its holders are drawn from |
| `sources` | `({ ({ template_id, weight }), ... })`: several types rolled by weight for each new holder; the first is also `source` |
| `fixed` | marks a single post held at one place |
| `spots` | places worked, one holder seated in each in turn; without it every holder stands at `works_at` |
| `class` | the class the job trains its holders in |
| `equipment` | the kit: one list of interchangeable items per slot, each holder rolls one |
| `kits` | `([ template_id : kit ])`: a holder drawn from a listed type gets that kit instead of the job's (`vacancies equip <job> for <type> ...`) |
| `timetable` | the hours the job keeps (below) |
| `home` | a house bound to the post |
| `resident` | holders are housed among the settlement's own |

The record says what the job is, never who does it. Who holds a seat is read
from the census (`query_vacancy_holders()` counts the rows whose `vacancy` is the
job), so a seat cannot go stale: when the holder dies the seat is open by
arithmetic. `open_vacancy()` restating a job keeps every part not restated;
`close_vacancy()` retires its holders (`retire_npc()`): live ones are destroyed,
their census rows dropped and their savefile folders deleted.

A job is named after its type (`barman` from `areas/<area>/npcs/barman`). Two
pubs are one `barman` job with two seats and two spots.

### Weighted types

A job may draw from several types: `vacancies add guard_day 4
<a>*6,<b>*3,<c>`. A type with no weight weighs 1. `_draw_vacancy_source()`
rolls one per hire, so the same post is filled with a mix. Restating the job
with one type drops the others.

### Timetables

A job's `timetable` maps a game hour (0 to 23, a string key in the stored job)
to `([ "goto": "work" | "home" | "out" | <location file>, "leave": <text>,
"arrive": <text> ])`. Both lines are optional souls templates resolved against
the person: `leave` is told to the room they set off from, `arrive` to the one
they reach once any door is closed behind them, so a day of work and home has
four at most. Somebody finishing a walk cut short says no `leave`. Timetables belong to the job:
day and night watches are two jobs. For somebody holding no post the type's own
`timetable` is used, and with no workplace it can only send them `home` or `out`
(a random POI). A type or job that names no hours keeps its people where they
are put.

While the entry in effect is `home`, at home or on the way there, the person
is resting and says none of their chats: those are written for the working day.

### Equipment, spots, homes

The kit is rolled once at creation and carried for life on the `npc.o`, so a
change to a job's `equipment` reaches only people hired afterwards;
`npcs rekit [type]` replaces the kit of existing people. `reseat_vacancy()`
hands each existing holder the spot the list would give today. `home` binds a
house to the post (see housing).

### Staffing

Declaring a job takes nobody on. `staff_vacancies()` walks every job and, for
each empty seat, calls `create_npc()` (through the owner of the location where
the holder will stand, so that area's level band and naming apply), then runs
`assign_homes()`. `staff_vacancies_at(file)` does the same for the jobs held at
one location. A job whose venue reports `query_venue_closed()` (a temple whose
deity the citizenship does not accept) is skipped. It is idempotent: a full job
hires nobody.

Nothing calls either function on its own. There is no command for them; a coder
runs them with `exec` from inside the area. Neither a location load nor a death
hires anybody, so a death frees the seat and it stays empty until somebody
staffs the area again. `fixed` is recorded and shown by the `vacancies` command
but has no effect on hiring.

## Births: what a new person gets

`create_npc()` decides everything once, and the area is the authority:

- **Gender** from the template (fixed, or rolled among the genders it lists).
- **Name** generated from the naming citizenship's `name_style` and the gender,
  for a sentient, non-unique type; stored on the `npc.o` as the given name.
- **Race** drawn from the naming citizenship's race pool, for the same kind of
  type; fauna and uniques keep the race their type gives.
- **Nationality** (`city_ob`): the citizenship at the top of the area's
  diplomacy chain, unless the template's `citizenship` says otherwise (a list
  draws one, an empty one makes them nobody's subjects).
- **Class and job object**: the job's `class` when set; if the game keeps a
  `/games/<game>/obj/jobs/<job>.c`, the holder is enrolled in it.
- **Level**: a template `level` wins; otherwise the area's level plus the
  template's `level_area_modifier`, swung by up to the area's spread
  (`random(spread + 1)`, either sign), never below 1.
- **Stats**: rolled in the area's `areas stats` range (the template's own
  `random_stats` or pinned `stats` win), then the template's `stat_modifiers`.
- **Kit** from the job, or from the type for somebody with no post; starting
  coin from the template's `money`.

A level changed on the area reaches existing people only through
`relevel_census()` (`areas relevel`).

## Materialize and drain

When an area loads a location (`load_location()`), `populate_location(loc)`
brings in exactly the people the census says stand there, with `place_npc()`:
read the `npc.o`, reapply the template (presentation is not saved), wear the
kit, move in, let the components see the world, and attach the `schedule`
component when the post (or the type) has hours. It then asks which timetable
entry governs the current hour and walks the person there, so somebody who was
mid-trip when the location unloaded resumes. Monsters of the location's bucket
are cloned too, and so are roamers indexed there from other areas. Nobody is
placed twice.

When a location is destroyed (`dest_me`), `drain_location()` records where each
of the area's people actually is now, saves their `npc.o`, and keeps the census
row, so they come back next load. A person who dies runs `npc_died()` from
`dest_me`: the row goes, the savefile folder is deleted, the hourly index and
the cross-area index forget them, the house books and the family register hear of
it, and the seat is open.

## Housing

The community's root area keeps the books (`housing.c`); the only record of who
lives where is `house_records`, `([ location_file : ([ "residents": ({ uuid }),
"owner": surname, "kept": 1, "communal": 1 ]) ])`, plus the list of free `plots`.
Housing somebody or emptying a house loads neither the person nor the place; only
raising a house loads its location.

- **Plots.** An empty buildable lot carved next to an existing location, with the
  `plot` component. `add_plot`, `remove_plot`, `query_plots`.
- **Homes.** A house is a location with the `home` component. `build_house_on_plot()`
  or `build_house_at()` swaps `plot` for `home` and turns the open passages into
  doors that start closed, and the house takes the `city` component of the
  locations it opens onto (`take_setting`). `demote_house()` is the inverse,
  drops it again (and a `forest` given by hand) and leaves residents homeless.
- **Built locations.** `furnish_house(file)` makes a location that already
  exists (a barracks, a room over a shop) a kept house as it stands, keeping its
  exits, short and long.
- **Shared houses** (a tenement, a courtyard house): `houses shared <n>`
  gives the house room for `n` households, each its own record on the books
  (the location file, then `<file>#2` ... `<file>#n`) with its own residents and
  family; `query_house_file` maps a household back to its location,
  `query_house_units` lists them. To `assign_homes()` each household is one
  ordinary house, so a few buildings hold a street's families. A shared house
  is never kept nor communal; the door opens for every family in it.
- **Kept houses** (raised by hand, or a job's `home`) are never handed to
  whoever is homeless. A **communal** house (a barracks) is shared by whoever
  holds its job; nobody living there founds a family or marries anyone in.
- **Who is entitled.** A design-time fact: a roster source flagged `resident`
  (`npcs resident`), or a job flagged `resident` and with no `home`. A job
  with a `home` houses its holders there, whoever they are; the house follows the
  post, and when the holder dies whoever lived with them is housed again.
- **`assign_homes()`** (`houses assign`) houses every homeless resident of the
  whole community, in turn: join a spouse who has a house; otherwise, if never
  married, marry a never-married person of the other sex living alone in a
  non-communal house; otherwise pair with another homeless resident and found a
  household; otherwise get a house alone. A household takes an empty ordinary
  house before a free plot is built on. People already housed with no family get
  one first. Families are founded only in a `settled` area, with surnames and
  descent rule from the citizenship; shortfalls (no free plot) are written to
  the area's event log. It runs after `staff_vacancies()` and on its own.

## The hourly schedule

The round has three links, ordered in the crontab:

1. cron advances the game clock (`weather advance`) on each game's weather
   handler;
2. cron then calls `update_areas()` on the game's areas handler, which reads that
   hour from the same game's weather handler;
3. for each area with somebody due, it queues `({ area, uuid, hour })` triples.

Each area keeps `schedule_index`, `([ hour : ({ uuid }) ])`, filled by
`index_schedule_hours()` when a person with a schedule is placed and cleaned when
they die. A game's own areas handler (`/games/<game>/handlers/areas.c`, which
inherits the shared one) also keeps `schedule_areas`, `([ hour : ({ area path })
])`, so a round restores only the areas that have work; a game with no handler
of its own is visited whole by the shared one.

The queue is released a few at a time (three per tick, then a one or two second
`call_out`), so departures and the loads they trigger stagger instead of landing
on one beat. For each item `wake_and_schedule(uuid, hour)` uses the person if
they are already in the world (a roamer may be in another area); otherwise it
loads the location the census puts them in, which materializes them; then it
calls `do_schedule(hour)`, which the `schedule` component answers by announcing
`msg` and walking the person to the entry's destination (`work`, `home`, `out`
or a location file) with paced, interruptible travel. An entry that cannot be
woken (no row, no position, a position that no longer loads) is dropped from the
index then and there. The entry in effect at an hour is the latest one at or
before it, wrapping past midnight, and while it says `home` and the person is
home they do not wander out.

## Cross-area roaming

A person's roster area (`npc_area_path`) never changes: its caps count them, its
scheduler wakes them. A person may work in one area and sleep in another, as long
as a route on the coordinate map connects them. When a roamer's location in a
foreign area unloads, `drain_location()` tells the roster area the new position
(`set_census_location()`) and indexes it in the shared areas handler
(`set_foreign_position()`); that foreign location places it again on load
(`foreign_positions_at()`). The index is a runtime hint: `place_npc` checks that
the census still puts the person there.

## Parent areas and the community

`room2loc` makes one area per directory, which is right for places and wrong for
people. An area can name the area it is part of (`areas parent`); the link
is set by hand, never derived from the tree. `query_root_area()` walks it (at
most `AREA_MAX_ANCESTRY` steps) and every population accessor of a delegating
area answers from the root: caps and the conversion provenance they are summed
from, census, vacancies, plots, houses, families. Writes are forwarded and saved
on the root. The area keeps its own locations, exits, POIs, monster buckets and
schedule index.

Setting or clearing a parent moves those books between the two roots
(`lib/location/area/community.c`), so nobody is lost or duplicated:

- `areas parent <path> check` reports what it would do; without `check` it
  is done, or refused while any conflict remains, each one named with the
  command that settles it.
- **Joining** a community moves the area's census, jobs, houses, plots and
  provenance onto the new root. Two jobs with the same name conflict:
  `vacancies rename <name> <new name>` on either side.
- **Leaving** splits the books: a job goes where it is held; a person follows
  their job, else their house, else where they stand; a house goes where it
  stands, or with the area it is booked with (`houses books <area> [house]`);
  plots and provenance go where they stand. A holder whose house would end up
  on the other side of their job conflicts: book the house with their side
  (only when nobody else in it works on the side it stands on), or take them
  out of it with `npcs rehouse <uuid>`, and the housing pass run on each side
  after the split houses them again. A job whose own house would end up on the
  other side conflicts the same way.
- People standing on the other side of their community are re-indexed as
  roamers of it, and their hours move to the area that places them.
- **Warnings**, which do not stop it: a type that ends up above its cap (the
  sweep and the staffing only add, so the excess fades as they die), and, on
  leaving, a citizenship or a level band the area took from its parent.
- Naming the parent an area already has adopts whatever its own books still
  hold (books written before the link existed) into its community.

The level band stays per area: `query_area_level()` and the spread come from the
nearest area up the chain that sets one, else 1 and 0, and the stat range is the
area's own.

## Citizenship and guards

An area names the citizenship that holds it (`areas diplomacy`); without one
it takes its parent's, and `none` marks it stateless, inheriting nothing. The
diplomacy graph (`/lib/handlers/diplomacy.c`, one per game, built by hand) owns
parents and the ally and enemy pairs.

- **Nationality** of a new person is the top of the chain of the area's
  citizenship.
- **Naming and race pool** come from the area's own citizenship, or from the
  nearest ancestor that has one when the area is stateless, so a road between
  two towns names its travellers like the region without making them subjects.
- **Descent and surnames** for families come from the citizenship too.

A watch is an ordinary vacancy staffed from a template that carries the `guard`
component. The component asks the exit handler to consult it at one exit: its own
`direction` attribute, or the `guard_dir` of the `town_entrance` POI where its
holder works. It stops movers whose citizenship is at war with its own (enmity
cascades through parents), can add guild and class-level rules and a message,
and at a `town_entrance` with `admit` set to `citizens` it lets through only its
own citizenship and those under it. At a `town_square` it has no direction and
stops nobody. At a `bodyguard` POI, standing at its post, it attacks whoever
attacks a citizen of its citizenship in the room, unless the attacker is an NPC
citizen itself. A guard is a `GENERIC_NPC` plus this component; the diplomacy
graph's `security` count and `guard` source are stored but nothing reads them to
place a guard. The barracks is the job's `home`, and shifts are two jobs.

## Reading the population

| command | shows |
|---|---|
| `npcs` | what the area holds in one screen |
| `npcs list [type]` | the census, or one kind |
| `npcs roster` | the types counted statistically and their caps |
| `npcs live` | only the people materialized now |
| `npcs populate` | run the sweep for this area now |
| `npcs rekit [type]` | replace the kit with what the template or job gives now |
| `npcs orphans [apply]` | people nothing asks for any more |
| `npcs verify [apply]` | audit the game's NPC savefiles against the census |
| `npcs retemplate [apply]` | repair stored template ids that name no template |
| `npcs template check [type]` | what is wrong with the templates the area uses |
| `npcs rehouse <uuid>` | take somebody out of their house, to be housed again |
| `vacancies [<job>\|free\|holders\|types]` | the jobs, their holders and whether each type can staff them |
| `pois [<kind>\|here]` | the places the area marks and what is worked at each |
| `houses [plots\|free\|audit]` | houses and who lives in them |

All report on the area you stand in.

## Setting up an area

Standing in the right location each time. The templates
(`areas/<area>/npcs/<type>.<lang>.json`) must exist first.

1. Convert the rooms (`room2loc`), which seeds the caps from the rooms'
   NPC counts; `npcs roster` shows them.
2. Set the place: `areas level <n> [<spread>]`, `areas stats <low>
   <high>`, `areas diplomacy <citizenship|none|inherit>`, and
   `areas parent <area path>` for a town's fields and roads.
3. Mark points of interest: `pois add <kind> [label]`; on an entrance
   `pois guard_dir <dir>` and `pois admit <citizens|all>`.
4. Declare the jobs, standing where each is worked: `vacancies add <name>
   <count> <template>[*<weight>],... [fixed]`, then `vacancies equip`,
   `timetable <name> <hour> <work|home|out|clear>`, `timetable <name> <hour>
   <leave|arrive> [message]`, `class`, `spot`
   (once per extra place), and `home <name>` or `resident <name>`.
5. Carve plots (`houses plot <dir>`) and raise special houses by hand
   (`houses make`, `houses short|long`, `houses communal`,
   `houses shared <n>`).
6. Flag the sentient roster types that live in ordinary houses: `npcs
   resident <template>`.
7. Mark the area `areas state settled` when the map is final (families are
   founded only then).
8. Staff the jobs (`staff_vacancies()` through `exec`), then house everybody with
   `houses assign`. Stand in a loaded location and run `populate_location` on its
   area to bring newly created people in at once.
9. Check with `vacancies`, `pois`, `houses`, `npcs` and `npcs template check`.
