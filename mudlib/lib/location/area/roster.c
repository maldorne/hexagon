
// The roster: how many of each kind of NPC belong to this area.
//
// Two mappings and the rules that keep them honest. `npc_caps` is the cap --
// what may live here and at most how many -- and `original_npc_sources` is
// where that cap came from: the per-location provenance recorded when each room
// was converted.
// The cap is derived from the provenance rather than typed in, which is what
// makes a reconversion idempotent, and it is also why removing a source for good
// means clearing its provenance too and not merely its cap.
//
// A few kinds are deliberately kept off the roster: whatever a POI vacancy
// claims, and the citizenship's guard source. Those are placed by hand or by
// diplomacy, so letting the statistical sweep also scatter them would duplicate
// them. The area's level band lives here too, since it is what an individual's
// level is drawn from at birth.

#include <areas/area.h>
#include <areas/poi.h>
#include <areas/diplomacy.h>
#include <living/persisted.h>

// Dynamic NPC population. Both are keyed by the NPC's source path (the
// hand-authored .c it is cloned from).
//   npc_caps: configuration -- what may spawn here and how many.
//     ([ npc_path : ([ "max": int, "resident": 1 (optional) ]) ])
//   ("resident" is the design-time flag that lets the housing system give this
//    source's NPCs a home; unflagged sources are never housed.)
//   (Fine-grained placement -- which location/POI, sector-type weighting --
//    is layered on in F2; F1 spawns anywhere in the area up to the cap.)
//   npc_census:   live state -- which concrete NPCs exist and where.
//     ([ uuid : ([ "source": npc_path, "current_location": location_file,
//                  "savefile": npc.o path ]) ])
// The census is the authoritative summary of the area's population; NPC
// objects are materialized into a location on load and drained on unload,
// but the census entry survives so the same NPC comes back.
int decide_level(string game, string source);

mapping npc_caps;
// Per-location NPC provenance from the room2loc conversion:
//   ([ location_file : ([ npc_path : count ]) ])
// This is the seed for npc_caps: the area cap for a source is the sum
// of that source's add_clone counts across every room of the area. Keeping
// it per location makes reconversion idempotent (a location overwrites only
// its own entry) without reloading the whole area.
mapping original_npc_sources;
// Average level of the area's NPCs and how far individual NPCs may deviate
// from it. An NPC's level is decided once, at census assignment, as
//   npc_default_level + template level_area_modifier  ±  random(npc_default_level_spread + 1)
// (a template that carries a concrete "level" overrides all of this). The
// deviation uses random(spread + 1) because random(2) yields only 0 or 1, so
// a spread of 2 gives a swing of 0..2. Stored per NPC in the census, so each
// NPC keeps the level it was created with.
int npc_default_level;
int npc_default_level_spread;

// How strong the people of this place are, as the range every one of their
// eight stats is rolled in. Strength belongs to the place, not to the race or
// the trade: the same orcs make a weak village and a hard stronghold, and a
// range fixed by either could not tell the two apart. The race's modifiers
// apply on top of the roll. 0/0 means the area declares none and whatever the
// type carries is left alone.
int npc_stat_low;
int npc_stat_high;

void create()
{
  npc_caps = ([ ]);
  original_npc_sources = ([ ]);
  npc_default_level = 1;
  npc_default_level_spread = 0;
  npc_stat_low = 0;
  npc_stat_high = 0;
}

// Flag (or clear) an intended NPC source as a settled resident. This is the
// design-time switch that decides who `assign_homes` may house. Returns 0 if the
// source is not a known intended NPC of this area.
int set_npc_resident(string source, int flag)
{
  object owner;

  owner = (object)this_object()->query_root_area();
  if (owner != this_object())
    return (int)owner->set_npc_resident(source, flag);

  if (!npc_caps[source])
    return 0;

  if (flag)
    npc_caps[source]["resident"] = 1;
  else
    map_delete(npc_caps[source], "resident");

  this_object()->save_me();
  return 1;
}


// ---------------------------------------------------------------------------
// Dynamic NPC population
// ---------------------------------------------------------------------------

// The caps this area works from -- its community's, when it delegates.
mapping query_npc_caps()
{
  object owner;

  owner = (object)this_object()->query_root_area();
  return owner == this_object() ? npc_caps
                                : (mapping)owner->query_npc_caps();
}

