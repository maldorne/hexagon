
// location.c

inherit obj      "/lib/core/object.c";
inherit light    "/lib/core/basic/light.c";
inherit property "/lib/core/basic/property.c";

inherit contents "/lib/room/contents.c";
inherit exits    "/lib/room/exits.c";
inherit zone     "/lib/room/zone.c";
inherit sign     "/lib/room/sign.c";

#include <basic/light.h>
#include <room/location.h>
#include <areas/area.h>
#include <areas/common.h>
#include <maps/maps.h>
#include <translations/exits.h>

static object * components;
mapping component_info;

// Derived from components on init_components(). Maps a hookable
// public function name to the ordered list of components that
// registered for it (sorted by HOOK_PRIORITY_*, ties broken by load
// order). Never persisted. See location-hook-system.md.
static mapping hook_chains;

string _original_room_file_name;
string _original_long;
string _original_short;
mapping _original_add_clones;   // path → count for each add_clone in the source room
mixed * _original_items;         // ordered ({ id_or_id_array, desc }) from add_item
// Author-curated scene prose — what no component can infer. Composed
// FIRST in long(); component hook_long contributions follow. Both
// empty → fall back to _original_long for backward compat. Editable
// inline with the `desc` cmd; persisted.
string _specific_long;
mapping _exit_map;

string file_name;  // .o file of the location
string area_path;  // path where we will find the area.o and our own file
string map_name;   // "default", or "underdark", or "mars", use something different 
                   // from "default" to build a new map

int * coordinates;

int _created_at;   // Unix timestamp, stamped on first save_me(), never overwritten.
int _last_imported_at;  // Unix timestamp, refreshed on every conversion from the source .c.

// function prototypes
string query_file_name();
void set_file_name(string filename);
void save_me();
void add_exits_from_exit_map(mapping m);
mixed * run_pipeline(string func, mixed * args);
mixed run_reduce(string func, mixed * args, mixed acc, string combinator);
string _concat_string(mixed acc, mixed piece);
int _sum_int(mixed acc, mixed piece);
object query_component_by_type(string type);
string query_props_string();
private void rebuild_hook_chains();
private void _sync_component_info();
// void init_original_info();

nomask int query_location() { return 1; }
nomask int query_room() { return 1; }
nomask int query_outside() { return !undefinedp(component_info[LOCATION_COMPONENT_OUTSIDE]); }
nomask int query_maze() { return !undefinedp(component_info[LOCATION_COMPONENT_MAZE]); }

// allow adding and removing objects from the inventory (is a room)
int add_weight(int n) { return 1; }
int test_add(object ob, int flag) { return 1; }
int test_remove(object ob, int flag) { return 1; }

void create() 
{
  components = ({ });
  component_info = ([ ]);
  hook_chains = ([ ]);

  _original_room_file_name = "";
  _original_long = "";
  _original_short = "";
  _original_add_clones = ([ ]);
  _original_items = ({ });
  _specific_long = "";
  _exit_map = ([ ]);

  file_name = "";
  area_path = "";
  map_name = "default";

  coordinates = nil;

  _created_at = 0;
  _last_imported_at = 0;

  light::create();
  property::create();
  contents::create();
  exits::create();
  zone::create();
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
  int i;

  contents::init();
  exits::init();

  // The props component lives in this location's _inventory so the
  // standard `components[i]->init()` call below reaches it through
  // the canonical move/init pass — no manual verb registration or
  // forwarder needed on the location side.
  for (i = 0; i < sizeof(components); i++)
    components[i]->init();
}

// Delegate id matching to the props component when present so that
// `look chair` (and any other parser path that checks `room->id(str)`)
// resolves to a prop instance attached to this location. The base
// returns 0 — the fallback when no props component is attached or the
// str does not match any prop.
int id(string str)
{
  object props_comp;

  props_comp = query_component_by_type(LOCATION_COMPONENT_PROPS);
  if (props_comp && props_comp->id_matches_prop(str))
    return 1;

  return 0;
}

// Movement entry point. Runs the components' do_exit_command pipeline
// (HOOK_PRIORITY_GATE → TRANSFORM → TRACE) and then calls the
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
  return exits::do_exit_command(args[0], args[1], args[2]);
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

string query_specific_long() { return _specific_long; }
void set_specific_long(string str) { _specific_long = str ? str : ""; }

int * query_coordinates() { return coordinates; }
void set_coordinates(int x, int y, int z) { coordinates = ({ x, y, z }); }
void clear_coordinates() { coordinates = nil; }

int query_created_at() { return _created_at; }
int query_last_imported_at() { return _last_imported_at; }
void stamp_last_imported_at() { _last_imported_at = time(); }

