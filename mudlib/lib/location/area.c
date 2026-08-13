inherit "/lib/core/object.c";

#include <room/location.h>
#include <living/persisted.h>
#include <areas/area.h>
#include <areas/poi.h>
#include <areas/diplomacy.h>
#include <room/prop.h>
#include <namegen.h>

// mapping in the form ([ file_name : location_data ])
mapping locations;
// mapping in the form ([ file_name : ({ "direction", destination }) ])
mapping connections;
// array of loaded locations
static object * loaded_locations;
string file_name;
string area_path;
// exploration: whether entering this area grants an exploration achievement,
// and the display name recorded in the player's diary when it does.
int gives_exploration;
string exploration_name;

// Dynamic NPC population. Both are keyed by the NPC's source path (the
// hand-authored .c it is cloned from).
//   npc_intended: configuration -- what may spawn here and how many.
//     ([ npc_path : ([ "category": ({ ids }) or nil, "max": int ]) ])
//   (Fine-grained placement -- which location/POI, sector-type weighting --
//    is layered on in F2; F1 spawns anywhere in the area up to the cap.)
//   npc_census:   live state -- which concrete NPCs exist and where.
//     ([ uuid : ([ "source": npc_path, "location": location_file,
//                  "savefile": npc.o path ]) ])
// The census is the authoritative summary of the area's population; NPC
// objects are materialized into a location on load and drained on unload,
// but the census entry survives so the same NPC comes back.
mapping npc_intended;
mapping npc_census;
// Per-location NPC provenance from the room2loc conversion:
//   ([ location_file : ([ npc_path : count ]) ])
// This is the seed for npc_intended: the area cap for a source is the sum
// of that source's add_clone counts across every room of the area. Keeping
// it per location makes reconversion idempotent (a location overwrites only
// its own entry) without reloading the whole area.
mapping npc_sources;

// Points of interest, keyed by the location's file_name (a location holds
// at most one POI). See include/areas/poi.h for the entry shape. Venture
// POIs (pub, shop) are attached automatically at conversion; the rest are
// declared by hand with the builder ring. Vacancies hang off each POI.
mapping pois;

// Average level of the area's NPCs and how far individual NPCs may deviate
// from it. An NPC's level is decided once, at census assignment, as
//   npc_default_level + template level_area_modifier  ±  random(npc_default_level_spread + 1)
// (a template that carries a concrete "level" overrides all of this). The
// deviation uses random(spread + 1) because random(2) yields only 0 or 1, so
// a spread of 2 gives a swing of 0..2. Stored per NPC in the census, so each
// NPC keeps the level it was created with.
int npc_default_level;
int npc_default_level_spread;

// The settlement's role board (target model, dev/area-npc-system.md §7.2): the
// named jobs a town staffs with sentient citizens -- barman, mayor, guards,
// farmers -- each with a count and a real work location. A role slot is a
// census entry tagged "role" with no "poi"/"guard", so it does NOT auto-respawn
// like a POI vacancy: a slot emptied by death is refilled by the settlement
// pass (fill_area_roles), which the F6 prosperity tick will drive. For now the
// slots are filled from a transitional template `source`; the F4 generator
// replaces that behind assign_npc_to_role.
//   ([ role_name : ([ "count": n, "work": location_file,
//                     "source": template_id, "sentient": 1 ]) ])
mapping roles;

// The citizenship this area belongs to (a name in the diplomacy graph).
// Guards fielded at the area's town entrances and squares follow this
// citizenship: the diplomacy handler tells us how many (its security level)
// and which NPC source they spawn from (its guard). Change this and, on the
// next fill, the old guards are replaced by the new citizenship's -- this is
// how an invaded town swaps its guards for the conqueror's.
string citizenship;

// prototype functions
void add_loaded_location(object location);
mapping query_vacancy_sources();
private void _recompute_intended();
private object live_census_npc(string poi_file, string uuid);
private void _ensure_guards_assigned(string location_file);
void fill_guards();
void repost_guards(string poi_file);
private void _remove_guard(string id);


void create() {
  locations = ([ ]);
  loaded_locations = ({ });
  connections = ([ ]);
  file_name = "";
  area_path = "";
  gives_exploration = 0;
  exploration_name = "";
  npc_intended = ([ ]);
  npc_census = ([ ]);
  npc_sources = ([ ]);
  pois = ([ ]);
  roles = ([ ]);
  npc_default_level = 1;
  npc_default_level_spread = 0;
  citizenship = "";
  ::create();
}

void restore_me() {
  restore_object(file_name);
}

void save_me() {
  save_object(file_name);
}

string query_file_name() { return file_name; }
void set_file_name(string name)
{
  file_name = name;
  save_me();
}

mapping query_locations() { return locations; }

// Exploration achievement granted on entering this area (see the player's
// exploration component and the movement arrival hook).
int query_gives_exploration() { return gives_exploration; }
void set_gives_exploration(int flag)
{
  gives_exploration = flag;
  save_me();
}
string query_exploration_name() { return exploration_name; }
void set_exploration_name(string name)
{
  exploration_name = name;
  save_me();
}

// Filters the area's loaded locations down to those carrying the
// maze component. Derived, no separate storage — the maze flag lives
// on the location's component_info.
object * query_maze_locations()
{
  object * ret;
  int i;

  ret = ({ });
  loaded_locations -= ({ nil });
  for (i = 0; i < sizeof(loaded_locations); i++)
    if (loaded_locations[i]->query_component_by_type(LOCATION_COMPONENT_MAZE))
      ret += ({ loaded_locations[i] });

  return ret;
}

// The full save path of this area
// (e.g. /save/games/<game>/locations/areas/<area>/rooms/), set
// by the area handler when the storage is created.
string query_area_path() { return area_path; }

