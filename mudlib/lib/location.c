
// location.c

inherit obj      "/lib/core/object.c";
inherit light    "/lib/core/basic/light.c";
inherit property "/lib/core/basic/property.c";

inherit contents "/lib/room/contents.c";
inherit exits    "/lib/room/exits.c";
inherit zone     "/lib/room/zone.c";
inherit sign     "/lib/room/sign.c";
// exit protection shared with rooms; a location's guards are placed by the
// diplomacy / POI system and registered here (see /lib/room/guarded_exits.c)
inherit guarded  "/lib/room/guarded_exits.c";
inherit dark     "/lib/room/dark.c";
// the location is the entity of its components (shop, props, maze, ...)
inherit entity   "/lib/core/ecs/entity.c";

#include <basic/light.h>
#include <language.h>
#include <room/location.h>
#include <room/room.h>
#include <room/location-cleaner.h>
#include <areas/area.h>
#include <areas/poi.h>
#include <areas/common.h>
#include <sector/sector.h>
#include <translations/exits.h>

string _original_room_file_name;
string _original_long;
string _original_short;
mapping _original_add_clones;   // path -> count for each add_clone in the source room
mixed * _original_items;         // ordered ({ id_or_id_array, desc }) from add_item
// the prop types the conversion has already given this location, so one
// removed by hand is not given again on a later conversion
string * _inferred_props;
// the fixed objects this location clones on every load (a tree to climb, a
// stone to move), as blueprint -> how many
mapping _fixtures;
// what this place smells, sounds, tastes and feels like, kept so it comes back
// on every load (taken from the room it is converted from)
mapping _senses;
// Author-curated scene prose — what no component can infer. Composed
// FIRST in long(); component hook_long contributions follow. Both
// empty -> fall back to _original_long for backward compat. Editable
// inline with the `desc` cmd; persisted.
string _specific_long;
// Author-curated title, for a location built by hand rather than converted
// from a room. Taken before _original_short, and after whatever a component
// has to say about the place. Persisted.
string _specific_short;
mapping _exit_map;

string file_name;  // .o file of the location
string area_path;  // path where we will find the area.o and our own file
string map_name;   // "default", or "underdark", or "mars", use something different 
                   // from "default" to build a new map

int * coordinates;

// The coordinate at which this location is currently registered in the map
// index (SECTORS_HANDLER). Kept in sync by SECTORS_HANDLER::add_location so that,
// when a location's coordinates change (a reload that re-guesses them, a
// manual move), the old sector entry is purged before the new one is written
// and no stale "ghost" position lingers behind. nil until first indexed.
int * _map_indexed_coord;

// The location's own light, carried over from its source room. An array so a
// light of 0 (a cave) is saved too; nil keeps the default.
int * _base_light;

int _created_at;   // Unix timestamp, stamped on first save_me(), never overwritten.
int _last_imported_at;  // Unix timestamp, refreshed on every conversion from the source .c.

// function prototypes
string query_file_name();
void set_file_name(string filename);
void save_me();
void add_exits_from_exit_map(mapping m);
object query_area();
void set_exits_from_exit_map(mapping m);
private mapping _exit_map_to_canonical(mapping m);
private mapping _exit_map_to_local(mapping m);
string query_props_string();
// void init_original_info();

nomask int query_location() { return 1; }
nomask int query_room() { return 1; }
nomask int query_outside() { return !undefinedp(component_info[LOCATION_COMPONENT_OUTSIDE]); }
nomask int query_maze() { return !undefinedp(component_info[LOCATION_COMPONENT_MAZE]); }
nomask int query_water_environment()
{
  return !undefinedp(component_info[LOCATION_COMPONENT_WATER]) ||
         !undefinedp(component_info[LOCATION_COMPONENT_BRIDGE]);
}
nomask int query_water() { return query_water_environment(); }
nomask int query_bridge() { return !undefinedp(component_info[LOCATION_COMPONENT_BRIDGE]); }
nomask int query_coast() { return !undefinedp(component_info[LOCATION_COMPONENT_COAST]); }

// What a bridge cell answers, through its component: where each living is
// (on the bridge or in the water), and the climb back. In any other place
// the water answers for everybody, and dry land for nobody.
int query_in_water(object who)
{
  object bridge;

  bridge = query_component_by_type(LOCATION_COMPONENT_BRIDGE);
  return bridge ? bridge->query_in_water(who) : query_water_environment();
}

