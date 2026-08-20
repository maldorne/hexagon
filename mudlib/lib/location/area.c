inherit obj      "/lib/core/object.c";

inherit monsters  "/lib/location/area/monsters.c";
inherit schedules "/lib/location/area/schedules.c";
inherit housing   "/lib/location/area/housing.c";
inherit pois      "/lib/location/area/pois.c";
inherit guards    "/lib/location/area/guards.c";
inherit census    "/lib/location/area/census.c";

#include <room/location.h>
#include <living/persisted.h>
#include <areas/area.h>
#include <areas/poi.h>
#include <areas/diplomacy.h>
#include <basic/gender.h>
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

// The settlement's role board: the
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

// prototype functions
void add_loaded_location(object location);
void recompute_intended();
string _template_id(string source);
void equip_npc(object npc, string * paths);
string * resolve_equipment(mixed * spec);
string generate_citizen_name(int gender);


void create() {
  locations = ([ ]);
  loaded_locations = ({ });
  connections = ([ ]);
  file_name = "";
  area_path = "";
  gives_exploration = 0;
  exploration_name = "";
  npc_intended = ([ ]);
  npc_sources = ([ ]);
  roles = ([ ]);
  npc_default_level = 1;
  npc_default_level_spread = 0;
  monsters::create();
  schedules::create();
  housing::create();
  pois::create();
  guards::create();
  census::create();
  obj::create();
}

void restore_me() {
  restore_object(file_name);
}

void save_me() {
  save_object(file_name);
}

// The area's event-log file, under the game's central logs dir (mirrors how
// ventures log under /save/games/<game>/logs/ventures/). The name is built from
// the area's path so each area has its own file: an area at
// /save/games/<game>/locations/areas/<area>/rooms/ logs to
// /save/games/<game>/logs/areas/areas-<area>-rooms.log.
string query_log_file()
{
  string game, rel;

  game = game_from_path(file_name);
  rel = replace_string(file_name, "/save/games/" + game + "/locations/", "");
  rel = replace_string(rel, "/area.o", "");
  return "/save/games/" + game + "/logs/areas/" +
         implode(explode(rel, "/"), "-") + ".log";
}

// Append a line to the area's event log. One English line per notable event --
// for now "no free plot", later invasions, guards reposted, houses raised, and
// so on. A shared record a builder or an audit can read. Creates the logs dir
// on demand (mkdir is recursive).
void log_event(string msg)
{
  string logf, dir;
  int slash;

  if (!msg)
    return;

  logf = query_log_file();
  slash = strsrch(logf, "/", -1);
  dir = (slash >= 0) ? logf[0..slash] : "";
  mkdir(dir);
  write_file(logf, ctime(time()) + "  " + msg + "\n");
}

// Every NPC currently materialized across the area's loaded locations. Used by
// the builder's `build npc` report to list each roster template's live members
// with their position, work and home.
object * query_live_npcs()
{
  object * everyone, * npcs;
  int i, j;

  everyone = ({ });
  for (i = 0; i < sizeof(loaded_locations); i++)
    if (loaded_locations[i])
      everyone += all_inventory(loaded_locations[i]);

  npcs = ({ });
  for (j = 0; j < sizeof(everyone); j++)
    if (everyone[j] && everyone[j]->query_npc())
      npcs += ({ everyone[j] });
  return npcs;
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

