// npc.c — a full NPC: a monster plus the area-managed persistence layer.
//
// This is what the area system spawns. As a generic clone it has an empty
// setup() and takes all its data from a bestiary data template
// (apply_template); hand-authored NPCs that need persistence / area membership
// inherit this instead of /lib/monster.c. Plain /lib/monster.c stays lean:
// none of the persisted-NPC state lives on it.
//
// The persistence layer gives an area-managed NPC its census identity, its own
// savefile for mutable state, and death cleanup. It is inert until an area
// stamps an identity (set_npc_uuid + set_npc_area_path + set_npc_game).

#include <living/persisted.h>
#include <areas/area.h>

inherit monster   "/lib/monster.c";
// Inventory persistence, the same mixin players use: create_auto_load snapshots
// the carried items into a saveable map, load_auto_load rebuilds them. This is
// what lets an NPC keep its own equipment on its npc.o across reboots.
inherit autoload  "/lib/core/basic/auto_load.c";

// Behaviour components (schedule, shop, patrol, ...) are attached per NPC. The
// host machinery lives inline below, exactly as the location component host
// lives inline in location.c -- the only component *file* is the base every
// component inherits, /lib/npc/component.c. Concrete components sit under here:
#define NPC_COMPONENT_DIR "/lib/npc/components/"

// Persisted into the NPC's own savefile so a restored NPC knows who it is.
string npc_uuid;         // census identity; nil/"" => not a persisted NPC
string npc_game;         // game slug, for the savefile path
string npc_area_path;    // owning area (census controller)
string npc_poi;          // owning point of interest, if any
string npc_source;       // blueprint path this NPC was spawned from; the
                         // bestiary-template identity and census key. Lets the
                         // NPC describe itself without a census lookup.
mapping npc_auto_load;   // the NPC's carried inventory, encoded for save_object
                         // (same shape as a player's auto_load)
string npc_given_name;   // a generated citizen's proper name (lowercase).
                         // id.c's `name` is static (never saved), so the one bit
                         // of a generated NPC's identity that is not code/template
                         // derived lives here, in the npc.o. Language-neutral (a
                         // proper noun does not translate). Nil for template NPCs,
                         // whose name comes from apply_template.

// Component host state. component_info (type -> persisted attrs) rides in the
// npc.o savefile; the live instances are static and re-cloned on restore.
mapping component_info;
static object * components;

// Where this NPC lives: a house location file (the home the area assigned it),
// or nil. Persisted in npc.o. The schedule component walks the NPC here at night
// via travel_to(query_home()). Just a location for now -- a family shares one.
string npc_home;

// For an anonymous monster (no uuid): the location whose bucket counts it, set
// when it is cloned. It is bookkeeping, not identity -- the animal may wander
// off, and on death it still decrements the bucket it was counted in.
string npc_monster_location;

// This individual's concrete workplace location file, resolved by the schedule
// component's "work" symbol. Assigned once and persisted in npc.o (the type-level
// timetable is generic; where this NPC actually works is per-individual). The
// area that rosters this NPC (population caps, scheduler) is npc_area_path -- the
// census controller -- and stays fixed even when the NPC walks into another area;
// where it physically is (its position) is tracked in the areas handler.
string npc_work;

void create()
{
  monster::create();

  npc_uuid = nil;
  npc_game = nil;
  npc_area_path = nil;
  npc_poi = nil;
  npc_source = nil;
  npc_auto_load = ([ ]);
  npc_given_name = nil;
  npc_home = nil;
  npc_work = nil;
  npc_monster_location = nil;
  component_info = ([ ]);
  components = ({ });
}

string query_home() { return npc_home; }
void set_home(string file) { npc_home = file; }

string query_monster_location() { return npc_monster_location; }
void set_monster_location(string file) { npc_monster_location = file; }

string query_work() { return npc_work; }
void set_work(string file) { npc_work = file; }

// ---------------------------------------------------------------------------
// Component host
// ---------------------------------------------------------------------------
//
// Attach behaviour components (schedule, shop, patrol, ...) to this NPC.
// Mirrors the location component host (inline in location.c), pared down: NPC
// components react to events with side effects, so this provides attach /
// detach / query and a plain broadcast (run_on_components). It omits the
// location system's hook-chain / pipeline / reduce machinery, which exists to
// transform a location's return values -- nothing an NPC component needs yet.
// Persistence is driven by save_npc / restore_npc below. Per-component half:
// /lib/npc/component.c.

