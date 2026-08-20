
// The roster: how many of each kind of NPC belong to this area.
//
// Two mappings and the rules that keep them honest. `npc_intended` is the cap --
// what may live here and at most how many -- and `npc_sources` is where that cap
// came from: the per-location provenance recorded when each room was converted.
// The cap is derived from the provenance rather than typed in, which is what
// makes a reconversion idempotent, and it is also why removing a source for good
// means clearing its provenance too and not merely its cap.
//
// A few kinds are deliberately kept off the roster: whatever a POI vacancy
// claims, and the citizenship's guard source. Those are placed by hand or by
// diplomacy, so letting the statistical sweep also scatter them would duplicate
// them. The area's level band lives here too, since it is what an individual's
// level is drawn from at birth.
//
// Inherited by /lib/location/area.c. Calls into the rest of the area go through
// this_object(): an area is a single object carrying the whole inheritance tree,
// so the call resolves at run time against the complete program. Only public
// functions are reachable that way, and results come back as mixed, hence the
// casts.

#include <areas/area.h>
#include <areas/poi.h>
#include <areas/diplomacy.h>
#include <living/persisted.h>

// Dynamic NPC population. Both are keyed by the NPC's source path (the
// hand-authored .c it is cloned from).
//   npc_intended: configuration -- what may spawn here and how many.
//     ([ npc_path : ([ "max": int, "resident": 1 (optional) ]) ])
//   ("resident" is the design-time flag that lets the housing system give this
//    source's NPCs a home; unflagged sources are never housed.)
//   (Fine-grained placement -- which location/POI, sector-type weighting --
//    is layered on in F2; F1 spawns anywhere in the area up to the cap.)
//   npc_census:   live state -- which concrete NPCs exist and where.
//     ([ uuid : ([ "source": npc_path, "location": location_file,
//                  "savefile": npc.o path ]) ])
// The census is the authoritative summary of the area's population; NPC
// objects are materialized into a location on load and drained on unload,
// but the census entry survives so the same NPC comes back.
mapping npc_intended;
// Per-location NPC provenance from the room2loc conversion:
//   ([ location_file : ([ npc_path : count ]) ])
// This is the seed for npc_intended: the area cap for a source is the sum
// of that source's add_clone counts across every room of the area. Keeping
// it per location makes reconversion idempotent (a location overwrites only
// its own entry) without reloading the whole area.
mapping npc_sources;
// Average level of the area's NPCs and how far individual NPCs may deviate
// from it. An NPC's level is decided once, at census assignment, as
//   npc_default_level + template level_area_modifier  ±  random(npc_default_level_spread + 1)
// (a template that carries a concrete "level" overrides all of this). The
// deviation uses random(spread + 1) because random(2) yields only 0 or 1, so
// a spread of 2 gives a swing of 0..2. Stored per NPC in the census, so each
// NPC keeps the level it was created with.
int npc_default_level;
int npc_default_level_spread;

void create()
{
  npc_intended = ([ ]);
  npc_sources = ([ ]);
  npc_default_level = 1;
  npc_default_level_spread = 0;
}

// Flag (or clear) an intended NPC source as a settled resident. This is the
// design-time switch that decides who `assign_homes` may house. Returns 0 if the
// source is not a known intended NPC of this area.
int set_intended_resident(string source, int flag)
{
  if (!npc_intended[source])
    return 0;

  if (flag)
    npc_intended[source]["resident"] = 1;
  else
    map_delete(npc_intended[source], "resident");

  this_object()->save_me();
  return 1;
}


// ---------------------------------------------------------------------------
// Dynamic NPC population
// ---------------------------------------------------------------------------

mapping query_npc_intended() { return npc_intended; }

void set_npc_intended(mapping m)
{
  npc_intended = m ? m : ([ ]);
  this_object()->save_me();
}

// Declare (or replace) an intended NPC for this area. `source` is the NPC's
// source path -- it is the identity (census key) and is snapshotted into a
// data template on first spawn. `max` is the area-wide population cap.
void add_intended_npc(string source, int max)
{
  npc_intended[source] = ([ "max": max ]);
  this_object()->save_me();
}

