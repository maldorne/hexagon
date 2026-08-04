inherit "/lib/core/object.c";

#include <room/location.h>
#include <living/npc_persisted.h>

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

// Dynamic NPC population (see dev/area-npc-system.md).
//   npc_intended: configuration -- what may spawn here and how many.
//     ([ kind : ([ "category": ({ ids }), "source": blueprint_path,
//                  "max": int, "zones": ({ room_zone, ... }) or nil ]) ])
//   npc_census:   live state -- which concrete NPCs exist and where.
//     ([ uuid : ([ "kind": kind, "location": location_file,
//                  "savefile": npc.o path ]) ])
// The census is the authoritative summary of the area's population; NPC
// objects are materialized into a location on load and drained on unload,
// but the census entry survives so the same NPC comes back.
mapping npc_intended;
mapping npc_census;

// prototype functions
void add_loaded_location(object location);


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

// Declare (or replace) an intended NPC kind for this area. `categories` is
// the coarse type list stamped on the mob, `source` the blueprint to clone,
// `max` the area-wide population cap, `zones` an optional room-zone filter
// (nil/empty = any location in the area).
void add_intended_npc(string kind, string * categories, string source,
                      int max, string * zones)
{
  npc_intended[kind] = ([ "category": categories, "source": source,
                          "max": max, "zones": zones ]);
  save_me();
}

void remove_intended_npc(string kind)
{
  map_delete(npc_intended, kind);
  save_me();
}

// Number of live census entries of a given kind (materialized or not).
private int npc_kind_count(string kind)
{
  string * ids;
  int i, n;

  ids = map_indices(npc_census);
  for (i = 0; i < sizeof(ids); i++)
    if (npc_census[ids[i]]["kind"] == kind)
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

// Is `uuid` already materialized inside `loc`? Non-mob contents answer nil to
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

// A location is eligible for a kind when the kind declares no zone filter, or
// the location shares at least one of the declared room zones.
private int npc_location_eligible(object loc, mapping spec)
{
  string * need, * have;

  need = spec["zones"];
  if (!need || !sizeof(need))
    return 1;

  have = loc->query_room_zones();
  return have && sizeof(have & need) > 0;
}

// Clone the kind's blueprint, stamp a fresh identity, move it into `loc`,
// persist it and record the census entry. Returns the new NPC or nil.
private object npc_spawn(string kind, object loc)
{
  mapping spec;
  object npc;
  string id, game;

  spec = npc_intended[kind];
  if (!spec || !spec["source"])
    return nil;

  npc = clone_object(spec["source"]);
  if (!npc)
    return nil;

  game = game_from_path(area_path);
  id = UUID_OB->uuid();

  npc->set_npc_uuid(id);
  npc->set_npc_game(game);
  npc->set_npc_area_path(area_path);
  if (spec["category"])
    npc->set_npc_categories(spec["category"]);

  npc->move(loc);
  npc->save_npc();

  npc_census[id] = ([ "kind": kind, "location": loc->query_file_name(),
                      "savefile": npc_save_dir(game, id) + NPC_SAVE_FILE ]);
  save_me();

  return npc;
}

// Re-materialize an existing census NPC into `loc`, loading its saved state.
private object npc_restore(string id, object loc)
{
  mapping entry, spec;
  object npc;

  entry = npc_census[id];
  spec = npc_intended[entry["kind"]];
  if (!spec || !spec["source"])
    return nil;

  npc = clone_object(spec["source"]);
  if (!npc)
    return nil;

  npc->set_npc_uuid(id);
  npc->set_npc_game(game_from_path(area_path));
  npc->set_npc_area_path(area_path);
  npc->restore_npc();
  npc->move(loc);

  return npc;
}

// Called (via call_out) when a location of this area finishes loading: bring
// back the NPCs the census assigns here, then top up toward the area caps for
// every eligible intended kind. Idempotent -- already-present NPCs are left
// alone, so repeated prewarm load/unload does not duplicate population.
void populate_location(object loc)
{
  string file;
  string * ids, * kinds;
  int i;
  mapping spec;

  if (!loc)
    return;

  file = loc->query_file_name();
  if (!file || !strlen(file))
    return;

  // 1. restore census NPCs assigned to this location
  ids = npc_census_for_location(file);
  for (i = 0; i < sizeof(ids); i++)
    if (!npc_uuid_present(loc, ids[i]))
      npc_restore(ids[i], loc);

  // 2. spawn toward the cap for each eligible kind (one per load; the
  //    population spreads as more eligible locations are visited)
  kinds = map_indices(npc_intended);
  for (i = 0; i < sizeof(kinds); i++)
  {
    spec = npc_intended[kinds[i]];
    if (npc_location_eligible(loc, spec) &&
        npc_kind_count(kinds[i]) < spec["max"])
      npc_spawn(kinds[i], loc);
  }
}

// Called from a location's dest_me before its contents are torn down: persist
// each of our NPCs so its state survives the unload. The census entry stays,
// so populate_location will bring the NPC back on the next load.
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

// Called by a persisted NPC (via monster::do_death) when it dies: drop its
// census slot so the population frees up and a replacement may spawn later.
// The savefile itself is removed by the NPC's own delete_npc_save().
void npc_died(string uuid)
{
  if (npc_census[uuid])
  {
    map_delete(npc_census, uuid);
    save_me();
  }
}