object * query_components() { return components ? components : ({ }); }

object query_component_by_type(string type)
{
  int i;

  if (!components)
    return nil;
  for (i = 0; i < sizeof(components); i++)
    if (components[i]->query_type() == type)
      return components[i];
  return nil;
}

int has_component(string type) { return query_component_by_type(type) != nil; }

// Clone a component blueprint, stamp its type, seed its attrs, and bind it to
// this NPC. Shared by add_component and init_components.
private object _spawn_component(string type, mapping attrs)
{
  object c;

  c = clone_object(NPC_COMPONENT_DIR + type + ".c");
  if (!c)
    return nil;

  c->set_type(type);
  c->init_auto_load_attributes(attrs ? attrs : ([ ]));
  c->initialize(this_object());
  components += ({ c });
  return c;
}

// Re-clone every component named in a restored component_info (from restore_npc).
void init_components(mapping info)
{
  string * types;
  int i;

  if (!components)
    components = ({ });
  if (!info)
    return;

  types = map_indices(info);
  for (i = 0; i < sizeof(types); i++)
    _spawn_component(types[i], info[types[i]]);
}

// Attach a component of `type` with the given attrs, or reseed a live one. This
// is how a role stamps behaviour on a freshly materialised NPC.
void add_component(string type, mapping attrs)
{
  object live;

  if (!components)
    components = ({ });
  if (!component_info)
    component_info = ([ ]);
  if (!attrs)
    attrs = ([ ]);

  component_info[type] = attrs;

  live = query_component_by_type(type);
  if (live)
  {
    live->init_auto_load_attributes(attrs);
    return;
  }

  _spawn_component(type, attrs);
}

void remove_component(string type)
{
  object live;

  if (component_info)
    map_delete(component_info, type);

  live = query_component_by_type(type);
  if (live)
  {
    components -= ({ live });
    destruct(live);
  }
}

// Pull each live component's current attrs back into component_info so
// save_object persists up-to-date state. Called from save_npc before saving.
void sync_component_info()
{
  int i;

  if (!components)
    return;
  if (!component_info)
    component_info = ([ ]);

  for (i = 0; i < sizeof(components); i++)
    component_info[components[i]->query_type()] =
      components[i]->query_auto_load_attributes();
}

// Broadcast an event to every component. Each component implements only the
// event_<name> methods it cares about; an undefined one returns nil harmlessly.
// Side effects only -- no return folding.
void run_on_components(string func, mixed * args)
{
  int i;

  if (!components)
    return;
  for (i = 0; i < sizeof(components); i++)
    call_other(components[i], func, args);
}

// Guard-role proxy. The room's exit handler consults a posted guard through
// guardian_check / guardian_message on the NPC object; delegate to the guard
// component when this NPC carries one. An NPC without it blocks nobody, so a
// non-guard consulted by mistake simply lets everyone through.
int guardian_check(object mover)
{
  object g;
  g = query_component_by_type("guard");
  return g ? g->check(mover) : 1;
}

string guardian_message()
{
  object g;
  g = query_component_by_type("guard");
  return g ? g->message() : nil;
}

// Forward weather / climate events to this NPC's components (a climate component
// can react to rain, cold, etc.).
void event_weather(object who, varargs int flag, int * values)
{
  run_on_components("event_weather", ({ who, flag }));
}

// The areas handler calls this at a schedule hour: hand it to the schedule
// component (if any), which walks the NPC to where it should be this hour.
void do_schedule(int hour)
{
  run_on_components("do_schedule", ({ hour }));
}

// Give this NPC a generated proper name: store it (persisted in npc.o) and set
// it as the engine name (the find_living id), lowercased. Call on a freshly
// cloned NPC, before any template names it -- monster::set_name only takes the
// first name. A generated citizen's short/long still come from the template.
void set_given_name(string s)
{
  // stored lowercase, like every name in a savefile / the find_living id;
  // capitalise it only where it is displayed
  npc_given_name = s ? lower_case(s) : s;
  if (npc_given_name && strlen(npc_given_name))
    set_name(npc_given_name);
}

string query_given_name() { return npc_given_name; }

// A generated citizen is shown by its kind, not its proper name, wherever
// query_cap_name drives the display -- room / glance lists, combat and death
// messages, emotes -- so it never reads like a player. Its proper name
// (npc_given_name) stays the find_living id and targeting key, and is surfaced
// only when you examine it. Only NPCs we gave a generated name are affected;
// every other object (fauna, uniques, players) keeps the default cap_name.
string query_cap_name()
{
  if (npc_given_name && strlen(npc_given_name))
    return query_short();
  return ::query_cap_name();
}