  save_me();
  return 1;
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
    recompute_intended();
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

void set_npc_intended(mapping m)
{
  npc_intended = m ? m : ([ ]);
  save_me();
}

// Declare (or replace) an intended NPC for this area. `source` is the NPC's
// source path -- it is the identity (census key) and is snapshotted into a
// data template on first spawn. `max` is the area-wide population cap.
void add_intended_npc(string source, int max)
{
  npc_intended[source] = ([ "max": max ]);
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
string _template_id(string source)
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
  locs = map_indices(query_pois());
  for (i = 0; i < sizeof(locs); i++)
  {
    mapping * vs;
    vs = query_pois()[locs[i]][POI_FIELD_VACANCIES];
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
  ids = map_indices(query_npc_census());
  for (i = 0; i < sizeof(ids); i++)
  {
    mapping e;
    string tid;

    e = query_npc_census()[ids[i]];
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

  vacancy_sources = query_vacancy_sources();

  // the citizenship's guard NPC is placed by diplomacy at guarded POIs, so it
  // must not also be scattered by the population sweep as statistical filler
  guard_source = "";
  if (strlen(query_citizenship()))
    guard_source = _template_id(
      DIPLOMACY_HANDLER->query_guard_path(game_from_path(area_path),
                                          query_citizenship()));

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
    POPULATION_HANDLER->include_area(area_path);

  save_me();
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
  save_me();
}

void set_area_spread(int n)
{
  npc_default_level_spread = n < 0 ? 0 : n;
  save_me();
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

// ---------------------------------------------------------------------------
// Role board
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

// Declare (or replace) a role: the area-level cap for a kind of citizen. `count`
// is how many of it the settlement wants, `work` its work location, `source` the
// NPC blueprint filling its slots (snapshotted to a template now, the one time
// its .c is read). Stores the template id. Behaviour (sentient, equipment,
// timetable) is not stored here -- it lives on the type template, written by the
// builder through the bestiary; the area only holds the count and the workplace.
void add_role(string name, int count, string work, string source)
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

  roles[name] = ([ "count":  count,
                   "work":   work,
                   "source": source ]);
  save_me();
}

// Give one NPC the kit a role defines: monster::add_clone clones each blueprint
// straight into it, then init_equip wears / wields the lot. Used both when a
// role NPC first materializes and when a live role's kit is changed.
void equip_npc(object npc, string * paths)
{
  int i;

  if (!npc || !pointerp(paths) || !sizeof(paths))
    return;
  for (i = 0; i < sizeof(paths); i++)
    npc->add_clone(paths[i], 1);
  npc->init_equip();
}

// Resolve a role's equipment spec into one concrete kit. The spec is an array
// of slots; each slot is an array of interchangeable blueprints and one is
// picked at random (a fixed item is just a one-element slot). This is rolled
// ONCE per NPC at assignment and stored on the census entry, so a citizen keeps
// the same weapon for life instead of re-rolling every time it materializes.
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

// Re-roll and equip the kit for this role's live holders that still carry
// nothing, reading the kit from the type template (its authoritative home for
// equipment). A holder that already has gear keeps it -- a saved citizen's gear
// never changes. Called after the builder changes a role's kit on the template,
// so existing empty-handed holders pick it up without a respawn. New holders
// roll their kit at first materialization (npc_restore, from the template).
void reequip_role_holders(string name)
{
  object loc;
  object * inv;
  mapping template;
  mixed * spec;
  string source;
  int i;

  if (!roles[name])
    return;

  source = roles[name]["source"];
  template = (source && strlen(source))
               ? BESTIARY_HANDLER->query_template(game_from_path(area_path), source)
               : nil;
  spec = template ? template["equipment"] : nil;
  if (!pointerp(spec) || !sizeof(spec))
    return;

  loc = loaded_location(roles[name]["work"]);
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
    e = uuid ? query_npc_census()[uuid] : nil;
    if (!e || e["role"] != name || e["poi"] || e["guard"])
      continue;
    // gear a holder that has none yet (created before the kit was set); one
    // that already carries gear keeps it -- a saved citizen's gear never
    // changes. The kit is rolled, equipped and saved onto the NPC's own npc.o.
    if (sizeof(all_inventory(inv[i])))
      continue;
    equip_npc(inv[i], resolve_equipment(spec));
    inv[i]->save_npc();
  }
}

// Live count of a role's staff: census entries tagged with this role that are
// role-board slots (no POI, no guard), so this never counts a POI vacancy or a
// guard that happens to share the role name.
int count_role_npcs(string name)
{
  string * ids;
  int i, n;

  ids = map_indices(query_npc_census());
  for (i = 0; i < sizeof(ids); i++)
  {
    mapping e;
    e = query_npc_census()[ids[i]];
    if (e["role"] == name && !e["poi"] && !e["guard"])
      n++;
  }

  return n;
}

// A generated given-name (lowercase) for one of this area's citizens: the name
// generator draws it from the area citizenship's name style, in the form
// matching the given gender (a GENDER_* id). It does not depend on the role.
// Returns nil when the area has no citizenship, the citizenship declares no
// name style, or the generator has no wordlist for it -- the NPC then keeps its
// template's name.
string generate_citizen_name(int gender)
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
  string id, game, source, work;

  source = role["source"];
  work = role["work"];
  if (!source || !strlen(source) || !work || !strlen(work))
    return nil;

  game = game_from_path(area_path);
  id = UUID_OB->uuid();

  // Data-only roster entry. The individual (gender, generated name, level,
  // equipment) is materialized and persisted on its own npc.o at first load
  // (npc_restore); nothing per-individual is stored in the census.
  add_census_entry(id, ([ "source":   source,
                          "location": work,
                          "savefile": npc_save_dir(game, id) + NPC_SAVE_FILE,
                          "role":     name ]));

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

  ids = map_indices(query_npc_census());
  for (i = 0; i < sizeof(ids); i++)
  {
    mapping e;
    object npc;

    e = query_npc_census()[ids[i]];
    if (e["role"] == name && !e["poi"] && !e["guard"])
    {
      npc = live_census_npc(e["location"], ids[i]);
      if (npc)
        npc->dest_me();
      if (e["savefile"] && file_size(e["savefile"]) >= 0)
        remove_file(e["savefile"]);
      drop_census_entry(ids[i]);
    }
  }

  map_delete(roles, name);
  save_me();
}
