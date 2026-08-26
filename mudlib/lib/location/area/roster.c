
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
//     ([ uuid : ([ "source": npc_path, "location": location_file,
//                  "savefile": npc.o path ]) ])
// The census is the authoritative summary of the area's population; NPC
// objects are materialized into a location on load and drained on unload,
// but the census entry survives so the same NPC comes back.
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

void create()
{
  npc_caps = ([ ]);
  original_npc_sources = ([ ]);
  npc_default_level = 1;
  npc_default_level_spread = 0;
}

// Flag (or clear) an intended NPC source as a settled resident. This is the
// design-time switch that decides who `assign_homes` may house. Returns 0 if the
// source is not a known intended NPC of this area.
int set_npc_resident(string source, int flag)
{
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

  owner = (object)this_object()->query_population_area();
  return owner == this_object() ? npc_caps
                                : (mapping)owner->query_npc_caps();
}

void set_npc_caps(mapping m)
{
  npc_caps = m ? m : ([ ]);
  this_object()->save_me();
}

// Declare (or replace) an intended NPC for this area. `source` is the NPC's
// source path -- it is the identity (census key) and is snapshotted into a
// data template on first spawn. `max` is the area-wide population cap.
void add_intended_npc(string source, int max)
{
  npc_caps[source] = ([ "max": max ]);
  this_object()->save_me();
}

void remove_intended_npc(string source)
{
  map_delete(npc_caps, source);
  this_object()->save_me();
}

mapping query_npc_sources() { return original_npc_sources; }

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