// The current of a water or bridge cell: whether it would drag `who` (in the
// water here), and a thing it is to carry off. Dry land answers nobody.
int query_water_drags(object who)
{
  object water;

  water = query_component_by_type(LOCATION_COMPONENT_WATER);
  if (!water)
    water = query_component_by_type(LOCATION_COMPONENT_BRIDGE);
  return water ? water->query_water_drags(who) : 0;
}

void water_carry(object ob)
{
  object water;

  water = query_component_by_type(LOCATION_COMPONENT_WATER);
  if (!water)
    water = query_component_by_type(LOCATION_COMPONENT_BRIDGE);
  if (water)
    water->water_carry(ob);
}

void drop_into_water(object who, varargs string message, string room_message)
{
  object bridge;

  bridge = query_component_by_type(LOCATION_COMPONENT_BRIDGE);
  if (bridge)
    bridge->drop_into_water(who, message, room_message);
}

int jump_into_water(object who)
{
  object bridge;

  bridge = query_component_by_type(LOCATION_COMPONENT_BRIDGE);
  return bridge ? bridge->jump_into_water(who) : 0;
}

int query_bridge_reachable()
{
  object bridge;

  bridge = query_component_by_type(LOCATION_COMPONENT_BRIDGE);
  return bridge ? bridge->query_bridge_reachable() : 0;
}

void set_bridge_reachable(int i)
{
  object bridge;

  bridge = query_component_by_type(LOCATION_COMPONENT_BRIDGE);
  if (bridge)
    bridge->set_bridge_reachable(i);
}

string query_exit_refusal(string dir, object who)
{
  object bridge, jail;
  string refusal;

  bridge = query_component_by_type(LOCATION_COMPONENT_BRIDGE);
  refusal = bridge ? bridge->query_exit_refusal(dir, who) : nil;
  if (refusal)
    return refusal;

  // a prisoner serving a sentence does not leave a cell
  jail = query_component_by_type(LOCATION_COMPONENT_JAIL);
  return jail ? jail->query_exit_refusal(dir, who) : nil;
}

string query_out_of_reach(object attacker, object victim)
{
  object bridge;

  bridge = query_component_by_type(LOCATION_COMPONENT_BRIDGE);
  return bridge ? bridge->query_out_of_reach(attacker, victim) : nil;
}

// The climb skill asks these of whatever is climbed.
int query_climbable() { return query_bridge(); }

string query_climb_refusal(object who)
{
  object bridge;

  bridge = query_component_by_type(LOCATION_COMPONENT_BRIDGE);
  return bridge ? bridge->query_climb_refusal(who) : nil;
}

int query_climb_modifier(object who)
{
  object bridge;

  bridge = query_component_by_type(LOCATION_COMPONENT_BRIDGE);
  return bridge ? bridge->query_climb_modifier(who) : 0;
}

void event_climbed(object who)
{
  object bridge;

  bridge = query_component_by_type(LOCATION_COMPONENT_BRIDGE);
  if (bridge)
    bridge->event_climbed(who);
}

void event_climb_failed(object who)
{
  object bridge;

  bridge = query_component_by_type(LOCATION_COMPONENT_BRIDGE);
  if (bridge)
    bridge->event_climb_failed(who);
}

// allow adding and removing objects from the inventory (is a room)
int add_weight(int n) { return 1; }
int test_add(object ob, int flag) { return 1; }
int test_remove(object ob, int flag) { return 1; }

void create() 
{
  entity::create();

  _original_room_file_name = "";
  _original_long = "";
  _original_short = "";
  _original_add_clones = ([ ]);
  _original_items = ({ });
  _inferred_props = ({ });
  _fixtures = ([ ]);
  _senses = ([ ]);
  _specific_long = "";
  _specific_short = "";
  _exit_map = ([ ]);

  file_name = "";
  area_path = "";
  map_name = "default";

  coordinates = nil;
  _map_indexed_coord = nil;

  _created_at = 0;
  _last_imported_at = 0;

  light::create();
  property::create();
  contents::create();
  exits::create();
  zone::create();
  guarded::create();
  dark::create();
  // the last one
  obj::create();

  // init_original_info();

  // don't save/restore the location if it's the location.c object itself
  // if (base_name(this_object()) != "/lib/location")
  // {
  //   if (file_size(query_file_name()) >= 0)
  //     restore_object(query_file_name());
  //   else
  //     save_me();
  // }

  set_light(BASE_ROOM_LIGHT_VALUE);

  // allow goto command to work
  add_property("location", 1);
}

void init()
{
  contents::init();
  exits::init();

  // The props component lives in this location's _inventory so the
  // standard init() call below reaches it through the canonical move/init
  // pass — no manual verb registration or forwarder needed on the location
  // side.
  run_on_components("init");
}

