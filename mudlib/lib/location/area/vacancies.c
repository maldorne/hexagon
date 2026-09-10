// The vacancies of a settlement: the jobs it offers and expects somebody to
// hold.
//
// One record for every kind of post there is. The barman's place behind his
// bar, the four farmers its fields need, the guard at the gate: they differed
// as three separate systems only in who declared them and where the holder
// stands, never in what they are. A vacancy says what the job is, how many hold
// it and where; it says nothing about who does it.
//
// Who holds a seat is read from the census, which is the only record of a
// person. A vacancy therefore cannot go stale: kill the holder and the seat is
// open again by arithmetic, with nothing to clear.
//
// A seat emptied by death is not refilled on the spot. The person is gone; the
// post is taken up later by somebody else -- at once for a post anchored to a
// point of interest, which a settlement notices immediately, and with the next
// settlement pass for the rest.

#include <room/location.h>
#include <areas/area.h>
#include <areas/poi.h>
#include <areas/vacancy.h>
#include <living/persisted.h>
#include <basic/gender.h>
#include <namegen.h>

// Every job this place offers, one record each. See <areas/vacancy.h> for the
// shape.
mapping * vacancies;

private string assign_npc_to_vacancy(mapping vacancy, string where);

void create()
{
  vacancies = ({ });
}

// The vacancies this area works from -- its community's, when it delegates. A
// job belongs to the people who hold it, not to the field they hold it in.
mapping * query_vacancies()
{
  object owner;

  owner = (object)this_object()->query_root_area();
  if (owner != this_object())
    return (mapping *)owner->query_vacancies();

  return vacancies ? vacancies : ({ });
}

// The job by that name, or nil. A name is a job's identity, so a settlement
// offers one of each, and it is named after the type it is filled from. Two
// pubs are one barman job with two seats and two spots, not two jobs.
mapping query_vacancy(string job)
{
  mapping * all;
  int i;

  all = query_vacancies();
  for (i = 0; i < sizeof(all); i++)
    if (all[i][VACANCY_JOB] == job)
      return all[i];

  return nil;
}

// The vacancies held at one location.
mapping * query_vacancies_at(string at)
{
  mapping * all, * out;
  int i;

  all = query_vacancies();
  out = ({ });
  for (i = 0; i < sizeof(all); i++)
    if (all[i][VACANCY_WORKS_AT] == at)
      out += ({ all[i] });

  return out;
}

// Every type the settlement staffs. The population sweep scatters fauna, never
// people who hold a job, so these sources are not its business.
mapping query_vacancy_sources()
{
  mapping * all;
  mapping out;
  int i;

  all = query_vacancies();
  out = ([ ]);
  for (i = 0; i < sizeof(all); i++)
    if (all[i][VACANCY_SOURCE])
      out[all[i][VACANCY_SOURCE]] = 1;

  return out;
}

// Open a job here, or restate one that is already open. `at` is where it is
// held, `source` the type its holders are drawn from -- a blueprint path the
// first time, snapshotted into a template and kept by template id afterwards.
// Restating a job keeps what the builder is not saying: its class, its house.
void open_vacancy(string job, int count, string at, string source,
                  varargs mapping extra)
{
  mapping vacancy, previous;
  object owner;
  string game;
  int i;

  if (!job || !strlen(job) || count < 0 || !at || !strlen(at))
    return;

  owner = (object)this_object()->query_root_area();
  if (owner != this_object())
  {
    owner->open_vacancy(job, count, at, source, extra);
    return;
  }

  // Somebody who holds a job is a person, never a head of statistical fauna,
  // but that is a fact about the type and the type is authored: the template
  // says `sentient` or the job draws from the wrong kind. Nothing writes it
  // here -- templates are read-only -- so the mistake is reported instead, by
  // `build vacancy add` and by `npcs template check`.
  if (source && strlen(source))
    source = (string)this_object()->query_template_from_source(source);

  previous = query_vacancy(job);

  vacancy = ([ VACANCY_JOB:    job,
               VACANCY_COUNT:  count,
               VACANCY_WORKS_AT:     at,
               VACANCY_SOURCE: source ]);

  if (mappingp(extra))
  {
    string * keys;
    keys = map_indices(extra);
    for (i = 0; i < sizeof(keys); i++)
      vacancy[keys[i]] = extra[keys[i]];
  }

  // carry the parts the builder is not restating
  if (previous)
  {
    if (previous[VACANCY_CLASS] && !vacancy[VACANCY_CLASS])
      vacancy[VACANCY_CLASS] = previous[VACANCY_CLASS];
    if (previous[VACANCY_HOME] && !vacancy[VACANCY_HOME])
      vacancy[VACANCY_HOME] = previous[VACANCY_HOME];

    vacancies -= ({ previous });
  }

  vacancies += ({ vacancy });

  this_object()->rebuild_npc_caps();
  this_object()->save_me();
}

