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

inherit monster "/lib/monster.c";

// Persisted into the NPC's own savefile so a restored NPC knows who it is.
string npc_uuid;         // census identity; nil/"" => not a persisted NPC
string npc_game;         // game slug, for the savefile path
string npc_area_path;    // owning area (census controller)
string npc_poi;          // owning point of interest, if any
string * npc_categories; // coarse types (aggressive/animal/citizen/...); a
                         // single NPC can carry several. Empty => derived.

void create()
{
  monster::create();

  npc_uuid = nil;
  npc_game = nil;
  npc_area_path = nil;
  npc_poi = nil;
  npc_categories = ({ });
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
  return !catch(save_object(dir + NPC_SAVE_FILE, 1));
}

// Restore this NPC's state from its savefile. The caller sets the identity
// (uuid + game) first. Inert for a non-persisted NPC.
int restore_npc()
{
  if (!query_persisted() || !npc_game)
    return 0;

  return restore_object(npc_save_dir(npc_game, npc_uuid) + NPC_SAVE_FILE, 1);
}

// Delete the savefile (on death). Leaves the letter/uuid folders behind; an
// area verify pass can prune empties later.
void delete_npc_save()
{
  if (!query_persisted() || !npc_game)
    return;

  catch(remove_file(npc_save_dir(npc_game, npc_uuid) + NPC_SAVE_FILE));
}

// Called once the NPC has actually died (see do_death): notify the owning area
// census and drop the savefile. The diplomacy controller (city_ob) is already
// notified by living::do_death, so it is not repeated here.
void persisted_npc_died()
{
  if (!query_persisted())
    return;

  if (npc_area_path)
    catch(AREA_HANDLER->create_area(npc_area_path)->npc_died(npc_uuid));

  delete_npc_save();
}

// ---------------------------------------------------------------------------
// Death and data template
// ---------------------------------------------------------------------------

// Once actually dead, free the census slot and drop the savefile (this lives
// here, not on monster.c, so plain mobs do not carry it).
int do_death(varargs object killer)
{
  int r;

  r = monster::do_death(killer);
  if (this_object() && query_dead())
    persisted_npc_died();

  return r;
}

// Apply a bestiary data template (a plain mapping of field -> value) onto this
// generic NPC. The set of fields carried is deliberately explicit; extend it
// (here and in bestiary::extract_template) as more of the source's data needs
// to survive the room2loc conversion.
// A gendered field is either a single value or a per-gender wrapper
// ({ nil, value_for_gender_1, value_for_gender_2 }) written by the bestiary
// for a source that varies by gender. The nil sentinel at index 0 (no living
// is neuter) marks the wrapper apart from a plain list value (aliases /
// plurals are themselves arrays). Resolve it against the caller's gender.
private mixed gender_value(mixed v, int g)
{
  if (pointerp(v) && sizeof(v) == 3 && v[0] == nil)
    return v[g];
  return v;
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
  if (t["weight"])
    set_weight(t["weight"]);
}