// When you examine a generated citizen directly its short reveals the proper
// name -- "<kind> (<Name>)" -- while query_cap_name above keeps room lists and
// combat on the bare kind. `short()` is what the look command prints as the
// examine header; query_short (the raw kind) is left untouched so cap_name and
// the plural stay clean.
string short(varargs int dark)
{
  if (npc_given_name && strlen(npc_given_name))
    return query_short() + " (" + capitalize(npc_given_name) + ")";
  return ::short(dark);
}

// The room-contents grouping keys on pretty_short. Keep it on the bare kind so
// same-kind citizens collapse into one line (a count plus the plural) instead
// of each listing separately -- the name lives only on short() (the examine
// header) and must not leak into the grouping key.
string pretty_short(varargs int dark)
{
  if (npc_given_name && strlen(npc_given_name))
    return query_short();
  return ::pretty_short(dark);
}

// Generic NPC: no hard-coded content. A hand-authored subclass overrides this.
void setup()
{
}

int query_npc() { return 1; }

// ---------------------------------------------------------------------------
// Persisted-NPC identity and census
// ---------------------------------------------------------------------------

int query_persisted() { return npc_uuid && strlen(npc_uuid); }

string query_npc_uuid() { return npc_uuid; }
void set_npc_uuid(string s) { npc_uuid = s; }

string query_npc_game() { return npc_game; }
void set_npc_game(string s) { npc_game = s; }

string query_npc_area_path() { return npc_area_path; }
void set_npc_area_path(string s) { npc_area_path = s; }

string query_npc_poi() { return npc_poi; }
void set_npc_poi(string s) { npc_poi = s; }

string query_npc_source() { return npc_source; }
void set_npc_source(string s) { npc_source = s; }

// ---------------------------------------------------------------------------
// Persisted-NPC savefile (mutable per-NPC state)
// ---------------------------------------------------------------------------

// Persist this NPC to its own savefile. Inert for a non-persisted NPC; the
// area drives when this runs (on location unload).
int save_npc()
{
  string dir;

  if (!query_persisted() || !npc_game)
    return 0;

  dir = npc_save_dir(npc_game, npc_uuid);
  // save_object does not create directories; mkdir is recursive here
  mkdir(dir);
  // snapshot the carried inventory (weapons, armour, clothes, anything) into
  // the auto-load map so save_object persists it -- exactly as a player saves
  // its inventory
  npc_auto_load = create_auto_load(all_inventory(this_object()));
  // refresh each live component's persisted attrs into component_info so
  // save_object writes their current state
  sync_component_info();
  return !catch(save_object(dir + NPC_SAVE_FILE, 1));
}

// Restore this NPC's state from its savefile. The caller sets the identity
// (uuid + game) first. Inert for a non-persisted NPC.
int restore_npc()
{
  int ok;

  if (!query_persisted() || !npc_game)
    return 0;

  ok = restore_object(npc_save_dir(npc_game, npc_uuid) + NPC_SAVE_FILE, 1);
  if (ok)
  {
    // rebuild the saved inventory (each item cloned and its attributes applied)
    // inside us, so a restored NPC carries exactly what it was saved with
    load_auto_load(npc_auto_load, this_object());
    // id.c's `name` is static and was not restored; re-seed the find_living id
    // from the saved given name so a restored citizen answers to itself again
    if (npc_given_name && strlen(npc_given_name))
      set_name(lower_case(npc_given_name));
    // re-clone the NPC's behaviour components from the restored component_info
    init_components(component_info);
  }
  return ok;
}

// Delete the NPC's entire save folder on death: every file in it, then the
// folder itself. An NPC may accumulate several save files over time (as a
// player's folder does), so remove them all -- not just npc.o -- and leave no
// empty directory behind. Done inline within the NPC's own uuid folder on
// purpose: there is no general recursive-delete efun (too easy to misuse), and
// this only ever touches this NPC's folder.
void delete_npc_save()
{
  string dir;
  string * files;
  int i;

  if (!query_persisted() || !npc_game)
    return;

  dir = npc_save_dir(npc_game, npc_uuid);
  files = (string *)get_dir(dir + "*");
  for (i = 0; i < sizeof(files); i++)
    catch(remove_file(dir + files[i]));
  catch(rmdir(dir));
}