void remove_intended_npc(string source)
{
  map_delete(npc_intended, source);
  this_object()->save_me();
}

mapping query_npc_sources() { return npc_sources; }

// Normalise a source to the template id the area keys everything by. A source
// arrives either already as a template id (post-conversion data) or as the
// original monster .c path (pre-conversion saves); template_id is idempotent,
// so both collapse to the same key and old saves keep working with no separate
// migration pass. Day-to-day operation never needs the .c to exist.
string _template_id(string source)
{
  if (!source || !strlen(source))
    return source;
  return BESTIARY_HANDLER->template_id(game_from_path((string)this_object()->query_area_path()), source);
}

// Fold any pre-conversion monster paths in the area's persisted state down to
// template ids, in place: npc_sources keys, vacancy sources and census
// "source". This rewrites labels only -- every uuid and live NPC keeps its
// identity, so it is not the duplicate-spawning churn that removing and
// re-adding a vacancy would cause. Idempotent (template_id is), so running it
// on every recompute is safe. Saves only when something actually changed.
private void _migrate_source_ids()
{
  string * locs, * ids;
  int i, j, changed;

  changed = 0;

  // npc_sources: re-key each location's inner (source -> count) map by id
  locs = map_indices(npc_sources);
  for (i = 0; i < sizeof(locs); i++)
  {
    mapping inner, rekeyed;
    string * keys;

    inner = npc_sources[locs[i]];
    keys = map_indices(inner);
    rekeyed = ([ ]);
    for (j = 0; j < sizeof(keys); j++)
    {
      string tid;
      tid = _template_id(keys[j]);
      if (tid != keys[j])
        changed = 1;
      rekeyed[tid] = inner[keys[j]];
    }
    npc_sources[locs[i]] = rekeyed;
  }

  // vacancy sources stored on each POI
  locs = map_indices((mapping)this_object()->query_pois());
  for (i = 0; i < sizeof(locs); i++)
  {
    mapping * vs;
    vs = ((mapping)this_object()->query_pois())[locs[i]][POI_FIELD_VACANCIES];
    for (j = 0; vs && j < sizeof(vs); j++)
    {
      string tid;
      tid = _template_id(vs[j][VACANCY_FIELD_SOURCE]);
      if (tid != vs[j][VACANCY_FIELD_SOURCE])
      {
        vs[j][VACANCY_FIELD_SOURCE] = tid;
        changed = 1;
      }
    }
  }

  // the identity recorded on each live census NPC
  ids = map_indices((mapping)this_object()->query_npc_census());
  for (i = 0; i < sizeof(ids); i++)
  {
    mapping e;
    string tid;

    e = ((mapping)this_object()->query_npc_census())[ids[i]];
    tid = _template_id(e["source"]);
    if (tid != e["source"])
    {
      e["source"] = tid;
      changed = 1;
    }
  }

  if (changed)
    this_object()->save_me();
}