// Close a job and let go of whoever held it: its census people are culled
// (destroying any that are live and deleting their savefiles), so closing a
// post never leaves an orphan behind.
void close_vacancy(string job)
{
  mapping vacancy;
  string * ids;
  object owner;
  int i;

  owner = (object)this_object()->query_root_area();
  if (owner != this_object())
  {
    owner->close_vacancy(job);
    return;
  }

  vacancy = query_vacancy(job);
  if (!vacancy)
    return;

  ids = map_indices((mapping)this_object()->query_npc_census());
  for (i = 0; i < sizeof(ids); i++)
  {
    mapping e;
    object npc;

    e = ((mapping)this_object()->query_npc_census())[ids[i]];
    if (e[CENSUS_VACANCY] != job)
      continue;

    npc = AREA_HANDLER->find_live_npc(ids[i]);
    if (npc)
      npc->dest_me();

    this_object()->drop_census_entry(ids[i]);
  }

  vacancies -= ({ vacancy });

  this_object()->rebuild_npc_caps();
  this_object()->save_me();
}

// What the job trains its holders in. A farmer is not a soldier, and the type
// they are drawn from cannot say so: one type staffs several settlements, and
// the job is what decides how a person fights. "" clears it and the holders
// fall back to whatever the type carries. Returns 0 if the job is not open.
int set_vacancy_class(string job, string path)
{
  mapping vacancy;
  object owner;

  owner = (object)this_object()->query_root_area();
  if (owner != this_object())
    return (int)owner->set_vacancy_class(job, path);

  vacancy = query_vacancy(job);
  if (!vacancy)
    return 0;

  if (path && strlen(path))
    vacancy[VACANCY_CLASS] = path;
  else
    map_delete(vacancy, VACANCY_CLASS);

  this_object()->save_me();
  return 1;
}

// The kit this job hands a new holder: one list of interchangeable items per
// slot, each holder rolling one from each. It belongs to the post rather than
// to the people it draws from, so two settlements can arm the same trade
// differently. An empty spec clears it. Returns 0 if the job is not open.
int set_vacancy_equipment(string job, mixed * spec)
{
  mapping vacancy;
  object owner;

  owner = (object)this_object()->query_root_area();
  if (owner != this_object())
    return (int)owner->set_vacancy_equipment(job, spec);

  vacancy = query_vacancy(job);
  if (!vacancy)
    return 0;

  if (pointerp(spec) && sizeof(spec))
    vacancy[VACANCY_EQUIPMENT] = spec;
  else
    map_delete(vacancy, VACANCY_EQUIPMENT);

  this_object()->save_me();
  return 1;
}