int id(string str)
{
  return 0;
}

// Movement entry point. Runs the components' do_exit_command pipeline
// (HOOK_PRIORITY_GATE -> TRANSFORM -> TRACE) and then calls the
// inherited exit dispatcher with the final args.
//
// Pipeline hook contract: receives ({ str, verb, ob }), returns the
// (possibly transformed) ({ str, verb, ob }). A hook can mutate the
// verb (e.g. maze randomises among declared exits) or abort the move
// by returning args with verb == 0 — components are atomic and do not
// know about each other; ordering and short-circuit semantics live
// here, in the orchestrator.
int do_exit_command(string str, varargs mixed verb, object ob)
{
  mixed * args;
  // Default the optional args the same way the inherited dispatcher
  // would (see /lib/room/handlers/exit_handler.c) so hooks downstream
  // always see usable values instead of having to defend against nil.
  if (!verb) verb = query_verb();
  if (!ob) ob = this_player();
  args = run_pipeline("do_exit_command", ({ str, verb, ob }));
  // a gate aborted the move: the dispatcher would read a missing verb as the
  // typed one again
  if (!args[1])
    return 0;
  return exits::do_exit_command(args[0], args[1], args[2]);
}

// The exit lines, unless a component says them its own way (water shows where
// the shore is instead).
string query_dirs_string()
{
  mixed ret;

  // no cast: with no component answering, the reduce gives back nil
  ret = run_reduce("dirs_string", ({ }), nil, "_concat_string");
  if (ret)
    return ret;
  return exits::query_dirs_string();
}

string query_short_exit_string()
{
  mixed ret;

  // no cast: with no component answering, the reduce gives back nil
  ret = run_reduce("short_exit_string", ({ }), nil, "_concat_string");
  if (ret)
    return ret;
  return exits::query_short_exit_string();
}

// A line of the place's own under the short line of a glance, from its
// components: where the water is, for a coast. Empty for most places.
string query_glance_string()
{
  mixed ret;

  // no cast: with no component answering, the reduce gives back nil
  ret = run_reduce("glance_string", ({ }), nil, "_concat_string");
  return ret ? ret : "";
}

string query_original_room_file_name() { return _original_room_file_name; }
void set_original_room_file_name(string str) { _original_room_file_name = str; }
string query_original_short() { return _original_short; }
void set_original_short(string str) { _original_short = str; }
string query_original_long() { return _original_long; }
void set_original_long(string str) { _original_long = str; }

mapping query_original_add_clones() { return _original_add_clones; }
void set_original_add_clones(mapping m) { _original_add_clones = m; }
mixed * query_original_items() { return _original_items; }
void set_original_items(mixed * a) { _original_items = a; }
string * query_inferred_props() { return ({ }) + (_inferred_props ? _inferred_props : ({ })); }

void add_inferred_prop(string type)
{
  if (!_inferred_props)
    _inferred_props = ({ });
  if (member_array(type, _inferred_props) == -1)
    _inferred_props += ({ type });
}

// The fixed objects of this location, as blueprint (no .c) -> how many.
mapping query_fixtures() { return ([ ]) + (_fixtures ? _fixtures : ([ ])); }

void set_fixtures(mapping m) { _fixtures = m ? ([ ]) + m : ([ ]); }

// The senses of the place, kept in the .o and applied at once.
void set_location_senses(mapping m)
{
  set_senses(m);
  _senses = query_senses();
}

// Clone whatever fixed objects are missing here: those already in the
// location are counted, so calling it again never adds a second one.
void restore_fixtures()
{
  object * here;
  string * paths;
  int i, j;

  if (!_fixtures || !map_sizeof(_fixtures))
    return;

  here = all_inventory(this_object());
  paths = map_indices(_fixtures);

  for (i = 0; i < sizeof(paths); i++)
  {
    int missing;

    missing = _fixtures[paths[i]];
    for (j = 0; j < sizeof(here); j++)
      if (here[j] && base_name(here[j]) == paths[i])
        missing--;

    for (j = 0; j < missing; j++)
    {
      object ob;

      ob = nil;
      catch(ob = clone_object(paths[i]));
      if (ob)
        ob->move(this_object());
    }
  }
}

// Add `count` (one by default) fixed objects of `path` and clone them here.
int add_fixture(string path, varargs int count)
{
  if (!path || !strlen(path))
    return 0;
  if (strlen(path) > 2 && path[strlen(path) - 2..] == ".c")
    path = path[0..strlen(path) - 3];
  if (count < 1)
    count = 1;

  if (!_fixtures)
    _fixtures = ([ ]);
  _fixtures[path] = (_fixtures[path] ? _fixtures[path] : 0) + count;

  restore_fixtures();
  return 1;
}

