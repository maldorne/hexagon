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

// The first vacancy for `job`, or nil. Two places can offer the same job -- a
// town with two pubs wants two barmen -- so a job name alone does not identify
// a post; pass `at` to name one exactly.
mapping query_vacancy(string job, varargs string at)
{
  mapping * all;
  int i;

  all = query_vacancies();
  for (i = 0; i < sizeof(all); i++)
    if (all[i][VACANCY_JOB] == job &&
        (!at || !strlen(at) || all[i][VACANCY_WORKS_AT] == at))
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

  if (source && strlen(source))
  {
    game = game_from_path((string)this_object()->query_area_path());
    if (!BESTIARY_HANDLER->has_template(game, source))
      BESTIARY_HANDLER->add_template(source);
    source = (string)this_object()->query_template_from_source(source);

    // somebody who holds a job is a person, named and tracked, never a head of
    // statistical fauna
    BESTIARY_HANDLER->set_template_behaviour(game, source, ([ "sentient": 1 ]));
  }

  previous = query_vacancy(job, at);

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
void close_vacancy(string job, varargs string at)
{
  mapping vacancy;
  string * ids;
  object owner;
  int i;

  owner = (object)this_object()->query_root_area();
  if (owner != this_object())
  {
    owner->close_vacancy(job, at);
    return;
  }

  vacancy = query_vacancy(job, at);
  if (!vacancy)
    return;

  ids = map_indices((mapping)this_object()->query_npc_census());
  for (i = 0; i < sizeof(ids); i++)
  {
    mapping e;
    object npc;

    e = ((mapping)this_object()->query_npc_census())[ids[i]];
    if (e[CENSUS_VACANCY] != job || e[CENSUS_WORKS_AT] != vacancy[VACANCY_WORKS_AT])
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
int set_vacancy_class(string job, string path, varargs string at)
{
  mapping vacancy;
  object owner;

  owner = (object)this_object()->query_root_area();
  if (owner != this_object())
    return (int)owner->set_vacancy_class(job, path, at);

  vacancy = query_vacancy(job, at);
  if (!vacancy)
    return 0;

  if (path && strlen(path))
    vacancy[VACANCY_CLASS] = path;
  else
    map_delete(vacancy, VACANCY_CLASS);

  this_object()->save_me();
  return 1;
}

// The house that comes with the job. Its holder lives there, and so does the
// next one: the house follows the post, not the person. Returns how many
// vacancies were bound (0 if the job is not open).
int set_vacancy_home(string job, string home, varargs string at)
{
  mapping * all;
  object owner;
  int i, found;

  owner = (object)this_object()->query_root_area();
  if (owner != this_object())
    return (int)owner->set_vacancy_home(job, home, at);

  all = query_vacancies();
  for (i = 0; i < sizeof(all); i++)
  {
    object npc;

    if (all[i][VACANCY_JOB] != job)
      continue;
    if (at && strlen(at) && all[i][VACANCY_WORKS_AT] != at)
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
    if (census[ids[i]][CENSUS_VACANCY] == vacancy[VACANCY_JOB] &&
        census[ids[i]][CENSUS_WORKS_AT] == vacancy[VACANCY_WORKS_AT])
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

// Gear the live holders of a job that still carry nothing, reading the kit from
// the type template (its authoritative home). A holder that already has gear
// keeps it -- a saved citizen's gear never changes. Called after the builder
// changes a kit, so existing empty-handed holders pick it up without a respawn.
void reequip_vacancy_holders(string job, varargs string at)
{
  mapping vacancy, template;
  object loc;
  object * inv;
  mixed * spec;
  string source;
  int i;

  vacancy = query_vacancy(job, at);
  if (!vacancy)
    return;

  source = vacancy[VACANCY_SOURCE];
  template = (source && strlen(source))
               ? BESTIARY_HANDLER->query_template(
                   game_from_path((string)this_object()->query_area_path()),
                   source)
               : nil;
  spec = template ? template["equipment"] : nil;
  if (!pointerp(spec) || !sizeof(spec))
    return;

  loc = (object)this_object()->query_loaded_location(vacancy[VACANCY_WORKS_AT]);
  if (!loc)
    return;

  inv = all_inventory(loc);
  for (i = 0; i < sizeof(inv); i++)
  {
    string uuid;
    mapping e;

    if (!inv[i] || !inv[i]->query_persisted())
      continue;
    uuid = inv[i]->query_npc_uuid();
    e = uuid ? ((mapping)this_object()->query_npc_census())[uuid] : nil;
    if (!e || e[CENSUS_VACANCY] != job)
      continue;
    if (sizeof(all_inventory(inv[i])))
      continue;

    equip_npc(inv[i], resolve_equipment(spec));
    inv[i]->save_npc();
  }
}

// ---------------------------------------------------------------------------
// Filling the seats
// ---------------------------------------------------------------------------

// Where the next holder of a spread job should stand.
//
// A vacancy records one place -- wherever the builder stood when opening it --
// but some jobs are rarely done in a single spot: a farm is a set of fields, a
// market a row of stalls. Those spread over every location of the area carrying
// the same working component as the recorded spot, handed out in turn. With one
// such location, or none, the recorded spot is used and nothing changes.
//
// Deciding here rather than on the individual is what makes it survive death:
// the replacement is placed by the same rule instead of inheriting whatever
// spot the last one happened to hold.
string spread_spot_for(mapping vacancy, int nth)
{
  object seat;
  object * comps;
  string * kinds, * files, * candidates;
  int i, j;

  if (!vacancy[VACANCY_SPREAD])
    return vacancy[VACANCY_WORKS_AT];

  seat = (object)this_object()->load_location(vacancy[VACANCY_WORKS_AT]);
  if (!seat)
    return vacancy[VACANCY_WORKS_AT];

  // what makes that spot a workplace, ignoring what every open-air location has
  kinds = ({ });
  comps = seat->query_components();
  for (i = 0; i < sizeof(comps); i++)
    if (comps[i]->query_type() != LOCATION_COMPONENT_OUTSIDE)
      kinds += ({ comps[i]->query_type() });

  if (!sizeof(kinds))
    return vacancy[VACANCY_WORKS_AT];

  candidates = ({ });
  files = map_indices((mapping)this_object()->query_locations());
  for (i = 0; i < sizeof(files); i++)
  {
    object loc;

    loc = (object)this_object()->load_location(files[i]);
    if (!loc)
      continue;
    for (j = 0; j < sizeof(kinds); j++)
      if (loc->query_component_by_type(kinds[j]))
      {
        candidates += ({ files[i] });
        break;
      }
  }

  if (sizeof(candidates) < 2)
    return vacancy[VACANCY_WORKS_AT];

  return candidates[nth % sizeof(candidates)];
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

  // where the person stands is not which post it holds: a spread job seats its
  // holders across the area's like places, and every one of them still holds
  // the one post recorded at `at`
  if (!where || !strlen(where))
    where = at;

  game = game_from_path((string)this_object()->query_area_path());
  id = UUID_OB->uuid();

  this_object()->add_census_entry(
    id, ([ "source":        source,
           CENSUS_LOCATION: where,
           "savefile":      npc_save_dir(game, id) + NPC_SAVE_FILE,
           CENSUS_VACANCY:  vacancy[VACANCY_JOB],
           CENSUS_WORKS_AT:       vacancy[VACANCY_WORKS_AT] ]));

  return id;
}

// Take on as many people as the job is short of. A seat emptied by a death is
// refilled here, not at the moment of death.
void fill_vacancy(mapping vacancy)
{
  object loc;
  int have, want, i;

  if (!vacancy)
    return;

  want = vacancy[VACANCY_COUNT];
  have = sizeof(query_vacancy_holders(vacancy));

  // a spread job hands each holder its own place, in turn
  for (i = have; i < want; i++)
    assign_npc_to_vacancy(vacancy, spread_spot_for(vacancy, i));

  // bring the new people in now if their place is already loaded; otherwise
  // they arrive when it next loads
  loc = (object)this_object()->query_loaded_location(vacancy[VACANCY_WORKS_AT]);
  if (loc)
    this_object()->restore_location_npcs(loc);
}

// Staff every job the settlement offers. Idempotent: a job already at its count
// takes nobody on.
void fill_vacancies()
{
  mapping * all;
  int i;

  all = query_vacancies();
  for (i = 0; i < sizeof(all); i++)
    fill_vacancy(all[i]);
}

// Fill whatever is held at one location, as it loads.
void fill_vacancies_at(string at)
{
  mapping * here;
  int i;

  here = query_vacancies_at(at);
  for (i = 0; i < sizeof(here); i++)
    fill_vacancy(here[i]);
}

// call_out target: take somebody on again a while after a holder died at `at`.
void _refill_vacancy(string at)
{
  fill_vacancies_at(at);
}