// The hours this job keeps, keyed by game hour as a string. Also the post's:
// a night watch and a day watch are the same people on different shifts. An
// empty mapping clears it. Returns 0 if the job is not open.
int set_vacancy_timetable(string job, mapping hours)
{
  mapping vacancy;
  object owner;

  owner = (object)this_object()->query_root_area();
  if (owner != this_object())
    return (int)owner->set_vacancy_timetable(job, hours);

  vacancy = query_vacancy(job);
  if (!vacancy)
    return 0;

  if (mappingp(hours) && map_sizeof(hours))
    vacancy[VACANCY_TIMETABLE] = hours;
  else
    map_delete(vacancy, VACANCY_TIMETABLE);

  this_object()->save_me();
  return 1;
}

// Whether the job's holders are housed among the settlement's own. A trade whose
// people live in the town like anyone else is flagged here; a post that comes
// with a house of its own uses set_vacancy_home instead. Returns 0 if the job is
// not open.
int set_vacancy_resident(string job, int flag)
{
  mapping vacancy;
  object owner;

  owner = (object)this_object()->query_root_area();
  if (owner != this_object())
    return (int)owner->set_vacancy_resident(job, flag);

  vacancy = query_vacancy(job);
  if (!vacancy)
    return 0;

  if (flag)
    vacancy[VACANCY_RESIDENT] = 1;
  else
    map_delete(vacancy, VACANCY_RESIDENT);

  this_object()->save_me();
  return 1;
}

// The house that comes with the job. Its holder lives there, and so does the
// next one: the house follows the post, not the person. Returns how many
// vacancies were bound (0 if the job is not open).
int set_vacancy_home(string job, string home)
{
  mapping * all;
  object owner;
  int i, found;

  owner = (object)this_object()->query_root_area();
  if (owner != this_object())
    return (int)owner->set_vacancy_home(job, home);

  all = query_vacancies();
  for (i = 0; i < sizeof(all); i++)
  {
    object npc;

    if (all[i][VACANCY_JOB] != job)
      continue;

    all[i][VACANCY_HOME] = home;
    found++;

    // move whoever holds it now
    npc = (object)this_object()->query_vacancy_holder(all[i]);
    if (npc)
    {
      npc->set_home(home);
      npc->save_npc();
    }
  }

  if (found)
    this_object()->save_me();

  return found;
}

// ---------------------------------------------------------------------------
// Who holds what
// ---------------------------------------------------------------------------

// The census ids of everybody holding this vacancy.
string * query_vacancy_holders(mapping vacancy)
{
  mapping census;
  string * ids, * out;
  int i;

  census = (mapping)this_object()->query_npc_census();
  ids = map_indices(census);
  out = ({ });

  for (i = 0; i < sizeof(ids); i++)
    if (census[ids[i]][CENSUS_VACANCY] == vacancy[VACANCY_JOB])
      out += ({ ids[i] });

  return out;
}

// The live NPC holding a one-seat vacancy, or nil.
object query_vacancy_holder(mapping vacancy)
{
  string * ids;

  ids = query_vacancy_holders(vacancy);
  return sizeof(ids) ? AREA_HANDLER->find_live_npc(ids[0]) : nil;
}

// ---------------------------------------------------------------------------
// Equipment
// ---------------------------------------------------------------------------

void equip_npc(object npc, string * paths)
{
  int i;

  if (!npc || !pointerp(paths) || !sizeof(paths))
    return;
  for (i = 0; i < sizeof(paths); i++)
    npc->add_clone(paths[i], 1);
  npc->init_equip();
}

// Resolve a kit spec into one concrete kit. The spec is an array of slots; each
// slot is an array of interchangeable blueprints and one is picked at random (a
// fixed item is just a one-element slot). Rolled once per NPC at assignment, so
// a citizen keeps the same weapon for life instead of re-rolling on every
// materialization.
string * resolve_equipment(mixed * spec)
{
  string * kit;
  int i;

  kit = ({ });
  if (!pointerp(spec))
    return kit;

  for (i = 0; i < sizeof(spec); i++)
  {
    mixed slot;
    slot = spec[i];
    if (pointerp(slot) && sizeof(slot))
      kit += ({ slot[random(sizeof(slot))] });
    else if (stringp(slot))
      kit += ({ slot });
  }

  return kit;
}

