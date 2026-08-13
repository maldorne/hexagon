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

// Persisted into the NPC's own savefile so a restored NPC knows who it is.
string npc_uuid;         // census identity; nil/"" => not a persisted NPC
string npc_game;         // game slug, for the savefile path
string npc_area_path;    // owning area (census controller)
string npc_poi;          // owning point of interest, if any
string npc_source;       // blueprint path this NPC was spawned from; the
                         // bestiary-template identity and census key. Lets the
                         // NPC describe itself without a census lookup.
string * npc_categories; // coarse types (aggressive/animal/citizen/...); a
                         // single NPC can carry several. Empty => derived.
mapping npc_auto_load;   // the NPC's carried inventory, encoded for save_object
                         // (same shape as a player's auto_load)

void create()
{
  monster::create();

  npc_uuid = nil;
  npc_game = nil;
  npc_area_path = nil;
  npc_poi = nil;
  npc_source = nil;
  npc_categories = ({ });
  npc_auto_load = ([ ]);
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

void set_npc_categories(string * a) { npc_categories = a ? a : ({ }); }
void add_npc_category(string s)
{
  if (member_array(s, npc_categories) < 0)
    npc_categories += ({ s });
}

string * query_npc_categories()
{
  if (sizeof(npc_categories))
    return npc_categories[..];

  // No explicit categories: derive a coarse default. Aggressive NPCs read as
  // aggressive, everything else as pacific. The finer buckets
  // (animal/citizen/guard) are set explicitly by the blueprint or generator.
  if (this_object()->query_aggressive())
    return ({ NPC_CATEGORY_AGGRESSIVE });

  return ({ NPC_CATEGORY_PACIFIC });
}

// convenience membership test
int is_npc_category(string s)
{
  return member_array(s, query_npc_categories()) >= 0;
}

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
  // rebuild the saved inventory (each item cloned and its attributes applied)
  // inside us, so a restored NPC carries exactly what it was saved with
  if (ok)
    load_auto_load(npc_auto_load, this_object());
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
  if (query_persisted() && query_dead())
  {
    if (npc_area_path)
    {
      object area;
      area = AREA_HANDLER->query_area(npc_area_path);
      if (area)
        catch(area->npc_died(npc_uuid));
    }
    delete_npc_save();
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

void apply_template(mapping t)
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
  if (t["race_ob"])
    set_race_ob(t["race_ob"]);
  if (t["class_ob"])
    set_class_ob(t["class_ob"]);
  if (t["level"])
    set_level(t["level"]);
  if (t["align"])
    set_real_align(t["align"]);
  // weight is not applied here: set_race_ob above already set the body weight
  // from the race, which is where it belongs
}