string query_map_name() { return map_name; }
void set_map_name(string name) { map_name = name; }

object * query_components() { return components; }
object query_component_by_type(string type)
{
  int i;

  for (i = 0; i < sizeof(components); i++)
    if (components[i]->query_type() == type)
      return components[i];

  return nil;
}

// Seed (or reseed) a component's persisted attributes.
//
// Two callers: (a) the conversion path, which seeds state from a
// legacy room into the location, and (b) builder cmds like `props add`
// that attach a new component on demand. In both cases the live
// instance must be brought up to date so subsequent reads observe the
// seed instead of stale (or missing) data.
//
// If a live component of that type already exists, push the attrs
// into it. Otherwise clone the matching blueprint, initialise it with
// the attrs, register it on the location, and rebuild the hook chains
// so render/dispatch sees it immediately.
void add_component(string component_type, mapping properties)
{
  object live;

  component_info[component_type] = properties;

  live = query_component_by_type(component_type);
  if (live)
  {
    live->init_auto_load_attributes(properties);
    return;
  }

  live = clone_object("/lib/location/components/" + component_type + ".c");
  if (!live) return;

  live->init_auto_load_attributes(properties);
  live->initialize(this_object());
  components += ({ live });
  rebuild_hook_chains();
}

void remove_component(string component_type)
{
  map_delete(component_info, component_type);
}

void init_components(mapping info)
{
  string * component_paths;
  int i;

  component_paths = keys(info);

  for (i = 0; i < sizeof(component_paths); i++)
  {
    object component;
    component = clone_object("/lib/location/components/" + component_paths[i] + ".c");

    if (!component)
      continue;

    // initialize variable values
    component->init_auto_load_attributes(info[component_paths[i]]);
    component->initialize(this_object());
    components += ({ component });
  }

  rebuild_hook_chains();
}

// Walk the current components, ask each which functions it hooks and
// at what priority, group by function name, sort by priority (lower
// first). Ties keep load order — the iteration above is already in
// component_info insertion order.
private void rebuild_hook_chains()
{
  mapping by_func;  // func_name -> ({ ({ priority, component }), ... })
  string * funcs;
  int i, j, k;

  hook_chains = ([ ]);
  by_func = ([ ]);

  for (i = 0; i < sizeof(components); i++)
  {
    mapping hooks;
    string * hook_funcs;

    hooks = components[i]->query_hooks();
    if (!hooks)
      continue;

    hook_funcs = map_indices(hooks);
    for (j = 0; j < sizeof(hook_funcs); j++)
    {
      if (undefinedp(by_func[hook_funcs[j]]))
        by_func[hook_funcs[j]] = ({ });
      by_func[hook_funcs[j]] += ({ ({ hooks[hook_funcs[j]], components[i] }) });
    }
  }

  funcs = map_indices(by_func);
  for (i = 0; i < sizeof(funcs); i++)
  {
    mixed * pairs;
    object * chain;

    // small insertion sort by priority, stable
    pairs = by_func[funcs[i]];
    for (j = 1; j < sizeof(pairs); j++)
    {
      mixed * cur;
      cur = pairs[j];
      k = j - 1;
      while (k >= 0 && pairs[k][0] > cur[0])
      {
        pairs[k + 1] = pairs[k];
        k -= 1;
      }
      pairs[k + 1] = cur;
    }

    chain = ({ });
    for (j = 0; j < sizeof(pairs); j++)
      chain += ({ pairs[j][1] });
    hook_chains[funcs[i]] = chain;
  }
}

// Pipeline dispatcher. Threads args through each registered hook in
// priority order; every hook returns the (possibly transformed) args
// for the next link. Returns the final args; caller is responsible
// for invoking the inherited implementation with them.
mixed * run_pipeline(string func, mixed * args)
{
  object * chain;
  int i;

  if (!hook_chains || undefinedp(hook_chains[func]))
    return args;

  chain = hook_chains[func];
  for (i = 0; i < sizeof(chain); i++)
    args = chain[i]->call_hook(func, args);

  return args;
}