void set_npc_caps(mapping m)
{
  object owner;

  owner = (object)this_object()->query_root_area();
  if (owner != this_object())
  {
    owner->set_npc_caps(m);
    return;
  }

  npc_caps = m ? m : ([ ]);
  this_object()->save_me();
}

// Declare (or replace) an intended NPC for this area. `source` is the NPC's
// source path -- it is the identity (census key) and is snapshotted into a
// data template on first spawn. `max` is the area-wide population cap.
void add_intended_npc(string source, int max)
{
  object owner;

  owner = (object)this_object()->query_root_area();
  if (owner != this_object())
  {
    owner->add_intended_npc(source, max);
    return;
  }

  npc_caps[source] = ([ "max": max ]);
  this_object()->save_me();
}

void remove_intended_npc(string source)
{
  object owner;

  owner = (object)this_object()->query_root_area();
  if (owner != this_object())
  {
    owner->remove_intended_npc(source);
    return;
  }

  map_delete(npc_caps, source);
  this_object()->save_me();
}

mapping query_npc_sources()
{
  object owner;

  owner = (object)this_object()->query_root_area();
  return owner == this_object() ? original_npc_sources
                                : (mapping)owner->query_npc_sources();
}

// The template id for a blueprint path. Everything the area stores is keyed by
// template id; this is what turns a path handed in from outside -- a builder
// naming an NPC .c, a guard source read from the diplomacy graph -- into that
// key. Idempotent, so feeding it an id back returns it unchanged.
string query_template_from_source(string source)
{
  if (!source || !strlen(source))
    return source;
  return BESTIARY_HANDLER->template_id(game_from_path((string)this_object()->query_area_path()), source);
}

// Recompute npc_caps from the per-location conversion provenance: the
// area cap for a source is the sum of its add_clone counts across every
// room of the area.
//
// Three kinds of source are deliberately kept out of the statistical roster,
// re-applied here so a reconversion cannot leak them back into the population:
//   - a source claimed by a vacancy (a unique the POI system places by hand)
//   - the area citizenship's guard (diplomacy places it at guarded POIs)
//   - anything that is not a living NPC source (add_clone also clones trees
//     and props, which are not NPCs)
void rebuild_npc_caps()
{
  string * location_files, * npc_paths;
  int i, j;
  mapping counts, clones_here, vacancy_sources, previous;
  string guard_source;

  // sum each NPC source's add_clone count across every location of the area
  counts = ([ ]);
  location_files = map_indices(original_npc_sources);
  for (i = 0; i < sizeof(location_files); i++)
  {
    clones_here = original_npc_sources[location_files[i]];
    npc_paths = map_indices(clones_here);
    for (j = 0; j < sizeof(npc_paths); j++)
    {
      string tid;
      tid = query_template_from_source(npc_paths[j]);
      counts[tid] =
        (counts[tid] ? counts[tid] : 0) + clones_here[npc_paths[j]];
    }
  }

  vacancy_sources = (mapping)this_object()->query_vacancy_sources();

  // the citizenship's guard NPC is placed by diplomacy at guarded POIs, so it
  // must not also be scattered by the population sweep as statistical filler
  guard_source = "";
  if (strlen((string)this_object()->query_citizenship()))
    guard_source = query_template_from_source(
      DIPLOMACY_HANDLER->query_guard_path(game_from_path((string)this_object()->query_area_path()),
                                          (string)this_object()->query_citizenship()));

  // original_npc_sources only ever holds living NPC sources: conversion filters
  // trees and props out (it loads each source once, keeps only query_monster
  // ones) before recording them, so the roster no longer re-loads the source .c
  // to re-check -- it just sums the counts.
  // keep the previous config so design-time flags (resident, ...) set by the
  // builder survive a recompute -- the counts are re-derived, the flags are not
  previous = npc_caps;
  npc_caps = ([ ]);
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

    npc_caps[npc_paths[i]] = ([ "max": counts[npc_paths[i]] ]);

    // carry forward design-time flags recompute must not clobber
    if (previous[npc_paths[i]] && previous[npc_paths[i]]["resident"])
      npc_caps[npc_paths[i]]["resident"] = 1;
  }
}

