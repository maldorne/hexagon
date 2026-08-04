// persisted.c — dormant persisted-NPC capability, mixed into monster.c.
//
// A hand-authored or generated mob only becomes a persisted NPC once an area
// gives it an identity (set_npc_uuid + set_npc_area_path + set_npc_game).
// Until then every method here is inert: no savefile is written, no census is
// notified. This lets any existing blueprint be area-driven without changing
// its inherit chain, and leaves plain room mobs completely untouched.

#include <living/npc_persisted.h>
#include <areas/area.h>

// Persisted into the NPC's own savefile so a restored NPC knows who it is.
string npc_uuid;        // census identity; nil/"" => not a persisted NPC
string npc_game;        // game slug, for the savefile path
string npc_area_path;   // owning area (census controller)
string npc_poi;         // owning point of interest, if any
string npc_category;    // explicit coarse type; nil => derived on query

void create()
{
  npc_uuid = nil;
  npc_game = nil;
  npc_area_path = nil;
  npc_poi = nil;
  npc_category = nil;
}

int query_persisted() { return npc_uuid && strlen(npc_uuid); }

string query_npc_uuid() { return npc_uuid; }
void set_npc_uuid(string s) { npc_uuid = s; }

string query_npc_game() { return npc_game; }
void set_npc_game(string s) { npc_game = s; }

string query_npc_area_path() { return npc_area_path; }
void set_npc_area_path(string s) { npc_area_path = s; }

string query_npc_poi() { return npc_poi; }
void set_npc_poi(string s) { npc_poi = s; }

void set_npc_category(string s) { npc_category = s; }
string query_npc_category()
{
  if (npc_category && strlen(npc_category))
    return npc_category;

  // No explicit category: derive a coarse default. Aggressive mobs read as
  // aggressive, everything else as pacific. The finer buckets
  // (animal/citizen/guard) are set explicitly by the blueprint or generator.
  if (this_object()->query_aggressive())
    return NPC_CATEGORY_AGGRESSIVE;

  return NPC_CATEGORY_PACIFIC;
}

// Persist this NPC to its own savefile. Inert for a non-persisted mob. The
// area drives when this runs (on location unload); F0 only provides the
// mechanism.
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
// (uuid + game) first. Inert for a non-persisted mob.
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

// Called from monster::do_death once the NPC has actually died: notify the
// owning area census and drop the savefile. The diplomacy controller
// (city_ob) is already notified by living::do_death, so it is not repeated
// here. AREA_HANDLER->npc_died lands in F1; the catch keeps this a safe no-op
// until then.
void persisted_npc_died()
{
  if (!query_persisted())
    return;

  if (npc_area_path)
    catch(AREA_HANDLER->create_area(npc_area_path)->npc_died(npc_uuid));

  delete_npc_save();
}
