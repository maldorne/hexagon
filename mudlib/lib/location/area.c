inherit "/lib/core/object.c";

#include <room/location.h>
#include <living/persisted.h>
#include <areas/area.h>
#include <areas/poi.h>

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

// Dynamic NPC population (see dev/area-npc-system.md). Both are keyed by the
// NPC's blueprint path (its "source").
//   npc_intended: configuration -- what may spawn here and how many.
//     ([ blueprint_path : ([ "category": ({ ids }) or nil, "max": int ]) ])
//   (Fine-grained placement -- which location/POI, sector-type weighting --
//    is layered on in F2; F1 spawns anywhere in the area up to the cap.)
//   npc_census:   live state -- which concrete NPCs exist and where.
//     ([ uuid : ([ "source": blueprint_path, "location": location_file,
//                  "savefile": npc.o path ]) ])
// The census is the authoritative summary of the area's population; NPC
// objects are materialized into a location on load and drained on unload,
// but the census entry survives so the same NPC comes back.
mapping npc_intended;
mapping npc_census;
// Per-location NPC provenance from the room2loc conversion:
//   ([ location_file : ([ blueprint_path : count ]) ])
// This is the seed for npc_intended: the area cap for a blueprint is the sum
// of that blueprint's add_clone counts across every room of the area. Keeping
// it per location makes reconversion idempotent (a location overwrites only
// its own entry) without reloading the whole area.
mapping npc_sources;

// Points of interest, keyed by the location's file_name (a location holds
// at most one POI). See include/areas/poi.h for the entry shape. Venture
// POIs (pub, shop) are attached automatically at conversion; the rest are
// declared by hand with the builder ring. Vacancies hang off each POI.
mapping pois;

// prototype functions
void add_loaded_location(object location);
mapping query_vacancy_sources();


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
// (e.g. /save/games/rl-aeternum/locations/areas/elfereth/rooms/), set
// by the area handler when the storage is created.
string query_area_path() { return area_path; }

/**
 * Short identifier of the area: everything after the "areas/" segment
 * of the storage path, with the trailing "/" stripped.
 *
 * For "/save/games/rl-aeternum/locations/areas/elfereth/rooms/" this
 * returns "elfereth/rooms"; nested area trees (e.g. an inner-city
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
// blueprint path -- it is the identity (census key) and is snapshotted into a
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

// Recompute npc_intended from the per-location conversion provenance: the
// area cap for a blueprint is the sum of its add_clone counts across every
// room of the area.
//
// Two sources are deliberately kept out of the statistical roster: a
// blueprint claimed by a vacancy (a unique the POI system places by hand,
// not the population sweep) and anything that is not a living blueprint
// (add_clone is also used for trees and props, which are not NPCs). Both
// exclusions are re-applied here so a reconversion cannot resurrect a
// vacancy NPC or item cruft into the population -- the bug that made
// remove_intended_npc non-durable.
private void _recompute_intended()
{
  string * locs, * blueprints;
  int i, j;
  mapping totals, clones, vacancy_sources;

  totals = ([ ]);
  locs = map_indices(npc_sources);
  for (i = 0; i < sizeof(locs); i++)
  {
    clones = npc_sources[locs[i]];
    blueprints = map_indices(clones);
    for (j = 0; j < sizeof(blueprints); j++)
      totals[blueprints[j]] =
        (totals[blueprints[j]] ? totals[blueprints[j]] : 0) +
        clones[blueprints[j]];
  }

  vacancy_sources = query_vacancy_sources();

  npc_intended = ([ ]);
  blueprints = map_indices(totals);
  for (i = 0; i < sizeof(blueprints); i++)
  {
    object bp;

    // a unique bound to a vacancy is placed by the POI system, never by
    // the population sweep
    if (vacancy_sources[blueprints[i]])
      continue;

    // only living blueprints count towards the NPC population; skip trees,
    // props and other non-living add_clone sources
    bp = nil;
    catch(bp = load_object(blueprints[i]));
    if (!bp || !bp->query_monster())
      continue;

    npc_intended[blueprints[i]] = ([ "category": nil,
                                     "max": totals[blueprints[i]] ]);
  }
}

// Record a converted location's NPC blueprints (its source room's add_clone
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

// The vacancies of a POI, or an empty array when there is no POI there.
mapping * query_vacancies(string location_file)
{
  mapping entry;
  entry = pois[location_file];
  return entry ? entry[POI_FIELD_VACANCIES] : ({ });
}

// Declare a vacancy on a location's POI: a named `role` filled from a
// unique NPC blueprint `source`. Starts unfilled. No-op if the location
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

// The set of blueprint sources claimed by a vacancy anywhere in the area,
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
      ret[vs[j][VACANCY_FIELD_SOURCE]] = 1;
  }

  return ret;
}

// Live census count of a given source across the whole area (materialized or
// not) -- this is L_b, checked against the area cap C_b.
private int npc_live_count(string source)
{
  string * ids;
  int i, n;

  ids = map_indices(npc_census);
  for (i = 0; i < sizeof(ids); i++)
    if (npc_census[ids[i]]["source"] == source)
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

  game = game_from_path(area_path);
  if (!BESTIARY_HANDLER->has_template(game, source))
    BESTIARY_HANDLER->add_template(source);

  id = UUID_OB->uuid();
  npc_census[id] = ([ "source": source, "location": location_file,
                      "savefile": npc_save_dir(game, id) + NPC_SAVE_FILE,
                      "gender": decide_gender(game, source) ]);
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

  game = game_from_path(area_path);
  if (!BESTIARY_HANDLER->has_template(game, source))
    BESTIARY_HANDLER->add_template(source);

  id = UUID_OB->uuid();
  npc_census[id] = ([ "source": source, "location": location_file,
                      "savefile": npc_save_dir(game, id) + NPC_SAVE_FILE,
                      "gender": decide_gender(game, source),
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
private object npc_restore(string id, object loc)
{
  object npc;
  string game, source, savefile;
  mapping spec, entry;

  entry = npc_census[id];
  source = entry["source"];
  game = game_from_path(area_path);

  npc = clone_object(GENERIC_NPC);
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
  npc->apply_template(BESTIARY_HANDLER->query_template(game, source));

  spec = npc_intended[source];
  if (spec && spec["category"])
    npc->set_npc_categories(spec["category"]);

  savefile = entry["savefile"];
  if (savefile && file_size(savefile) >= 0)
    npc->restore_npc();
  else
    npc->save_npc();

  npc->move(loc);
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
  // alongside the location's regular census NPCs.
  _ensure_vacancies_assigned(file);

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

  if (npc_census[uuid])
  {
    map_delete(npc_census, uuid);
    save_me();
  }

  vacancy = clear_vacancy_by_uuid(uuid);
  if (vacancy)
    call_out("_refill_vacancy", VACANCY_RESPAWN_DELAY, vacancy[0]);
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