/**
 * Short identifier of the area: everything after the "areas/" segment
 * of the storage path, with the trailing "/" stripped.
 *
 * For "/save/games/<game>/locations/areas/<area>/rooms/" this
 * returns "<area>/rooms"; nested area trees (e.g. an inner-city
 * sub-area) keep their inner structure in the name. Falls back to the
 * full path if "areas/" is not present.
 */
string query_area_name()
{
  string * parts;
  int i;
  parts = explode(area_path, "/") - ({ "" });
  for (i = 0; i < sizeof(parts); i++)
    if (parts[i] == "areas" && i + 1 < sizeof(parts))
      return implode(parts[i + 1..], "/");
  return area_path;
}

// Set by the area handler when the storage is created. The argument
// is the full save path (/save/games/.../areas/<name>/...); the public
// "area name" (the short identifier) is derived from it.
void set_area_path(string path)
{
  area_path = path;
  // save_me();
}

// restore the area from a file
// returns 1 if the file exists and the location was restored, 0 otherwise
int restore_from_file_name(string name)
{
  if (file_size(name) >= 0)
  {
    restore_object(name);

    // Rebuild the derived roster from npc_sources on load (npc_intended is not
    // authoritative state, it is derived) and, in the same pass, fold any
    // pre-conversion monster paths in the persisted data down to template ids.
    // This lets a converted area self-heal on first access after a reboot,
    // without reloading any source .c.
    _recompute_intended();
    return 1;
  }

  return 0;
}

object load_location(string location_file_name)
{
  object location;
  int i;

  // check if this location is in this area
  if (member_array(location_file_name, keys(locations)) == -1)
    return nil;

  // removed already dest'ed locations
  loaded_locations -= ({ nil });

  // look through loaded_locations to see if it's already loaded
  for (i = 0; i < sizeof(loaded_locations); i++)
  {
    if (loaded_locations[i]->query_file_name() == location_file_name)
    {
      // write("🎃 location already loaded: " + location_file_name + "\n");
      return loaded_locations[i];
    }
  }

  location = clone_object(BASE_LOCATION_OBJ);

  if (!location)
    return nil;

  location->restore_from_file_name(location_file_name);
  loaded_locations += ({ location });

  return location;
}

void add_location(string location_file_name, mapping location_data) 
{
  locations[location_file_name] = map_copy(location_data);
  save_me();
}

void remove_location(string location_file_name) 
{
  map_delete(locations, location_file_name);
  save_me();
}

void add_loaded_location(object location) 
{
  int i;

  loaded_locations -= ({ nil });

  for (i = 0; i < sizeof(loaded_locations); i++)
  {
    // why are we having two objects for the same location?
    // don't know, don't care, the last one should be the good one
    if (loaded_locations[i]->query_file_name() == location->query_file_name())
    {
      loaded_locations[i] = location;
      return;
    }
  }

  loaded_locations += ({ location });
}

object * query_loaded_locations() 
{
  loaded_locations -= ({ nil });
  return loaded_locations;
}

mapping query_connections() { return connections; }
void add_connection(string location_file_name, string direction, string destination)
{
  if (!connections[location_file_name])
    connections[location_file_name] = ({ direction, destination });

  save_me();
}

// ---------------------------------------------------------------------------
// Dynamic NPC population
// ---------------------------------------------------------------------------

mapping query_npc_intended() { return npc_intended; }
mapping query_npc_census() { return npc_census; }

void set_npc_intended(mapping m)
{
  npc_intended = m ? m : ([ ]);
  save_me();
}

// Declare (or replace) an intended NPC for this area. `source` is the NPC's
// source path -- it is the identity (census key) and is snapshotted into a
// data template on first spawn. `categories` is an optional coarse-type list
// stamped on the NPC, `max` the area-wide population cap.
void add_intended_npc(string source, string * categories, int max)
{
  npc_intended[source] = ([ "category": categories, "max": max ]);
  save_me();
}

void remove_intended_npc(string source)
{
  map_delete(npc_intended, source);
  save_me();
}

mapping query_npc_sources() { return npc_sources; }