// Take every fixed object of `path` off the list and out of the location.
int remove_fixture(string path)
{
  object * here;
  int i;

  if (!path || !_fixtures)
    return 0;
  if (strlen(path) > 2 && path[strlen(path) - 2..] == ".c")
    path = path[0..strlen(path) - 3];
  if (undefinedp(_fixtures[path]))
    return 0;

  map_delete(_fixtures, path);

  here = all_inventory(this_object());
  for (i = 0; i < sizeof(here); i++)
    if (here[i] && base_name(here[i]) == path)
      here[i]->dest_me();

  return 1;
}

string query_specific_long() { return _specific_long; }
void set_specific_long(string str) { _specific_long = str ? str : ""; }
string query_specific_short() { return _specific_short; }
void set_specific_short(string str) { _specific_short = str ? str : ""; }

int * query_coordinates() { return coordinates; }
void set_coordinates(int x, int y, int z) { coordinates = ({ x, y, z }); }
void clear_coordinates() { coordinates = nil; }

int * query_map_indexed_coord() { return _map_indexed_coord; }
void set_map_indexed_coord(int * coord) { _map_indexed_coord = coord; }

int query_created_at() { return _created_at; }
int query_last_imported_at() { return _last_imported_at; }
void stamp_last_imported_at() { _last_imported_at = time(); }

// The light of the place itself, before the hour, the weather or anything
// carried in changes it.
void set_base_light(int value)
{
  _base_light = ({ value });
  set_light(value);
}

int query_base_light()
{
  return (_base_light && sizeof(_base_light)) ? _base_light[0] : BASE_ROOM_LIGHT_VALUE;
}

string query_map_name() { return map_name; }
void set_map_name(string name) { map_name = name; }

string query_component_dir() { return "/lib/location/components/"; }

// a location keeps its components in its own savefile, so any change to them is
// saved at once
void components_changed() { save_me(); }

// Components are not in the location's inventory, so they would never hear of
// anybody coming or going: the location passes both events on to them.
void event_enter(object ob, varargs string msg, object from, mixed avoid)
{
  obj::event_enter(ob, msg, from, avoid);
  run_on_components("event_enter", ob, msg, from, avoid);
}

void event_exit(object ob, varargs string msg, object dest, mixed avoid)
{
  obj::event_exit(ob, msg, dest, avoid);
  run_on_components("event_exit", ob, msg, dest, avoid);
}

// void init_original_info()
// {
//   if (this_object()->query_short())
//     _original_short = this_object()->query_short();
//   if (this_object()->query_long())
//     _original_long = this_object()->query_long();
// }

string query_components_string()
{
  int i;
  string * types;

  if (!sizeof(components))
    return "";

  types = ({ });

  for (i = 0; i < sizeof(components); i++)
    types += ({ components[i]->query_type() });

  return "%^BOLD%^CYAN%^" + _LANG_LOCATION_COMPONENTS_HEADER + ": " +
         implode(types, ", ") + ".%^RESET%^";
}

// Reduce hook contract for short/long/extra_look:
//   args = ({ ... }), returns the contributed string piece (or "" /
//   nil to contribute nothing). A component can take authority and
//   replace the accumulator by returning ({ HOOK_EXCLUSIVE, piece }).
string short(varargs int dark)
{
  string ret;

  // nothing but the darkness message when nothing can be seen (see
  // /lib/room/dark.c)
  if (dark && query_dark_hides_place(dark))
    return query_dark_mess(dark);

  ret = (string)run_reduce("short", ({ dark }), "", "_concat_string");
  if (!ret || !strlen(ret))
    ret = _specific_short;
  if (!ret || !strlen(ret))
    ret = _original_short;
  return ret;
}

// Whether this place is shut to the work that would be done in it. A component
// answers for its own venue -- a temple whose god the land will not have, a
// trade nobody is allowed to ply here -- and one closure is enough. The job
// itself stays declared: what closes is the place, not the post.
int query_venue_closed()
{
  return (int)run_reduce("venue_closed", ({ }), 0, "_any_true");
}