// ---------------------------------------------------------------------------
// Filling the seats
// ---------------------------------------------------------------------------

// Where the nth holder of a job stands. A job that lists its places seats one
// holder in each, in turn; a job that lists none is worked where it was
// declared. The list is data, not a guess: see VACANCY_SPOTS.
string spot_for(mapping vacancy, int nth)
{
  mixed spots;

  spots = vacancy ? vacancy[VACANCY_SPOTS] : nil;
  if (!pointerp(spots) || !sizeof(spots))
    return vacancy ? vacancy[VACANCY_WORKS_AT] : nil;

  return spots[nth % sizeof(spots)];
}

// Add a place this job is worked, or take one off the list. Returns 1 when the
// list changed.
int add_vacancy_spot(string job, string location_file)
{
  mapping vacancy;
  object owner;

  owner = (object)this_object()->query_root_area();
  if (owner != this_object())
    return (int)owner->add_vacancy_spot(job, location_file);

  vacancy = query_vacancy(job);
  if (!vacancy || !location_file || !strlen(location_file))
    return 0;

  if (!pointerp(vacancy[VACANCY_SPOTS]))
    vacancy[VACANCY_SPOTS] = ({ });
  if (member_array(location_file, vacancy[VACANCY_SPOTS]) != -1)
    return 0;

  vacancy[VACANCY_SPOTS] += ({ location_file });
  this_object()->save_me();
  return 1;
}

int remove_vacancy_spot(string job, string location_file)
{
  mapping vacancy;
  object owner;

  owner = (object)this_object()->query_root_area();
  if (owner != this_object())
    return (int)owner->remove_vacancy_spot(job, location_file);

  vacancy = query_vacancy(job);
  if (!vacancy || !pointerp(vacancy[VACANCY_SPOTS]) ||
      member_array(location_file, vacancy[VACANCY_SPOTS]) == -1)
    return 0;

  vacancy[VACANCY_SPOTS] -= ({ location_file });
  this_object()->save_me();
  return 1;
}


// A generated given-name (lowercase) for one of this area's citizens: the name
// generator draws it from the area citizenship's name style, in the form
// matching the given gender (a GENDER_* id). It does not depend on the job.
// Returns nil when the area has no citizenship, the citizenship declares no
// name style, or the generator has no wordlist for it -- the NPC then keeps its
// template's name.
string generate_citizen_name(int gender)
{
  string cpath, style, word;
  object cit;

  // the naming style is a trait of the nationality, not of the town: every
  // settlement under the same country draws its citizens' names from one pool,
  // and an area with no citizenship of its own borrows the pool of the region
  // it sits in without taking its nationality
  cpath = (string)this_object()->query_naming_citizenship_path();
  if (!strlen(cpath))
    return nil;

  cit = load_object(cpath);
  if (!cit)
    return nil;

  style = cit->query_name_style();
  if (!style || !strlen(style))
    return nil;

  word = (gender == GENDER_FEMALE) ? "female" : "male";
  // order 3 with a length window keeps results name-like without copying the
  // source list; see packages/namegen for the quality/size trade-off
  return NAMEGEN_OB->generate_for(style, word, 3, 4, 9);
}

// Take somebody on for a job. Data-only: the person materializes when its place
// loads (npc_restore), which is where the body is built. Returns the uuid.
//
// This is the seam the generator plugs into. Today the body still comes from a
// transitional type template; when it comes from the culture instead, only this
// function changes.
private string assign_npc_to_vacancy(mapping vacancy, string where)
{
  string id, game, source, at;

  source = vacancy[VACANCY_SOURCE];
  at = vacancy[VACANCY_WORKS_AT];
  if (!source || !strlen(source) || !at || !strlen(at))
    return nil;

  // where the person stands is not which post it holds: a job that lists its
  // places seats its holders across them, and every one of them still holds the
  // one post recorded at `at`
  if (!where || !strlen(where))
    where = at;

  game = game_from_path((string)this_object()->query_area_path());
  id = UUID_OB->uuid();

  this_object()->add_census_entry(
    id, ([ "source":           source,
           "savefile":         npc_save_dir(game, id) + NPC_SAVE_FILE,
           CENSUS_VACANCY:     vacancy[VACANCY_JOB],
           CENSUS_WORKS_AT:    where,
           CENSUS_LOCATION:    where ]));

  return id;
}

