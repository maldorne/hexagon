inherit obj      "/lib/core/object.c";

inherit monsters  "/lib/location/area/monsters.c";
inherit schedules "/lib/location/area/schedules.c";
inherit housing   "/lib/location/area/housing.c";
inherit pois      "/lib/location/area/pois.c";
inherit citizen   "/lib/location/area/citizenship.c";
inherit census    "/lib/location/area/census.c";
inherit vacancies "/lib/location/area/vacancies.c";
inherit roster    "/lib/location/area/roster.c";

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
// The area's locations currently in memory, indexed by file name:
//   ([ file_name : location object ])
// Keyed by file name so "is this location loaded?" is a lookup instead of a
// scan, and a duplicate entry for the same file is impossible.
static mapping loaded_locations;
string file_name;
string area_path;
// exploration: whether entering this area grants an exploration achievement,
// and the display name recorded in the player's diary when it does.
int gives_exploration;
string exploration_name;
// The area this one belongs to, or "" when it answers to nobody. An area is a
// place -- locations, coordinates, components -- and the two need not coincide
// with what the place is part of: a town's fields are their own place but the
// same town, so the fields name the town as their parent and keep their own
// geography. Everything shared between an area and its ancestors is resolved
// through this link, starting with the population (roster, census, roles,
// houses). Set by hand, never derived from the directory tree: a wilderness
// that happens to sit under a region folder is nobody's suburb.
string parent_area;




// prototype functions
object query_root_area();
void add_loaded_location(object location);
object query_loaded_location(string file);
object * query_loaded_locations();


void create() {
  locations = ([ ]);
  loaded_locations = ([ ]);
  connections = ([ ]);
  file_name = "";
  area_path = "";
  parent_area = "";
  gives_exploration = 0;
  exploration_name = "";
  monsters::create();
  schedules::create();
  housing::create();
  pois::create();
  citizen::create();
  census::create();
  vacancies::create();
  roster::create();
  obj::create();
}

void restore_me() {
  restore_object(file_name);
}

void save_me() {
  object owner;

  save_object(file_name);

  // A delegated area shares its community's mappings by reference, so anything
  // that changed a roster, a census or a role here changed them in the parent
  // too -- and the parent is the file they are written to.
  owner = query_root_area();
  if (owner != this_object())
    owner->save_me();
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
  object * everyone, * npcs, * locs;
  int i, j;

  everyone = ({ });
  locs = query_loaded_locations();
  for (i = 0; i < sizeof(locs); i++)
    everyone += all_inventory(locs[i]);

  npcs = ({ });
  for (j = 0; j < sizeof(everyone); j++)
    if (everyone[j] && everyone[j]->query_npc())
      npcs += ({ everyone[j] });
  return npcs;
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
  object * ret, * locs;
  int i;

  ret = ({ });
  locs = query_loaded_locations();
  for (i = 0; i < sizeof(locs); i++)
    if (locs[i]->query_component_by_type(LOCATION_COMPONENT_MAZE))
      ret += ({ locs[i] });

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

    // Rebuild the derived roster from original_npc_sources on load: npc_caps is not
    // authoritative state, it is the sum of the per-location provenance.
    rebuild_npc_caps();
    return 1;
  }

  return 0;
}

object load_location(string location_file_name)
{
  object location;

  // A location outside this area still has to be reachable from here: a
  // community spans every area that delegates to it, so its houses, work spots
  // and census entries name locations that belong to a sibling. The handler
  // routes the file to the area that owns it.
  if (!locations[location_file_name])
  {
    object owner;

    if (!strlen(location_file_name))
      return nil;

    // the owner check is what keeps this from bouncing: the handler comes
    // straight back here for a file this area is supposed to hold
    owner = load_object(LOCATION_HANDLER)->
              query_area_from_location_file_name(location_file_name);
    if (!owner || owner == this_object())
      return nil;

    return load_object(LOCATION_HANDLER)->load_location(location_file_name);
  }

  location = query_loaded_location(location_file_name);
  if (location)
    return location;

  location = clone_object(BASE_LOCATION_OBJ);

  if (!location)
    return nil;

  location->restore_from_file_name(location_file_name);
  loaded_locations[location_file_name] = location;

  // The only path from unloaded to loaded, so the only place its people are
  // brought in. Done before handing it back, or the first look shows it empty.
  census::restore_location_npcs(location);

  return location;
}

string query_parent_area_path() { return parent_area; }

// The area directly above this one, or nil when it stands on its own.
object query_parent_area()
{
  return strlen(parent_area) ? AREA_HANDLER->query_area(parent_area) : nil;
}

// Attach this area to another, or bring it back to standing on its own (""
// clears the link). Both directions are deliberate acts: the tree is never
// guessed.
void set_parent_area(string path)
{
  parent_area = path ? path : "";
  save_me();
}

// The top of this area's chain: the one that holds whatever is shared down it,
// starting with the community -- roster, census, roles, houses. An area with no
// parent is its own root, which is the common case. The walk is bounded so a
// pair of areas pointed at each other cannot hang the driver, and a parent that
// no longer loads leaves this area standing alone rather than headless.
object query_root_area()
{
  object area;
  string path;
  int steps;

  path = parent_area;
  area = this_object();

  for (steps = 0; strlen(path) && steps < AREA_MAX_ANCESTRY; steps++)
  {
    object up;

    up = AREA_HANDLER->query_area(path);
    if (!up || up == area)
      break;

    area = up;
    path = (string)up->query_parent_area_path();
  }

  return area;
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
  if (location)
    loaded_locations[location->query_file_name()] = location;
}

// The loaded location object for a file, or nil when it is not in memory. Never
// loads anything: a caller that wants it loaded asks load_location instead.
object query_loaded_location(string file)
{
  return file ? loaded_locations[file] : nil;
}

object * query_loaded_locations() 
{
  return map_values(loaded_locations) - ({ nil });
}

mapping query_connections() { return connections; }
void add_connection(string location_file_name, string direction, string destination)
{
  if (!connections[location_file_name])
    connections[location_file_name] = ({ direction, destination });

  save_me();
}