// Record a converted location's NPC sources (its source room's add_clone
// counts, kept on the location as _original_add_clones) and recompute the area
// cap. Idempotent: reconverting a location overwrites only its own entry. This
// is the room2loc seed for the area's population.
void set_location_original_sources(string location_file, mapping clones)
{
  object owner;

  // The provenance is keyed by location file, so a community can hold the
  // entries of every area that delegates to it and derive one set of caps from
  // the lot. The sweep still registers this area by its own path: the caps are
  // shared, but the locations the filler is placed in are this area's.
  owner = (object)this_object()->query_root_area();
  if (owner != this_object())
  {
    owner->set_location_original_sources(location_file, clones);
    if (map_sizeof((mapping)owner->query_npc_caps()))
      POPULATION_HANDLER->include_area(
        (string)this_object()->query_area_path());
    return;
  }

  if (clones && map_sizeof(clones))
    original_npc_sources[location_file] = map_copy(clones);
  else
    map_delete(original_npc_sources, location_file);

  rebuild_npc_caps();

  // register with the population sweep so it keeps this area topped up
  if (map_sizeof(npc_caps))
    POPULATION_HANDLER->include_area((string)this_object()->query_area_path());

  this_object()->save_me();
}

// This area's own share of `source`, read off the conversion provenance: each
// of its locations whose room cloned that type, mapped to how many it cloned.
//
// The caps are shared across a community, so this is what keeps the filler
// where it came from -- pilgrims recorded on the road stay on the road, and the
// road tops up its own five rather than the whole community's thirteen -- even
// though the town they are counted with is the same community. Empty when the
// type was declared elsewhere, or added by hand with no provenance at all.
mapping query_original_clone_counts(string source)
{
  mapping provenance, mine, out;
  string * files;
  int i;

  provenance = (mapping)this_object()->query_npc_sources();
  mine = (mapping)this_object()->query_locations();
  files = map_indices(provenance);
  out = ([ ]);

  for (i = 0; i < sizeof(files); i++)
  {
    mapping clones;
    string * paths;
    int j;

    if (!mine[files[i]])
      continue;

    clones = provenance[files[i]];
    paths = map_indices(clones);
    for (j = 0; j < sizeof(paths); j++)
      if (query_template_from_source(paths[j]) == source)
      {
        out[files[i]] = clones[paths[j]];
        break;
      }
  }

  return out;
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
int * query_area_stats() { return ({ npc_stat_low, npc_stat_high }); }

// Set the range this area rolls its people's stats in, or clear it with a low
// of 0. Reversed bounds are taken in the order given rather than refused.
void set_area_stats(int low, int high)
{
  if (low < 1)
  {
    npc_stat_low = 0;
    npc_stat_high = 0;
  }
  else
  {
    npc_stat_low = low;
    npc_stat_high = high < low ? low : high;
  }

  this_object()->save_me();
}

// The type as this area hands it out: the bestiary template with the area's
// own stat range folded in, so an NPC born here is rolled to the strength of
// the place. A stat the type pins explicitly still wins -- apply_template
// applies those after the roll -- and an area with no range of its own hands
// the template back untouched.
mapping query_banded_template(string game, string source)
{
  mapping t;

  t = BESTIARY_HANDLER->query_template(game, source);
  if (!t || !npc_stat_low)
    return t;

  t = ([ ]) + t;
  t["random_stats"] = ([ "low": npc_stat_low, "high": npc_stat_high ]);

  return t;
}

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

// Re-roll the level of every NPC of this area's community that is below the
// band the area now hands out. A level is decided once and kept for the life of
// the NPC, so changing an area's band leaves the people who were already born
// under the old one behind: this is the deliberate correction for that, and the
// only thing that ever re-levels an NPC. Each one is materialized if it is not
// live, re-levelled and saved. Returns how many were touched.
int relevel_census()
{
  mapping census;
  string * ids;
  string game;
  int i, touched;

  census = (mapping)this_object()->query_npc_census();
  ids = map_indices(census);
  game = game_from_path((string)this_object()->query_area_path());

  for (i = 0; i < sizeof(ids); i++)
  {
    object npc, loc;
    string source;
    int want;

    source = census[ids[i]]["source"];
    if (!source)
      continue;

    npc = AREA_HANDLER->find_live_npc(ids[i]);
    if (!npc)
    {
      // wake it where the census says it is
      loc = (object)this_object()->load_location(census[ids[i]][CENSUS_LOCATION]);
      if (!loc)
        continue;

      this_object()->restore_one_npc(ids[i], loc);

      npc = AREA_HANDLER->find_live_npc(ids[i]);
      if (!npc)
        continue;
    }

    want = decide_level(game, source);
    if ((int)npc->query_level() >= want)
      continue;

    npc->set_level(want);
    npc->save_npc();
    touched++;
  }

  return touched;
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