// Normalise a source to the template id the area keys everything by. A source
// arrives either already as a template id (post-conversion data) or as the
// original monster .c path (pre-conversion saves); template_id is idempotent,
// so both collapse to the same key and old saves keep working with no separate
// migration pass. Day-to-day operation never needs the .c to exist.
private string _template_id(string source)
{
  if (!source || !strlen(source))
    return source;
  return BESTIARY_HANDLER->template_id(game_from_path(area_path), source);
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
  locs = map_indices(pois);
  for (i = 0; i < sizeof(locs); i++)
  {
    mapping * vs;
    vs = pois[locs[i]][POI_FIELD_VACANCIES];
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
  ids = map_indices(npc_census);
  for (i = 0; i < sizeof(ids); i++)
  {
    mapping e;
    string tid;

    e = npc_census[ids[i]];
    tid = _template_id(e["source"]);
    if (tid != e["source"])
    {
      e["source"] = tid;
      changed = 1;
    }
  }

  if (changed)
    save_me();
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
private void _recompute_intended()
{
  string * location_files, * npc_paths;
  int i, j;
  mapping counts, clones_here, vacancy_sources;
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

  vacancy_sources = query_vacancy_sources();

  // the citizenship's guard NPC is placed by diplomacy at guarded POIs, so it
  // must not also be scattered by the population sweep as statistical filler
  guard_source = "";
  if (strlen(citizenship))
    guard_source = _template_id(
      DIPLOMACY_HANDLER->query_guard_path(game_from_path(area_path),
                                          citizenship));

  // npc_sources only ever holds living NPC sources: conversion filters trees
  // and props out (it loads each source once, keeps only query_monster ones)
  // before recording them, so the roster no longer re-loads the source .c to
  // re-check -- it just sums the counts.
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

    npc_intended[npc_paths[i]] = ([ "category": nil,
                                    "max": counts[npc_paths[i]] ]);
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

  _recompute_intended();

  // register with the population sweep so it keeps this area topped up
  if (map_sizeof(npc_intended))
    POPULATION_HANDLER->include_area(area_path);

  save_me();
}

// ---------------------------------------------------------------------------
// Points of interest and vacancies (see include/areas/poi.h)
// ---------------------------------------------------------------------------

mapping query_pois() { return pois; }

// The POI attached to a location, or nil. A location holds at most one.
mapping query_poi(string location_file)
{
  return pois[location_file];
}

int is_poi(string location_file)
{
  return !undefinedp(pois[location_file]);
}

// Attach (or replace) a POI on a location. `kind` must be one of POI_KINDS;
// `label` is optional display text. Re-attaching preserves the existing
// vacancies so a reconversion does not drop them.
void add_poi(string location_file, string kind, varargs string label)
{
  mapping entry;

  if (!location_file || !strlen(location_file))
    return;
  if (member_array(kind, POI_KINDS) < 0)
    return;

  entry = pois[location_file];
  if (!entry)
    entry = ([ POI_FIELD_VACANCIES: ({ }) ]);

  entry[POI_FIELD_KIND] = kind;
  if (label && strlen(label))
    entry[POI_FIELD_LABEL] = label;
  if (!entry[POI_FIELD_VACANCIES])
    entry[POI_FIELD_VACANCIES] = ({ });

  pois[location_file] = entry;
  save_me();
}

void remove_poi(string location_file)
{
  string * ids;
  int i;

  // a POI's guards belong to it: drop them (destructing any live) before the
  // POI itself goes, so removing it never leaves an orphan guard behind
  ids = map_indices(npc_census);
  for (i = 0; i < sizeof(ids); i++)
    if (npc_census[ids[i]]["guard"] &&
        npc_census[ids[i]]["poi"] == location_file)
      _remove_guard(ids[i]);

  map_delete(pois, location_file);
  save_me();
}

void set_poi_label(string location_file, string label)
{
  if (!pois[location_file])
    return;
  pois[location_file][POI_FIELD_LABEL] = label;
  save_me();
}

// The exit direction a town_entrance POI's guards watch (the way into the
// town). Only meaningful on a town_entrance; ignored by square guards.
void set_poi_guard_dir(string location_file, string dir)
{
  if (!pois[location_file])
    return;
  pois[location_file][POI_FIELD_GUARD_DIR] = dir;
  save_me();
}
string query_poi_guard_dir(string location_file)
{
  return pois[location_file] ? pois[location_file][POI_FIELD_GUARD_DIR] : nil;
}

// The vacancies of a POI, or an empty array when there is no POI there.
mapping * query_vacancies(string location_file)
{
  mapping entry;
  entry = pois[location_file];
  return entry ? entry[POI_FIELD_VACANCIES] : ({ });
}

// Declare a vacancy on a location's POI: a named `role` filled from a
// unique NPC `source`. Starts unfilled. No-op if the location
// has no POI or the role already exists.
void add_vacancy(string location_file, string role, string source)
{
  mapping entry;
  mapping * vs;
  int i;

  entry = pois[location_file];
  if (!entry || !role || !strlen(role) || !source || !strlen(source))
    return;

  vs = entry[POI_FIELD_VACANCIES];
  if (!vs) vs = ({ });
  for (i = 0; i < sizeof(vs); i++)
    if (vs[i][VACANCY_FIELD_ROLE] == role)
      return;

  // A vacancy is added by hand from an NPC blueprint path; build its template
  // now (the one time the source .c is loaded) and store the vacancy by its
  // template id, so filling it later never touches the .c.
  {
    string game;
    game = game_from_path(area_path);
    if (!BESTIARY_HANDLER->has_template(game, source))
      BESTIARY_HANDLER->add_template(source);
    source = _template_id(source);
  }

  vs += ({ ([ VACANCY_FIELD_ROLE:   role,
              VACANCY_FIELD_SOURCE: source,
              VACANCY_FIELD_UUID:   nil ]) });
  entry[POI_FIELD_VACANCIES] = vs;
  pois[location_file] = entry;

  // a vacancy source leaves the statistical roster
  _recompute_intended();
  save_me();
}

void remove_vacancy(string location_file, string role)
{
  mapping entry;
  mapping * vs, * out;
  int i;

  entry = pois[location_file];
  if (!entry) return;

  vs = entry[POI_FIELD_VACANCIES];
  if (!vs) return;

  // drop the census NPC that fills this vacancy (destruct any live copy and
  // delete its savefile) so removing the vacancy leaves no orphan that would
  // re-materialize on the next load -- mirrors remove_poi culling its guards
  {
    string * ids;
    int j;

    ids = map_indices(npc_census);
    for (j = 0; j < sizeof(ids); j++)
    {
      mapping e;
      object npc;

      e = npc_census[ids[j]];
      if (e["role"] == role && e["poi"] == location_file)
      {
        npc = live_census_npc(location_file, ids[j]);
        if (npc)
          npc->dest_me();
        if (e["savefile"] && file_size(e["savefile"]) >= 0)
          remove_file(e["savefile"]);
        map_delete(npc_census, ids[j]);
      }
    }
  }

  out = ({ });
  for (i = 0; i < sizeof(vs); i++)
    if (vs[i][VACANCY_FIELD_ROLE] != role)
      out += ({ vs[i] });

  entry[POI_FIELD_VACANCIES] = out;
  pois[location_file] = entry;
  _recompute_intended();
  save_me();
}

// Record which concrete NPC (uuid) currently fills a vacancy role, or clear
// it (uuid nil) when the NPC dies so the fill pass respawns it.
void set_vacancy_uuid(string location_file, string role, string uuid)
{
  mapping entry;
  mapping * vs;
  int i;

  entry = pois[location_file];
  if (!entry) return;

  vs = entry[POI_FIELD_VACANCIES];
  for (i = 0; vs && i < sizeof(vs); i++)
    if (vs[i][VACANCY_FIELD_ROLE] == role)
    {
      vs[i][VACANCY_FIELD_UUID] = uuid;
      save_me();
      return;
    }
}

// The set of NPC sources claimed by a vacancy anywhere in the area,
// as ([ source : 1 ]). Used by _recompute_intended to keep vacancy uniques
// out of the statistical population.
mapping query_vacancy_sources()
{
  mapping ret;
  string * locs;
  int i, j;

  ret = ([ ]);
  locs = map_indices(pois);
  for (i = 0; i < sizeof(locs); i++)
  {
    mapping * vs;
    vs = pois[locs[i]][POI_FIELD_VACANCIES];
    for (j = 0; vs && j < sizeof(vs); j++)
      ret[_template_id(vs[j][VACANCY_FIELD_SOURCE])] = 1;
  }

  return ret;
}

// Live census count of a given source across the whole area (materialized or
// not) -- this is L_b, checked against the area cap C_b.
private int npc_live_count(string source)
{
  string * ids, want;
  int i, n;

  want = _template_id(source);
  ids = map_indices(npc_census);
  for (i = 0; i < sizeof(ids); i++)
    if (_template_id(npc_census[ids[i]]["source"]) == want)
      n++;

  return n;
}

// Census entries assigned to a given location file.
private string * npc_census_for_location(string location_file)
{
  string * ids, * ret;
  int i;

  ids = map_indices(npc_census);
  ret = ({ });
  for (i = 0; i < sizeof(ids); i++)
    if (npc_census[ids[i]]["location"] == location_file)
      ret += ({ ids[i] });

  return ret;
}

// Is `uuid` already materialized inside `loc`? Non-NPC contents answer nil to
// query_npc_uuid (DGD call_other to an undefined function returns nil).
private int npc_uuid_present(object loc, string uuid)
{
  object * inv;
  int i;

  inv = all_inventory(loc);
  for (i = 0; i < sizeof(inv); i++)
    if (inv[i] && inv[i]->query_npc_uuid() == uuid)
      return 1;

  return 0;
}

// The gender a census NPC is born with, decided once at assignment so it stays
// stable across saves and restores. A fixed template dictates it; a bimodal
// one (no "gender" key) rolls male/female here. Stored in the census entry and
// handed to the NPC before its template is applied, so the per-gender strings
// match.
private int decide_gender(string game, string source)
{
  return BESTIARY_HANDLER->roll_gender(
           BESTIARY_HANDLER->query_template(game, source));
}

int query_area_level() { return npc_default_level; }
int query_area_spread() { return npc_default_level_spread; }

void set_area_level(int n)
{
  npc_default_level = n < 1 ? 1 : n;
  save_me();
}

void set_area_spread(int n)
{
  npc_default_level_spread = n < 0 ? 0 : n;
  save_me();
}

// The area's citizenship (a diplomacy-graph name), or "" if none. Guards are
// fielded from it; see the guard fill below.
string query_citizenship() { return citizenship; }
void set_citizenship(string name)
{
  string * locs;
  int i;

  citizenship = name ? name : "";
  save_me();

  // which guard source the roster excludes depends on the citizenship, so
  // recompute and persist it
  _recompute_intended();
  save_me();

  // re-post guards at every guarded POI: drop the old citizenship's guards and
  // field the new one's. This is the invasion path -- flip the citizenship and
  // the guards become the conqueror's.
  locs = map_indices(pois);
  for (i = 0; i < sizeof(locs); i++)
    repost_guards(locs[i]);
}

// The object path of the area's citizenship social object, or "" if unset.
// Guards carry this as their city_ob so diplomacy can resolve their loyalty.
string query_citizenship_path()
{
  string game;
  if (!strlen(citizenship))
    return "";
  game = game_from_path(area_path);
  return "/games/" + game + "/obj/citizenships/" + citizenship;
}

// The level a census NPC is born with, decided once at assignment so it stays
// stable for the life of that NPC (like its gender). A template with a
// concrete "level" dictates it outright; otherwise the level derives from the
// area: npc_default_level + the template's level_area_modifier, swung by up to the
// area spread (random(spread + 1), sign random). Never below 1.
private int decide_level(string game, string source)
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

// Assign a new NPC of `source` to `location_file` as data only: ensure the
// source has a data template and record a census entry. No object is
// materialized -- it becomes real (cloned from the template and saved) when
// the location loads (npc_restore). This is what the population sweep calls to
// scatter NPCs across the area without loading any location. Returns the uuid.
string assign_npc(string source, string location_file)
{
  string id, game;

  if (!npc_intended[source])
    return nil;

  // source is a template id; its template was built at conversion time, so
  // nothing here loads the original monster .c
  game = game_from_path(area_path);

  id = UUID_OB->uuid();
  npc_census[id] = ([ "source": source, "location": location_file,
                      "savefile": npc_save_dir(game, id) + NPC_SAVE_FILE,
                      "gender": decide_gender(game, source),
                      "level": decide_level(game, source) ]);
  save_me();

  return id;
}

// Assign a vacancy NPC: like assign_npc, but the source is a unique bound to
// a POI (not part of the statistical roster, so no npc_intended check), and
// the census entry is tagged with the owning POI location and role. Returns
// the uuid. The NPC materializes when the POI's location loads.
private string assign_vacancy_npc(string source, string location_file,
                                  string role)
{
  string id, game;

  // source is a template id (add_vacancy built its template); no monster .c
  game = game_from_path(area_path);

  id = UUID_OB->uuid();
  npc_census[id] = ([ "source": source, "location": location_file,
                      "savefile": npc_save_dir(game, id) + NPC_SAVE_FILE,
                      "gender": decide_gender(game, source),
                      "level": decide_level(game, source),
                      "poi": location_file, "role": role ]);
  save_me();

  return id;
}

// Ensure every vacancy of the POI at `location_file` has a census NPC
// assigned to that location. A vacancy is (re)assigned when it is empty or
// when its recorded NPC is no longer in the census (it died). Data-only:
// materialization happens in the normal restore loop.
private void _ensure_vacancies_assigned(string location_file)
{
  mapping entry;
  mapping * vs;
  int i;
  int changed;

  entry = pois[location_file];
  if (!entry)
    return;

  vs = entry[POI_FIELD_VACANCIES];
  changed = 0;
  for (i = 0; vs && i < sizeof(vs); i++)
  {
    string uuid;

    uuid = vs[i][VACANCY_FIELD_UUID];
    if (uuid && npc_census[uuid])
      continue;   // already filled and alive

    vs[i][VACANCY_FIELD_UUID] =
      assign_vacancy_npc(vs[i][VACANCY_FIELD_SOURCE], location_file,
                         vs[i][VACANCY_FIELD_ROLE]);
    changed = 1;
  }

  if (changed)
    save_me();
}

// Materialize a census NPC into `loc`: a generic NPC with the source's data
// template applied. If it already has a savefile (it was live before), restore
// its state on top; otherwise this is its first materialization (freshly
// assigned by the population sweep) and we save it so its state persists.
// The template's own name for a gender (a GENDER_* id): its "trade" word (e.g.
// "granjero"), used as an alias on a generated citizen. A fixed-gender template
// stores a plain string; a bimodal one a per-gender mapping keyed by the gender
// id as a string (matching how the bestiary stores per-gender fields).
private string _template_kind(mapping t, int gender)
{
  mixed v;

  if (!t)
    return nil;
  v = t["name"];
  if (stringp(v))
    return v;
  if (mappingp(v))
    return v["" + gender];
  return nil;
}

private object npc_restore(string id, object loc)
{
  object npc;
  string game, source, savefile;
  mapping spec, entry;

  entry = npc_census[id];
  game = game_from_path(area_path);

  // Converge the census on template ids: an entry saved before conversion holds
  // the original monster path -- normalise it once and backfill so later reads
  // are already ids. query_template accepts either, so this is safe mid-life.
  source = _template_id(entry["source"]);
  if (source != entry["source"])
  {
    entry["source"] = source;
    npc_census[id] = entry;
    save_me();
  }

  // a guard census entry clones the guard base (generic NPC + guardian role)
  // so the placed NPC gains the exit check; everything else is identical.
  npc = clone_object(entry["guard"] ? GUARD_NPC : GENERIC_NPC);
  if (!npc)
    return nil;

  npc->set_npc_uuid(id);
  npc->set_npc_game(game);
  npc->set_npc_area_path(area_path);
  npc->set_npc_source(source);
  if (entry["poi"])
    npc->set_npc_poi(entry["poi"]);

  // Fix the census gender before applying the template so a bimodal template
  // picks the matching per-gender strings. Older census entries predate the
  // stored gender -- decide and backfill one so they stay stable from now on.
  if (!entry["gender"])
  {
    entry["gender"] = decide_gender(game, source);
    npc_census[id] = entry;
    save_me();
  }
  npc->set_gender(entry["gender"]);

  // A sentient role slot is a named individual. Name it BEFORE the template
  // runs: monster::set_name takes only the first name (it seeds the NPC's
  // living_name and refuses later renames), so setting our generated name here
  // makes it the real name and the template's generic set_name becomes a no-op.
  // The name is stored lowercase (matching / living_name); the short is put
  // back to the capitalized name after the template overwrites it below.
  if (entry["name"])
    npc->set_name(lower_case(entry["name"]));

  npc->apply_template(BESTIARY_HANDLER->query_template(game, source));

  // The level is decided once at assignment and stored in the census, so an
  // NPC keeps the level it was created with (independent of later changes to
  // the area's average). Older census entries predate the stored level --
  // decide and backfill one so they stay stable from now on.
  if (!entry["level"])
  {
    entry["level"] = decide_level(game, source);
    npc_census[id] = entry;
    save_me();
  }
  npc->set_level(entry["level"]);

  // A sentient role slot is a named individual: it was named before the
  // template ran (see above), so the template kept the body/description but not
  // the name. Finish the individual here:
  //   - the short: the template overwrote it with its generic one, so put the
  //     individual's name back as the capitalized short;
  //   - the gender: a single-gender template forces its own gender in
  //     apply_template, so re-assert the census gender (which the citizen rolled
  //     for itself) so a female citizen is not turned male by a male template;
  //   - the aliases: keep the template's own kind word as an alias (on top of
  //     the template's aliases that apply_template already set), so the citizen
  //     still answers to its trade ("kill farmer" as well as "kill Lothadric").
  if (entry["name"])
  {
    string kind;

    npc->set_short(entry["name"]);
    npc->set_gender(entry["gender"]);
    kind = _template_kind(BESTIARY_HANDLER->query_template(game, source),
                          entry["gender"]);
    if (kind && strlen(kind))
      npc->add_alias(kind);
  }

  spec = npc_intended[source];
  if (spec && spec["category"])
    npc->set_npc_categories(spec["category"]);

  savefile = entry["savefile"];
  if (savefile && file_size(savefile) >= 0)
    npc->restore_npc();
  else
    npc->save_npc();

  npc->move(loc);

  // A guard carries the area's citizenship as its city_ob (so diplomacy can
  // resolve its loyalty) and, at an entrance, watches the entry direction: it
  // registers on that exit so the exit handler consults its guardian_check.
  // Square guards just stand there (no direction). Both are derived here from
  // the live POI / area rather than the census, so a change to guard_dir or the
  // citizenship is picked up the next time the guard materializes; none of it
  // persists on the NPC.
  if (entry["guard"])
  {
    string cpath, gdir;
    mapping poi;

    cpath = query_citizenship_path();
    if (strlen(cpath))
      npc->set_city_ob(cpath);

    // only an entrance guard watches a direction; a square guard is presence
    // only, so it never registers on an exit even if a stale guard_dir lingers
    poi = pois[entry["poi"]];
    gdir = (poi && poi[POI_FIELD_KIND] == POI_KIND_TOWN_ENTRANCE)
             ? poi[POI_FIELD_GUARD_DIR] : nil;
    if (gdir)
    {
      npc->set_guardian_direction(gdir);
      loc->register_guard(npc, gdir);
    }
  }

  return npc;
}

// Called when a location of this area loads (prewarm / movement): bring back
// exactly the NPCs the census says live here -- the ones that were in the
// location when it last unloaded. Restore-only: no new NPCs are created here.
// Idempotent (already-present uuids are left alone).
void restore_location_npcs(object loc)
{
  string file;
  string * ids;
  int i;

  if (!loc)
    return;

  file = loc->query_file_name();
  if (!file || !strlen(file))
    return;

  // A POI location fills its vacancies first: assign a census NPC to any
  // empty or dead vacancy slot, so the materialize loop below brings it in
  // alongside the location's regular census NPCs. A guarded POI (town entrance
  // or square) likewise tops up its citizenship's guards.
  _ensure_vacancies_assigned(file);
  _ensure_guards_assigned(file);

  ids = npc_census_for_location(file);
  for (i = 0; i < sizeof(ids); i++)
    if (!npc_uuid_present(loc, ids[i]))
      npc_restore(ids[i], loc);
}

// Live census count of a source across the area (cap-check for the population
// sweep): L_b, checked against the cap C_b (query_npc_intended()[source].max).
int query_npc_live_count(string source) { return npc_live_count(source); }

// Called from a location's dest_me before its contents are torn down: persist
// each of our NPCs so its state survives the unload. The census entry stays,
// so restore_location_npcs brings the NPC back on the next load.
void drain_location(object loc)
{
  object * inv;
  int i;

  if (!loc)
    return;

  inv = all_inventory(loc);
  for (i = 0; i < sizeof(inv); i++)
    if (inv[i] && inv[i]->query_persisted() &&
        inv[i]->query_npc_area_path() == area_path)
      inv[i]->save_npc();
}

// Find the loaded location object for a file, or nil if it is not resident.
private object loaded_location(string file)
{
  object * locs;
  int i;

  locs = query_loaded_locations();
  for (i = 0; i < sizeof(locs); i++)
    if (locs[i] && locs[i]->query_file_name() == file)
      return locs[i];

  return nil;
}

// If `uuid` filled a vacancy, clear that slot and return ({ file, role });
// otherwise nil. Used on death so the vacancy can be refilled.
private mixed * clear_vacancy_by_uuid(string uuid)
{
  string * locs;
  int i, j;

  locs = map_indices(pois);
  for (i = 0; i < sizeof(locs); i++)
  {
    mapping * vs;
    vs = pois[locs[i]][POI_FIELD_VACANCIES];
    for (j = 0; vs && j < sizeof(vs); j++)
      if (vs[j][VACANCY_FIELD_UUID] == uuid)
      {
        vs[j][VACANCY_FIELD_UUID] = nil;
        save_me();
        return ({ locs[i], vs[j][VACANCY_FIELD_ROLE] });
      }
  }

  return nil;
}

// call_out target: respawn a vacancy NPC some time after its holder died.
// Re-assigns the slot and, if the POI's location is loaded, materializes the
// NPC at once; otherwise it comes back the next time the location loads.
void _refill_vacancy(string file)
{
  object loc;

  if (!pois[file])
    return;

  _ensure_vacancies_assigned(file);

  loc = loaded_location(file);
  if (loc)
    restore_location_npcs(loc);
}

// Called by a persisted NPC (via monster::do_death) when it dies: drop its
// census slot so the population frees up and a replacement may spawn later.
// The savefile itself is removed by the NPC's own delete_npc_save().
//
// If the dead NPC filled a vacancy, free the slot and schedule a delayed
// respawn at its POI.
void npc_died(string uuid)
{
  mixed * vacancy;
  mapping entry;
  string guard_poi;

  // note a fallen guard's POI before we drop the census entry, so we can
  // re-post a replacement there after the cooldown
  entry = npc_census[uuid];
  guard_poi = (entry && entry["guard"]) ? entry["poi"] : nil;

  if (entry)
  {
    map_delete(npc_census, uuid);
    save_me();
  }

  vacancy = clear_vacancy_by_uuid(uuid);
  if (vacancy)
    call_out("_refill_vacancy", VACANCY_RESPAWN_DELAY, vacancy[0]);

  if (guard_poi)
    call_out("_refill_guards", VACANCY_RESPAWN_DELAY, guard_poi);
}

// Ensure every POI vacancy in the area is assigned and, where the location
// is loaded, materialized. Safe to call repeatedly (idempotent per slot).
void fill_vacancies()
{
  string * locs;
  int i;

  locs = map_indices(pois);
  for (i = 0; i < sizeof(locs); i++)
  {
    object loc;

    _ensure_vacancies_assigned(locs[i]);
    loc = loaded_location(locs[i]);
    if (loc)
      restore_location_npcs(loc);
  }
}

// --- guards --------------------------------------------------------------
// Guards are census NPCs a citizenship fields at its town entrances and
// squares. Unlike a vacancy (a single named role), a guarded POI wants a
// count -- the citizenship's security level -- of identical guards, all from
// the citizenship's guard NPC source. They are data-only like every census
// NPC: they materialize when the POI's location loads (npc_restore clones the
// guard base, stamps the citizenship and, at an entrance, registers the exit).

// One guard assigned to a POI as census data. The guard's identity is its
// source; its watched direction and citizenship are not stored -- they are
// derived at materialization from the POI's guard_dir and the area's current
// citizenship, so a later change to either is picked up without rewriting the
// census. Returns the uuid.
private string assign_guard_npc(string source, string poi_file)
{
  string id, game;

  // The guard source is a diplomacy-placed unique, not part of room conversion,
  // so its template is built here the first time one is fielded (the one place
  // a guard's monster .c is loaded, guarded by has_template so it happens once).
  // The census then stores the template id, and filling/refilling never reloads
  // the .c.
  game = game_from_path(area_path);
  if (!BESTIARY_HANDLER->has_template(game, source))
    BESTIARY_HANDLER->add_template(source);
  source = _template_id(source);

  id = UUID_OB->uuid();
  npc_census[id] = ([ "source": source, "location": poi_file,
                      "savefile": npc_save_dir(game, id) + NPC_SAVE_FILE,
                      "gender": decide_gender(game, source),
                      "level": decide_level(game, source),
                      "poi": poi_file, "guard": 1 ]);
  save_me();

  return id;
}

// Guard census ids at a POI whose source matches `source`. A citizenship
// change swaps the source, so guards from the old one no longer match and are
// treated as stale by the reconcile below.
private string * guard_census_at(string poi_file, string source)
{
  string * ids, * out, want;
  int i;

  want = _template_id(source);
  ids = map_indices(npc_census);
  out = ({ });
  for (i = 0; i < sizeof(ids); i++)
  {
    mapping e;
    e = npc_census[ids[i]];
    if (e["guard"] && e["poi"] == poi_file &&
        _template_id(e["source"]) == want)
      out += ({ ids[i] });
  }
  return out;
}

// The live object for a census uuid inside its (loaded) POI, or nil. Works for
// any census NPC (a guard or a vacancy unique), matched by its uuid.
private object live_census_npc(string poi_file, string uuid)
{
  object loc;
  object * inv;
  int i;

  loc = loaded_location(poi_file);
  if (!loc)
    return nil;
  inv = all_inventory(loc);
  for (i = 0; i < sizeof(inv); i++)
    if (inv[i] && inv[i]->query_npc_uuid() == uuid)
      return inv[i];
  return nil;
}

// Drop a guard: remove its census entry first (so a death callback becomes a
// no-op), then destruct the live object if it is materialized, and delete its
// leftover savefile.
private void _remove_guard(string id)
{
  mapping e;
  object npc;

  e = npc_census[id];
  if (!e)
    return;

  map_delete(npc_census, id);
  save_me();

  npc = live_census_npc(e["poi"], id);
  if (npc)
    npc->dest_me();

  if (e["savefile"] && file_size(e["savefile"]) >= 0)
    remove_file(e["savefile"]);
}

// Reconcile the guards of the POI at `location_file`: if it is a guarded kind,
// hold exactly the area citizenship's security level of guards, all from the
// citizenship's current guard source. Data-only; materialization happens in
// the restore loop. Guards from a stale source (after an invasion) or beyond
// the wanted count are removed, and the shortfall topped up.
private void _ensure_guards_assigned(string location_file)
{
  mapping entry;
  string kind, game, source;
  string * live, * ids;
  int want, have, i;
  object dh;

  entry = pois[location_file];
  if (!entry)
    return;
  kind = entry[POI_FIELD_KIND];
  if (kind != POI_KIND_TOWN_ENTRANCE && kind != POI_KIND_TOWN_SQUARE)
    return;

  // the area's citizenship supplies both the count (security) and the NPC
  // source (guard); with no citizenship, or none configured, field no guards
  game = game_from_path(area_path);
  dh = load_object(DIPLOMACY_HANDLER);
  source = strlen(citizenship) ? dh->query_guard_path(game, citizenship) : "";
  want = strlen(source) ? dh->query_security_level(game, citizenship) : 0;

  // guards already here from the current source
  live = strlen(source) ? guard_census_at(location_file, source) : ({ });
  have = sizeof(live);

  // cull every guard at this POI that is not a current-source guard (stale
  // source after a citizenship change)
  ids = map_indices(npc_census);
  for (i = 0; i < sizeof(ids); i++)
  {
    mapping e;
    e = npc_census[ids[i]];
    if (e["guard"] && e["poi"] == location_file &&
        member_array(ids[i], live) == -1)
      _remove_guard(ids[i]);
  }

  // trim current-source guards down if security dropped
  for (i = want; i < have; i++)
    _remove_guard(live[i]);

  // top up to the wanted count from the current source
  for (i = have; i < want; i++)
    assign_guard_npc(source, location_file);
}

// call_out target: re-post a guard some time after one died at `file`.
void _refill_guards(string file)
{
  object loc;

  if (!pois[file])
    return;

  _ensure_guards_assigned(file);

  loc = loaded_location(file);
  if (loc)
    restore_location_npcs(loc);
}

// Ensure every guarded POI in the area holds its citizenship's guards and,
// where the location is loaded, materialize them. Safe to call repeatedly.
void fill_guards()
{
  string * locs;
  int i;

  locs = map_indices(pois);
  for (i = 0; i < sizeof(locs); i++)
  {
    object loc;

    _ensure_guards_assigned(locs[i]);
    loc = loaded_location(locs[i]);
    if (loc)
      restore_location_npcs(loc);
  }
}

// Rebuild the guards of a POI from the current citizenship / POI config: drop
// the ones there now (destructing any live) and re-derive. Callers use it after
// editing the citizenship or a guard_dir, so live guards reflect the config
// without a reload.
void repost_guards(string poi_file)
{
  string * ids;
  int i;
  object loc;

  if (!pois[poi_file])
    return;

  // remove every guard currently assigned here, whatever its source
  ids = map_indices(npc_census);
  for (i = 0; i < sizeof(ids); i++)
  {
    mapping e;
    e = npc_census[ids[i]];
    if (e["guard"] && e["poi"] == poi_file)
      _remove_guard(ids[i]);
  }

  _ensure_guards_assigned(poi_file);

  loc = loaded_location(poi_file);
  if (loc)
    restore_location_npcs(loc);
}

// ---------------------------------------------------------------------------
// Role board (target model, dev/area-npc-system.md §7.2)
// ---------------------------------------------------------------------------
//
// A role is a named job (barman, mayor, guard, farmer) the settlement staffs
// with sentient citizens: a count and a real work location. A role slot is a
// census entry tagged "role" with no "poi"/"guard" -- so it never auto-respawns
// like a POI vacancy; a slot emptied by death is refilled by fill_area_roles
// (the settlement pass). Slots are filled from a transitional template `source`
// for now; the F4 generator will replace assign_npc_to_role's body, the single
// seam where the filler lives.

mapping query_roles() { return roles; }
mapping query_role(string name) { return roles[name]; }

// Declare (or replace) a role. `count` is how many of it the settlement wants,
// `work` its work location, `source` the transitional NPC blueprint filling its
// slots (snapshotted to a template now, the one time its .c is read), `sentient`
// whether it is a named citizen (no clone-respawn). Stores the template id.
void add_role(string name, int count, string work, string source, int sentient)
{
  if (!name || !strlen(name) || count < 0)
    return;

  if (source && strlen(source))
  {
    string game;
    game = game_from_path(area_path);
    if (!BESTIARY_HANDLER->has_template(game, source))
      BESTIARY_HANDLER->add_template(source);
    source = _template_id(source);
  }

  roles[name] = ([ "count":    count,
                   "work":     work,
                   "source":   source,
                   "sentient": sentient ? 1 : 0 ]);
  save_me();
}

// Live count of a role's staff: census entries tagged with this role that are
// role-board slots (no POI, no guard), so this never counts a POI vacancy or a
// guard that happens to share the role name.
int count_role_npcs(string name)
{
  string * ids;
  int i, n;

  ids = map_indices(npc_census);
  for (i = 0; i < sizeof(ids); i++)
  {
    mapping e;
    e = npc_census[ids[i]];
    if (e["role"] == name && !e["poi"] && !e["guard"])
      n++;
  }

  return n;
}

// A generated given-name for a sentient role slot: the name generator draws it
// from the area citizenship's name style, in the form matching the slot's
// gender (a GENDER_* id). Returns nil when the area has no citizenship, the
// citizenship declares no name style, or the generator has no wordlist for it
// -- the slot then keeps its template's name. Decided once at assignment and
// stored, so the individual materializes as the same person every time.
private string generate_role_name(int gender)
{
  string cpath, style, word;
  object cit;

  cpath = query_citizenship_path();
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

// Record one census NPC for a role, at its work location. Data-only -- it
// materializes when the work location loads (npc_restore). This is the seam
// the F4 generator plugs into: a sentient slot is given a generated name here
// (the rest of its body still comes from the transitional template); a
// non-sentient slot is a plain template clone. Returns the uuid.
private string assign_npc_to_role(string name, mapping role)
{
  string id, game, source, work, gname;
  int gender;

  source = role["source"];
  work = role["work"];
  if (!source || !strlen(source) || !work || !strlen(work))
    return nil;

  game = game_from_path(area_path);
  id = UUID_OB->uuid();

  // A sentient citizen rolls its own gender (a fair coin), independent of the
  // transitional template's -- otherwise a single-gender template (a male
  // farmer.c) would make every citizen male. Fauna/vacancies keep the
  // template-driven gender.
  gender = role["sentient"]
             ? (random(2) ? GENDER_FEMALE : GENDER_MALE)
             : decide_gender(game, source);

  npc_census[id] = ([ "source":   source,
                      "location": work,
                      "savefile": npc_save_dir(game, id) + NPC_SAVE_FILE,
                      "gender":   gender,
                      "level":    decide_level(game, source),
                      "role":     name ]);

  // a named citizen: generate a given-name now and store it, so the NPC (and
  // every later re-materialization of this slot) is the same individual
  if (role["sentient"])
  {
    gname = generate_role_name(gender);
    if (gname)
      npc_census[id]["name"] = gname;
  }

  save_me();

  return id;
}

// Staff a role up to its count: add census slots until the live count reaches
// it. A slot emptied by a death (role NPCs do not auto-respawn) is refilled
// here, not instantly at the point of death.
void fill_role(string name)
{
  mapping role;
  object loc;
  int have, want, i;

  role = roles[name];
  if (!role)
    return;

  want = role["count"];
  have = count_role_npcs(name);
  for (i = have; i < want; i++)
    assign_npc_to_role(name, role);

  // if the work location is already resident, materialize the new slots now
  // (mirrors the vacancy refill); otherwise they come in when it next loads
  loc = loaded_location(role["work"]);
  if (loc)
    restore_location_npcs(loc);
}

// Staff every role in the area -- the settlement pass. Idempotent: a role
// already at its count adds nothing.
void fill_area_roles()
{
  string * names;
  int i;

  names = map_indices(roles);
  for (i = 0; i < sizeof(names); i++)
    fill_role(names[i]);
}

// Drop a role: cull its census NPCs (destruct any live, delete their savefiles)
// and forget the role, so removing it leaves no orphan behind.
void remove_role(string name)
{
  string * ids;
  int i;

  if (!roles[name])
    return;

  ids = map_indices(npc_census);
  for (i = 0; i < sizeof(ids); i++)
  {
    mapping e;
    object npc;

    e = npc_census[ids[i]];
    if (e["role"] == name && !e["poi"] && !e["guard"])
    {
      npc = live_census_npc(e["location"], ids[i]);
      if (npc)
        npc->dest_me();
      if (e["savefile"] && file_size(e["savefile"]) >= 0)
        remove_file(e["savefile"]);
      map_delete(npc_census, ids[i]);
    }
  }

  map_delete(roles, name);
  save_me();
}