// ---------------------------------------------------------------------------
// Death and data template
// ---------------------------------------------------------------------------

// Free the census slot and POI vacancy, and drop the savefile, when this NPC
// dies. This runs from dest_me, not do_death: living::actual_death destructs
// the body, and a destructed object can no longer call_other, so the area
// must be notified from dest_me -- the last moment the NPC is still alive,
// just before ::dest_me() destroys it.
//
// dest_me also runs on a plain eviction/unload, where the NPC is not dead and
// its census entry must survive so it returns on the next load; the
// query_dead() guard (set by living::actual_death before it destructs us)
// tells the two apart. The diplomacy controller (city_ob) is already notified
// inside living::do_death, so it is not repeated here.
void dest_me()
{
  if (query_dead() && npc_area_path)
  {
    object area;

    area = AREA_HANDLER->query_area(npc_area_path);

    if (query_persisted())
    {
      // an individual: free its census row and remove its save folder
      if (area)
        catch(area->npc_died(npc_uuid));
      delete_npc_save();
    }
    else if (area && npc_source && npc_monster_location)
    {
      // an anonymous monster: take one off the bucket it was counted in
      catch(area->monster_died(npc_source, npc_monster_location));
    }
  }

  ::dest_me();
}

// Apply a bestiary data template (a plain mapping of field -> value) onto this
// generic NPC. The set of fields carried is deliberately explicit; extend it
// (here and in bestiary::extract_template) as more of the source's data needs
// to survive the room2loc conversion.
// A gendered field is either a single value or a per-gender wrapper: a mapping
// keyed by gender ([ gender: value ]) written by the bestiary for a source that
// varies by gender. Being a mapping marks it apart from a plain value (aliases
// and plurals are themselves lists). Keys are strings after the JSON round-trip
// (gender ids 0 neuter / 1 male / 2 female). Resolve against the caller's
// gender, falling back to male then any present value so a gender the field
// does not cover still yields a coherent string.
private mixed gender_value(mixed v, int g)
{
  string * ks;

  if (!mappingp(v))
    return v;

  if (v["" + g] != nil)
    return v["" + g];
  if (v["1"] != nil)
    return v["1"];

  ks = map_indices(v);
  return sizeof(ks) ? v[ks[0]] : nil;
}

/**
 * Stamp a data template onto this NPC.
 *
 * The template is re-applied on every materialization, not only the first,
 * because the fields it carries are held in variables save_object never
 * writes: name, aliases and plurals are static in the id code, short, long
 * and the main plural are static in the description code. A restored NPC
 * would otherwise come back with no name and no description at all. Stamping
 * every time also means editing a template updates every existing NPC of that
 * type, which is the point of keeping the type as data.
 *
 * `born` says this is the NPC's first materialization, and separates the two
 * halves of a template. Everything above the guard is presentation: not
 * persisted, so it has to be re-read each time. Everything below it is the
 * body -- race, class and a pinned level -- which save_object does keep, so
 * re-applying it on a restore ranges from pointless to destructive:
 * set_class_ob resets the level, the experience and the strength cap, which is
 * right when a living changes class and wrong when it is merely waking up.
 */
void apply_template(mapping t, varargs int born)
{
  int g;

  if (!t)
    return;

  // A fixed template carries its gender; a bimodal one leaves the choice to
  // the caller (set before apply), so read whatever gender is in effect and
  // index the per-gender fields by it.
  if (t["gender"])
    set_gender(t["gender"]);
  g = query_gender();

  if (t["name"])
    set_name(gender_value(t["name"], g));
  if (t["short"])
    set_short(gender_value(t["short"], g));
  if (t["long"])
    set_long(gender_value(t["long"], g));
  if (t["main_plural"])
    set_main_plural(gender_value(t["main_plural"], g));
  if (t["aliases"])
    set_aliases(gender_value(t["aliases"], g));
  if (t["plurals"])
    set_plurals(gender_value(t["plurals"], g));
  if (t["align"])
    set_real_align(t["align"]);

  if (!born)
    return;

  if (t["race_ob"])
    set_race_ob(t["race_ob"]);
  if (t["class_ob"])
    set_class_ob(t["class_ob"]);
  if (t["level"])
    set_level(t["level"]);
  // weight is not applied here: set_race_ob above already set the body weight
  // from the race, which is where it belongs
}