// Seat the holders of a job across its places again. Staffing hands each new
// holder its own spot, but a job staffed before its list was written put
// everybody on the place it was declared at and has kept them there since. This
// walks the holders and gives each the spot the list would give it today.
// Returns how many were moved; 0 for a job that is not open or not spread.
int reseat_vacancy(string job)
{
  mapping vacancy;
  mapping census;
  object owner;
  string * holders;
  int i, moved;

  owner = (object)this_object()->query_root_area();
  if (owner != this_object())
    return (int)owner->reseat_vacancy(job);

  vacancy = query_vacancy(job);
  if (!vacancy || !pointerp(vacancy[VACANCY_SPOTS]) ||
      !sizeof(vacancy[VACANCY_SPOTS]))
    return 0;

  census = (mapping)this_object()->query_npc_census();
  holders = (string *)query_vacancy_holders(vacancy);

  for (i = 0; i < sizeof(holders); i++)
  {
    mapping entry;
    string spot;

    entry = census[holders[i]];
    if (!entry)
      continue;

    spot = spot_for(vacancy, i);
    if (!spot || !strlen(spot) || entry[CENSUS_WORKS_AT] == spot)
      continue;

    // only where the job is done changes; where the person is right now is
    // their own business, and the timetable walks them over at its hour
    entry[CENSUS_WORKS_AT] = spot;
    moved++;
  }

  if (moved)
    this_object()->save_me();

  return moved;
}

// Whether the place a job is held in is shut to it. The location answers
// through its components; the job itself is never touched.
private int _venue_closed(mapping vacancy)
{
  string at;
  object loc;

  at = vacancy ? vacancy[VACANCY_WORKS_AT] : nil;
  if (!at || !strlen(at))
    return 0;

  loc = load_object(LOCATION_HANDLER)->load_location(at);
  return loc ? (int)loc->query_venue_closed() : 0;
}

// Staff one job: take somebody on for every empty seat, and bring them in if
// their place is loaded. Idempotent: a full job hires nobody, and
// restore_location_npcs leaves an already-present uuid alone.
private void _staff(mapping vacancy)
{
  int have, want, i;

  if (!vacancy || _venue_closed(vacancy))
    return;

  want = vacancy[VACANCY_COUNT];
  have = sizeof(query_vacancy_holders(vacancy));

  for (i = have; i < want; i++)
  {
    string where;
    object loc;

    // a job that lists its places hands each holder one of them, in turn
    where = spot_for(vacancy, i);
    assign_npc_to_vacancy(vacancy, where);

    if (!where || !strlen(where))
      where = vacancy[VACANCY_WORKS_AT];

    loc = (object)this_object()->query_loaded_location(where);
    if (loc)
      this_object()->restore_location_npcs(loc);
  }
}

// Staff every job this settlement offers -- the whole-settlement pass, which a
// scheduled population check would call. Nothing calls it on its own: how a
// settlement replaces its dead is undecided, so hiring is always asked for.
void staff_vacancies()
{
  mapping * all;
  int i;

  all = query_vacancies();
  for (i = 0; i < sizeof(all); i++)
    _staff(all[i]);
}

// Staff only the jobs held at one location, for when the answer is wanted about
// one place rather than the whole settlement.
void staff_vacancies_at(string at)
{
  mapping * here;
  int i;

  here = query_vacancies_at(at);
  for (i = 0; i < sizeof(here); i++)
    _staff(here[i]);
}