// Recompute npc_intended from the per-location conversion provenance: the
// area cap for a source is the sum of its add_clone counts across every
// room of the area.
//
// Three kinds of source are deliberately kept out of the statistical roster,
// re-applied here so a reconversion cannot leak them back into the population:
//   - a source claimed by a vacancy (a unique the POI system places by hand)
//   - the area citizenship's guard (diplomacy places it at guarded POIs)
//   - anything that is not a living NPC source (add_clone also clones trees
//     and props, which are not NPCs)
void recompute_intended()
{
  string * location_files, * npc_paths;
  int i, j;
  mapping counts, clones_here, vacancy_sources, previous;
  string guard_source;

  // fold any pre-conversion monster paths in persisted state down to template
  // ids first, so the roster, vacancies and census all key consistently
  _migrate_source_ids();

  // sum each NPC source's add_clone count across every location of the area
  counts = ([ ]);
  location_files = map_indices(npc_sources);
  for (i = 0; i < sizeof(location_files); i++)
  {
    clones_here = npc_sources[location_files[i]];
    npc_paths = map_indices(clones_here);
    for (j = 0; j < sizeof(npc_paths); j++)
    {
      string tid;
      tid = _template_id(npc_paths[j]);
      counts[tid] =
        (counts[tid] ? counts[tid] : 0) + clones_here[npc_paths[j]];
    }
  }

  vacancy_sources = (mapping)this_object()->query_vacancy_sources();

  // the citizenship's guard NPC is placed by diplomacy at guarded POIs, so it
  // must not also be scattered by the population sweep as statistical filler
  guard_source = "";
  if (strlen((string)this_object()->query_citizenship()))
    guard_source = _template_id(
      DIPLOMACY_HANDLER->query_guard_path(game_from_path((string)this_object()->query_area_path()),
                                          (string)this_object()->query_citizenship()));

  // npc_sources only ever holds living NPC sources: conversion filters trees
  // and props out (it loads each source once, keeps only query_monster ones)
  // before recording them, so the roster no longer re-loads the source .c to
  // re-check -- it just sums the counts.
  // keep the previous config so design-time flags (resident, ...) set by the
  // builder survive a recompute -- the counts are re-derived, the flags are not
  previous = npc_intended;
  npc_intended = ([ ]);
  npc_paths = map_indices(counts);
  for (i = 0; i < sizeof(npc_paths); i++)
  {
    // a unique bound to a vacancy is placed by the POI system, never by
    // the population sweep
    if (vacancy_sources[npc_paths[i]])
      continue;

    // the area citizenship's guard is diplomacy-placed, not filler
    if (strlen(guard_source) && npc_paths[i] == guard_source)
      continue;

    npc_intended[npc_paths[i]] = ([ "max": counts[npc_paths[i]] ]);

    // carry forward design-time flags recompute must not clobber
    if (previous[npc_paths[i]] && previous[npc_paths[i]]["resident"])
      npc_intended[npc_paths[i]]["resident"] = 1;
  }
}

// Record a converted location's NPC sources (its source room's add_clone
// counts, kept on the location as _original_add_clones) and recompute the area
// cap. Idempotent: reconverting a location overwrites only its own entry. This
// is the room2loc seed for the area's population.
void set_location_npc_sources(string location_file, mapping clones)
{
  if (clones && map_sizeof(clones))
    npc_sources[location_file] = map_copy(clones);
  else
    map_delete(npc_sources, location_file);

  recompute_intended();

  // register with the population sweep so it keeps this area topped up
  if (map_sizeof(npc_intended))
    POPULATION_HANDLER->include_area((string)this_object()->query_area_path());

  this_object()->save_me();
}

// The gender a census NPC is born with, decided once at assignment so it stays
// stable across saves and restores. A fixed template dictates it; a bimodal
// one (no "gender" key) rolls male/female here. Stored in the census entry and
// handed to the NPC before its template is applied, so the per-gender strings
// match.
int decide_gender(string game, string source)
{
  return BESTIARY_HANDLER->roll_gender(
           BESTIARY_HANDLER->query_template(game, source));
}

int query_area_level() { return npc_default_level; }
int query_area_spread() { return npc_default_level_spread; }

void set_area_level(int n)
{
  npc_default_level = n < 1 ? 1 : n;
  this_object()->save_me();
}

void set_area_spread(int n)
{
  npc_default_level_spread = n < 0 ? 0 : n;
  this_object()->save_me();
}

// The level a census NPC is born with, decided once at assignment so it stays
// stable for the life of that NPC (like its gender). A template with a
// concrete "level" dictates it outright; otherwise the level derives from the
// area: npc_default_level + the template's level_area_modifier, swung by up to the
// area spread (random(spread + 1), sign random). Never below 1.
int decide_level(string game, string source)
{
  mapping t;
  int base, dev;

  t = BESTIARY_HANDLER->query_template(game, source);

  // a template that fixes a concrete level ignores area level / spread
  if (t && t["level"])
    return t["level"];

  base = npc_default_level + (t && t["level_area_modifier"] ? t["level_area_modifier"] : 0);

  // random(spread + 1) gives 0..spread (random(2) is only 0 or 1), applied up
  // or down at random
  dev = random(npc_default_level_spread + 1);
  if (random(2))
    base += dev;
  else
    base -= dev;

  return base < 1 ? 1 : base;
}