string long(varargs string str, int dark)
{
  string ret, aux, props_section, composed;

  if (this_player())
    dark = (int)this_player()->check_dark(query_light());

  // absolute darkness or glare: nothing but the message (see /lib/room/dark.c)
  if (dark && query_dark_hides_place(dark))
    return query_dark_mess(dark);

  // the details of the place, or of anything in it, cannot be made out
  if (dark && str && strlen(str))
    return query_dark_mess(dark);

  composed = (string)run_reduce("long", ({ str, dark }), "", "_concat_string");

  // When str is provided the caller is looking at a specific target
  // inside the room (a prop, an item, etc), not at the room itself.
  // Return whatever the hook chain produced and skip the room
  // boilerplate (component banner, exits, props section, inventory)
  // — those belong to the no-arg form only.
  if (str && strlen(str))
    return (composed && strlen(composed)) ? composed : "";

  // Body composition (no-arg path):
  // 1. Author-curated _specific_long sets the scene for what no
  //    component can infer.
  // 2. Component hook_long contributions (via the run_reduce above)
  //    follow, joined by a single space so the result reads as prose.
  // 3. If neither yielded anything, fall back to _original_long for
  //    backward compatibility with unmigrated locations.
  ret = "";
  if (_specific_long && strlen(_specific_long))
    ret = _specific_long;
  if (composed && strlen(composed))
    ret = strlen(ret) ? ret + " " + composed : composed;
  if (!strlen(ret))
    ret = _original_long ? _original_long : "";

  // Append the trailing newline authors of legacy rooms typically
  // put inside their set_long("text.\n") call — wrap()'s prettify
  // mode turns that into a blank line under the description, so the
  // block reads as one paragraph surrounded by whitespace both
  // above (from wrap's leading newline) and below.
  if (strlen(ret) && ret[strlen(ret) - 1] != '\n')
    ret += "\n";

  // too dark or too bright for details: the description is lost, the rest of
  // the place (weather, exits, props, contents) is still seen
  if (dark)
    ret = query_dark_mess(dark);
  else
    ret = wrap(ret, (this_user() ? this_user()->query_cols() : 79), 1);

  // Atmospheric extra-look contributions, appended under the description
  // as their own line: the outside component's weather line (via the
  // "extra_look" reduce hook) and any legacy add_extra_look() objects.
  // Empty for locations with no such contributor (e.g. interiors with no
  // outside component), so this is safe to append unconditionally.
  aux = calc_extra_look();
  if (aux && strlen(aux))
    ret += aux;

  if (this_player()->query_coder())
  {
    aux = query_components_string();
    if (strlen(aux))
      ret += aux + "\n";
  }

  exit_string = query_dirs_string();
  ret += exit_string + "\n";

  // Compact section line ("Ves una barra, dos sillas y una
  // chimenea.") between the exits and the real inventory. Rich
  // per-prop prose lives above (folded into the room body by the
  // props component's hook_long); this line is the quick summary
  // that also appears in `glance`.
  props_section = query_props_string();
  if (strlen(props_section))
    ret += props_section;

  ret += query_contents("");

  return ret;
}

// Asks the attached props component for its rendered section line
// (composed by /lib/location/components/props.c::query_props_section_string,
// which delegates per-instance work to /lib/handlers/props.c). Returns
// "" when no props component is attached, so callers can append
// unconditionally and locations without props pay no rendering cost.
string query_props_string()
{
  object props_comp;
  string ret;

  props_comp = query_component_by_type(LOCATION_COMPONENT_PROPS);
  if (!props_comp) return "";

  ret = (string)props_comp->query_props_section_string();
  if (!strlen(ret)) return "";

  return wrap(ret, (this_user() ? this_user()->query_cols() : 79));
}

string calc_extra_look()
{
  string base;
  base = ::calc_extra_look();
  return (string)run_reduce("extra_look", ({ }),
                            base ? base : "", "_concat_string");
}

// Full save path of the area this location belongs to (the directory
// holding the .o files of every sibling location).
string query_area_path() { return area_path; }