// Reduce dispatcher. Walks the chain in priority order, asks each
// component for its piece (via call_hook), and folds it into the
// accumulator with the named combinator. The accumulator is seeded
// by the caller (typically with the inherited base value).
//
// A component can claim authority by returning ({ HOOK_EXCLUSIVE,
// value }); the combinator detects the tag, discards everything it
// had built so far, and replaces it with the tagged value. Later
// components still run (their side effects fire) but subsequent
// pieces are appended on top of the exclusive value following the
// same combinator rule. If multiple components return exclusive on
// the same call, the first one (lowest priority) wins as a baseline
// and subsequent ones augment it — predictable, matches the old
// "first authoritative component takes the long" behaviour.
//
// Combinator signature: combinator(mixed acc, mixed piece) -> mixed.
// Combinators live as helpers in location.c so the set is small and
// known.
mixed run_reduce(string func, mixed * args, mixed acc, string combinator)
{
  object * chain;
  int i;

  if (!hook_chains || undefinedp(hook_chains[func]))
    return acc;

  chain = hook_chains[func];
  for (i = 0; i < sizeof(chain); i++)
  {
    mixed piece;
    piece = chain[i]->call_hook(func, args);
    acc = call_other(this_object(), combinator, acc, piece);
  }

  return acc;
}

// Combinator: string concatenation. Treats nil/0 pieces as no
// contribution. Honours HOOK_EXCLUSIVE by replacing the accumulator.
string _concat_string(mixed acc, mixed piece)
{
  if (pointerp(piece) && sizeof(piece) == 2 && piece[0] == HOOK_EXCLUSIVE)
    return piece[1];
  if (!piece || !stringp(piece) || !strlen(piece))
    return acc;
  if (!acc) acc = "";
  return acc + piece;
}

// Combinator: integer sum. HOOK_EXCLUSIVE replaces the running total
// with the tagged integer.
int _sum_int(mixed acc, mixed piece)
{
  if (pointerp(piece) && sizeof(piece) == 2 && piece[0] == HOOK_EXCLUSIVE)
    return piece[1];
  if (!intp(piece))
    return acc;
  return acc + piece;
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

  return "%^BOLD%^CYAN%^Components: " + implode(types, ", ") + ".%^RESET%^";
}

// Reduce hook contract for short/long/extra_look:
//   args = ({ ... }), returns the contributed string piece (or "" /
//   nil to contribute nothing). A component can take authority and
//   replace the accumulator by returning ({ HOOK_EXCLUSIVE, piece }).
string short(varargs int dark)
{
  string ret;
  ret = (string)run_reduce("short", ({ dark }), "", "_concat_string");
  if (!ret || !strlen(ret))
    ret = _original_short;
  return ret;
}

string long(varargs string str, int dark)
{
  string ret, aux, props_section, composed;

  if (this_player())
    dark = (int)this_player()->check_dark(query_light());

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
  ret = wrap(ret, (this_user() ? this_user()->query_cols() : 79), 1);

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
    ret += props_section + "\n";

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

  props_comp = query_component_by_type(LOCATION_COMPONENT_PROPS);
  if (!props_comp) return "";

  return props_comp->query_props_section_string();
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
void set_file_name(string name)
{
  string * pieces;
  pieces = explode(name, "/");

  if (sizeof(pieces) >= 2)
    area_path = "/" + implode(pieces[0..sizeof(pieces) - 2], "/") + "/";

  file_name = name;
  save_me();
}

// restore the location from a file
// returns 1 if the file exists and the location was restored, 0 otherwise
int restore_from_file_name(string name)
{
  if (file_size(name) >= 0)
  {
    restore_object(name);

    add_exits_from_exit_map(_exit_map);

    init_components(component_info);

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

  // if (query_coordinates() != nil)
  //   return;

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

// we save the full exit map
// and restore it when the location is loaded
void add_exits_from_exit_map(mapping m)
{ 
  int i;
  string * exit_names;
  string * exits_info;

  exit_names = keys(m);

  for (i = 0; i < sizeof(exit_names); i++)
  {
    exits_info = m[exit_names[i]];

    // first remove the current exit if it exists
    remove_exit(exit_names[i]);

    add_exit(exit_names[i], 
             exits_info[0], 
             exits_info[1], 
             exits_info[2]);
  }
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

object * find_inv_match(string str)
{
  return (object *)all_inventory(this_object()) + 
         (object *)contents::query_hidden_objects();
         // m_values(items);
}

// Refresh component_info from the live components before serialising.
// Pull-on-save closes the symmetric loop with init_components → push
// via init_auto_load_attributes on restore. See the header comment of
// /lib/location/component.c for the full persistence contract.
private void _sync_component_info()
{
  int i;
  for (i = 0; i < sizeof(components); i++)
    component_info[components[i]->query_type()] =
      components[i]->query_auto_load_attributes();
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
  // just the seed values from conversion.
  _sync_component_info();

  // save in the map system every location with coordinates
  if (query_coordinates() != nil)
    MAPS_HANDLER->add_location(this_object());

  // save it in the area system also
  AREA_HANDLER->add_location(this_object());

  if (file_name && strlen(file_name))
    save_object(file_name);
}

void dest_me()
{
  object * arr;
  int i;

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
