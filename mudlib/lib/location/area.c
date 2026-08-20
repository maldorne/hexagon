inherit obj      "/lib/core/object.c";

inherit monsters  "/lib/location/area/monsters.c";
inherit schedules "/lib/location/area/schedules.c";
inherit housing   "/lib/location/area/housing.c";
inherit pois      "/lib/location/area/pois.c";
inherit guards    "/lib/location/area/guards.c";
inherit census    "/lib/location/area/census.c";
inherit roles     "/lib/location/area/roles.c";
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
// array of loaded locations
static object * loaded_locations;
string file_name;
string area_path;
// exploration: whether entering this area grants an exploration achievement,
// and the display name recorded in the player's diary when it does.
int gives_exploration;
string exploration_name;




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
  monsters::create();
  schedules::create();
  housing::create();
  pois::create();
  guards::create();
  census::create();
  roles::create();
  roster::create();
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