/**
 * Short identifier of the location's area: everything after the
 * "areas/" segment of the area path, with the trailing "/" stripped.
 * Falls back to the full path if "areas/" is not present. Matches
 * area->query_area_name() on the storage object.
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

string query_file_name() { return file_name; }

// This location's POI entry (from its area), or nil. A location holds at
// most one POI. See include/areas/poi.h.
mapping query_poi()
{
  object a;
  a = query_area();
  return a ? a->query_poi(file_name) : nil;
}

// Coder look/glance marker: "poi <kind>" (plus the label when set) when
// this location is a registered point of interest, else "". Composed here
// so look.c and glance.c render it identically.
string query_poi_marker()
{
  mapping p;

  p = query_poi();
  if (!p)
    return "";

  if (p[POI_FIELD_LABEL] && strlen(p[POI_FIELD_LABEL]))
    // labels are captured from the room short, which may carry %^colour%^
    // codes; strip them so the coder line stays plain.
    return "poi " + p[POI_FIELD_KIND] + " \"" +
           handler("terminal")->clean_string(p[POI_FIELD_LABEL]) + "\"";
  return "poi " + p[POI_FIELD_KIND];
}

void set_file_name(string name)
{
  string * pieces;
  pieces = explode(name, "/");

  if (sizeof(pieces) >= 2)
    area_path = "/" + implode(pieces[0..sizeof(pieces) - 2], "/") + "/";

  file_name = name;
  save_me();

  // now that the location has its game-bound identity, hand it to the
  // cleaner so it is tracked for residency / eviction. Only real clones
  // register; the /lib/location blueprint never does.
  if (clonep() && strlen(name))
    LOCATION_CLEANER->register_object(this_object());
}

// restore the location from a file
// returns 1 if the file exists and the location was restored, 0 otherwise
int restore_from_file_name(string name)
{
  if (file_size(name) >= 0)
  {
    restore_object(name);

    // the persisted exit map uses canonical English direction keys;
    // bring them back to the current language before the runtime exit
    // system consumes them.
    _exit_map = _exit_map_to_local(_exit_map);

    add_exits_from_exit_map(_exit_map);

    set_light(query_base_light());

    init_components(component_info);

    restore_fixtures();

    if (mappingp(_senses))
      set_senses(_senses);

    // restore_object set file_name directly (this is the normal load path,
    // not set_file_name), so register with the cleaner here too
    if (clonep() && strlen(file_name))
      LOCATION_CLEANER->register_object(this_object());

    return 1;
  }

  return 0;
}

int guess_coordinates()
{
  int x, y, z;
  int i;
  string * exit_info;
  int found;

  found = FALSE;

  // Guard against clobbering coords that were assigned deliberately —
  // via room2loc coords, direct set_coordinates, or a previous
  // successful inference pass. Without this a re-import silently
  // overwrites builder-set coords.
  if (query_coordinates() != nil)
    return FALSE;

  exit_info = query_dest_dir();

  for (i = 0; i < sizeof(exit_info); i += 2)
  {
    object dest;
    dest = query_dest_object(exit_info[i]);

    if (dest && dest->query_coordinates() != nil)
    {
      int * dest_coords;
      dest_coords = dest->query_coordinates();

      switch (exit_info[i])
      {
        case DIR_NORTH:
          set_coordinates(dest_coords[0], 
                          dest_coords[1] - 1, 
                          dest_coords[2]);
          found = TRUE;
          break;
        case DIR_SOUTH:
          set_coordinates(dest_coords[0], 
                          dest_coords[1] + 1, 
                          dest_coords[2]);
          found = TRUE;
          break;
        case DIR_EAST:
          set_coordinates(dest_coords[0] - 1, 
                          dest_coords[1], 
                          dest_coords[2]);
          found = TRUE;
          break;
        case DIR_WEST:
          set_coordinates(dest_coords[0] + 1, 
                          dest_coords[1], 
                          dest_coords[2]);
          found = TRUE;
          break;
        case DIR_NORTHWEST:
          set_coordinates(dest_coords[0] + 1, 
                          dest_coords[1] - 1, 
                          dest_coords[2]);
          found = TRUE;
          break;
        case DIR_NORTHEAST:
          set_coordinates(dest_coords[0] - 1, 
                          dest_coords[1] - 1, 
                          dest_coords[2]);
          found = TRUE;
          break;
        case DIR_SOUTHWEST:
          set_coordinates(dest_coords[0] + 1, 
                          dest_coords[1] + 1, 
                          dest_coords[2]);
          found = TRUE;
          break;
        case DIR_SOUTHEAST:
          set_coordinates(dest_coords[0] - 1, 
                          dest_coords[1] + 1, 
                          dest_coords[2]);
          found = TRUE;
          break;          
        case DIR_UP:
          set_coordinates(dest_coords[0], 
                          dest_coords[1], 
                          dest_coords[2] - 1);
          found = TRUE;
          break;
        case DIR_DOWN:
          set_coordinates(dest_coords[0], 
                          dest_coords[1], 
                          dest_coords[2] + 1);
          found = TRUE;
          break;
      }
    }
  }

  save_me();

  return found;
}

// Direction keys are stored on disk in canonical English so a game's
// /save tree stays migratable between the languages the mudlib compiles
// in (see room_handler::canonical_dir / localize_dir). Runtime keeps the
// exit map in the current language; these two helpers translate the map
// only at the persistence boundary — canonical on the way out, localized
// on the way back. Non-direction keys (custom exit verbs) pass through.
private mapping _exit_map_to_canonical(mapping m)
{
  mapping out;
  string * ks;
  int i;

  out = ([ ]);
  ks = keys(m);
  for (i = 0; i < sizeof(ks); i++)
    out[ROOM_HAND->canonical_dir(ks[i])] = m[ks[i]];

  return out;
}

private mapping _exit_map_to_local(mapping m)
{
  mapping out;
  string * ks;
  int i;

  out = ([ ]);
  ks = keys(m);
  // canonical first so both legacy localized and English keys normalize,
  // then localize to the current language
  for (i = 0; i < sizeof(ks); i++)
    out[ROOM_HAND->localize_dir(ROOM_HAND->canonical_dir(ks[i]))] = m[ks[i]];

  return out;
}

// we save the full exit map
// and restore it when the location is loaded
void add_exits_from_exit_map(mapping m)
{
  int i;
  string * exit_names;
  // ({ dest, type, material }) plus an optional 4th slot: the door options
  // mapping -- so the element type is mixed, not string
  mixed * exits_info;

  exit_names = keys(m);

  for (i = 0; i < sizeof(exit_names); i++)
  {
    exits_info = m[exit_names[i]];

    // first remove the current exit if it exists
    remove_exit(exit_names[i]);

    // a 4th tuple slot, when present, is the door options mapping (closed,
    // locked, keys, ...): pass it through so the rebuilt door is reconfigured
    // to its default state on every load
    add_exit(exit_names[i],
             exits_info[0],
             exits_info[1],
             exits_info[2],
             sizeof(exits_info) > 3 ? exits_info[3] : nil);
  }
}

// Replace-all: remove every existing exit, then apply `m`.
// Replace this location's exits with the ones a room `.c` declares, which is
// what a conversion hands over.
//
// Exits into a location that has no source room survive it. Those are the parts
// of the world carved at run time -- a plot and the house raised on it exist
// only as a `.o` -- so the room `.c` cannot mention the way in, and rebuilding
// blindly from it would wall them off while every other trace of them survived.
void set_exits_from_exit_map(mapping m)
{
  mapping carved;
  string * exit_names;
  int i;

  carved = ([ ]);
  exit_names = keys(_exit_map);

  for (i = 0; i < sizeof(exit_names); i++)
  {
    string dest;
    mixed source;

    // no (string) cast: that is a conversion kfun and it errors on the nil a
    // destination outside the location tree hands back
    dest = _exit_map[exit_names[i]][0];
    source = load_object(LOCATION_HANDLER)->query_room_source(dest);
    if (dest && !m[exit_names[i]] &&
        (!stringp(source) || file_size(source) < 0))
      carved[exit_names[i]] = _exit_map[exit_names[i]];

    remove_exit(exit_names[i]);
  }

  add_exits_from_exit_map(m);
  add_exits_from_exit_map(carved);
}

// check every exit, if it goes to a room already converted to location, 
// change its destination to the saved location file
int update_exits_to_locations()
{
  string * keys;
  int i;
  int exits_changed;

  keys = keys(_exit_map);
  exits_changed = 0;

  for (i = 0; i < sizeof(keys); i++)
  {
    object dest;
    string dest_location_file_name;

    dest = load_object(_exit_map[keys[i]][0]);

    if (!dest)
      continue;

    dest_location_file_name = LOCATION_HANDLER->get_location_file_name_from_room(dest);

    // if file already exists, change the exit destination
    if ((_exit_map[keys[i]][0] != dest_location_file_name) &&
        (file_size(dest_location_file_name) > 0))
    {
      _exit_map[keys[i]][0] = dest_location_file_name;
      exits_changed += 1;
    }
  }

  if (exits_changed > 0)
  {
    add_exits_from_exit_map(map_copy(_exit_map));
    save_me();
  }

  return exits_changed;
}

object query_area()
{
  return LOCATION_HANDLER->query_area_from_location_file_name(query_file_name());
}

// Inflate the props component's reference in the parser's candidate
// list so the mudlib's standard numeric idiom (`silla 2`, `chair 3`)
// resolves against composite props. The catalogue component is a
// single LPC object but carries N prop instances; without inflation
// find_match's `--num` decrement can never count past 1. We add
// (num - 1) extra references for prop-matching queries, letting the
// standard mechanism pick the N-th matching instance transparently.
// The single-instance case (num == 0 / no trailing number) doesn't
// need inflation and is left alone.
object * find_inv_match(string str)
{
  object * inv, * extras;
  object props_comp;
  string aux;
  int num, prop_count, i;

  inv = (object *)all_inventory(this_object()) +
        (object *)contents::query_hidden_objects();

  if (!str || !strlen(str)) return inv;

  props_comp = query_component_by_type(LOCATION_COMPONENT_PROPS);
  if (!props_comp) return inv;

  if (sscanf(str, "%s %d", aux, num) != 2 || num <= 1)
    return inv;

  prop_count = (int)props_comp->count_matching_props(aux);
  if (num > prop_count) return inv;

  // Extra copies so find_match's --num decrement can advance to
  // the requested slot. The component is already in inv once
  // (from all_inventory); add num-1 more.
  extras = ({ });
  for (i = 1; i < num; i++)
    extras += ({ props_comp });
  inv += extras;

  return inv;
}

void save_me()
{
  // stamp the original-creation timestamp the first time this location
  // is ever persisted; never overwrite it afterwards.
  if (!_created_at)
    _created_at = time();

  // save with the current exits
  _exit_map = query_exit_map();

  // pull each component's current autoload state into component_info
  // so the next restore sees what the live objects looked like, not
  // just the seed values from conversion. Pull-on-save closes the loop with
  // init_components, which pushes them back on restore (see the header of
  // /lib/location/component.c for the persistence contract).
  sync_component_info();

  // the file goes first: an index entry can be rebuilt from the file (a load
  // indexes what is not indexed), a file that was never written cannot
  if (file_name && strlen(file_name))
  {
    mapping runtime_exits;

    // persist canonical English direction keys, then restore the
    // localized runtime map so nothing downstream sees English keys.
    runtime_exits = _exit_map;
    _exit_map = _exit_map_to_canonical(runtime_exits);
    save_object(file_name);
    _exit_map = runtime_exits;
  }

  // save in the map system every location with coordinates
  if (query_coordinates() != nil)
    SECTORS_HANDLER->add_location(this_object());

  // save it in the area system also
  AREA_HANDLER->add_location(this_object());
}

// Passive eviction entry point. The cleaner decides WHEN to reclaim a
// location; this method decides HOW, and may refuse. Returns 1 to keep
// the location, 0 when it tore itself down.
int clean_up(varargs int flag)
{
  object * arr;
  int i;

  // pinned, or a corpse is resting here -> never reclaim
  if (query_property(NO_CLEAN_UP_PROP))
    return 1;
  if (query_property("corpse_here"))
    return 1;

  // belt-and-suspenders against a handler race: never reclaim a location
  // a real player is standing in. A unique object exists once in the whole
  // game, so the place it rests in stays rather than taking it down with it.
  arr = deep_inventory(this_object());
  for (i = 0; i < sizeof(arr); i++)
  {
    if (!arr[i])
      continue;
    if (userp(arr[i]) || arr[i]->query_unique_object())
      return 1;
  }

  dest_me();
  return 0;
}

void dest_me()
{
  object * arr;
  object a;
  int i;

  // hand the location back to the cleaner before it goes away
  if (clonep())
    LOCATION_CLEANER->deregister_object(this_object());

  // persist our dynamic NPCs before the inventory is torn down below, so
  // their state survives the unload (the census entries stay in the area)
  a = query_area();
  if (a)
    a->drain_location(this_object());

  // similar to room.c
  arr = all_inventory(this_object());
  arr -= ({ nil });

  for (i = 0; i < sizeof(arr); i++)
  {
    // important, so we do not destroy the player avatar
    if (interactive(arr[i]))
        arr[i]->move(ROOM_VOID);
    else
        arr[i]->dest_me();
  }

  // tear down our own live components, the same way remove_component does
  destruct_components();

  // forget the guard registry (the guards themselves are area-managed NPCs,
  // drained above)
  clear_guards();

  destruct(this_object());
}

int query_light()
{
  return (int)run_reduce("query_light", ({ }),
                         light::query_light(), "_sum_int");
}

mixed * stats()
{
  int i;
  mapping comp_stats;
  
  comp_stats = ([ ]);

  for (i = 0; i < sizeof(components); i++)
    comp_stats["Component " + components[i]->query_type()] = components[i]->stats();

  return obj::stats() +
         light::stats() +
         property::stats() + 
         zone::stats() + 
         exits::stats() +
          ({
      ({ "Map Name", map_name, }),
      ({ "Coordinates", coordinates, }),
      ({ "Components (nosave)", keys(component_info), }),
      ({ "Components stats (extra info)", comp_stats, }),
          });

      // desc::stats() +
      // guard::stats() +
      // navigation::stats() +
      // diplomacy::stats();
}
