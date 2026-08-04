// persisted.c — dormant persisted-NPC capability, mixed into monster.c.
//
// A hand-authored or generated mob only becomes a persisted NPC once an area
// gives it an identity (set_npc_uuid + set_npc_area_path + set_npc_game).
// Until then every method here is inert: no savefile is written, no census is
// notified. This lets any existing blueprint be area-driven without changing
// its inherit chain, and leaves plain room mobs completely untouched.

#include <living/persisted.h>
#include <areas/area.h>

// Persisted into the NPC's own savefile so a restored NPC knows who it is.
string npc_uuid;         // census identity; nil/"" => not a persisted NPC
string npc_game;         // game slug, for the savefile path
string npc_area_path;    // owning area (census controller)
string npc_poi;          // owning point of interest, if any
string * npc_categories; // coarse types (aggressive/animal/citizen/...); a
                         // single NPC can carry several. Empty => derived.

// Saved shadows of the static display / id fields. name/short/long, the
// alias & plural lists and the main plural all live in static vars, so a
// save_object template would not carry them; capture_for_template copies
// static -> shadow before a snapshot and apply_from_template copies
// shadow -> static after a template is restored into a generic mob.
string _tmpl_name, _tmpl_short, _tmpl_long, _tmpl_main_plural;
string * _tmpl_aliases, * _tmpl_plurals;

// defined below; used by save_npc / restore_npc before their definition
private void _capture_display_shadows();
private void _apply_display_shadows();

void create()
{
  npc_uuid = nil;
  npc_game = nil;
  npc_area_path = nil;
  npc_poi = nil;
  npc_categories = ({ });
  _tmpl_name = nil;
  _tmpl_short = nil;
  _tmpl_long = nil;
  _tmpl_main_plural = nil;
  _tmpl_aliases = nil;
  _tmpl_plurals = nil;
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

  // No explicit categories: derive a coarse default. Aggressive mobs read as
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
  // fold the static display fields into saved shadows so they survive the save
  _capture_display_shadows();
  return !catch(save_object(dir + NPC_SAVE_FILE, 1));
}

// Restore this NPC's state from its savefile. The caller sets the identity
// (uuid + game) first. Inert for a non-persisted mob.
int restore_npc()
{
  int r;

  if (!query_persisted() || !npc_game)
    return 0;

  r = restore_object(npc_save_dir(npc_game, npc_uuid) + NPC_SAVE_FILE, 1);
  // restore the static display fields from their saved shadows
  _apply_display_shadows();
  return r;
}

// name/short/long, the alias & plural lists and the main plural live in static
// vars, which save_object does not persist. These helpers shadow them into the
// saved _tmpl_* vars around every save/restore (both templates and savefiles),
// so a generic mob rebuilt from data still shows and matches as its original.
private void _capture_display_shadows()
{
  _tmpl_name = this_object()->query_name();
  _tmpl_short = this_object()->query_short();
  _tmpl_long = this_object()->query_long();
  _tmpl_main_plural = this_object()->query_main_plural();
  _tmpl_aliases = this_object()->query_alias();
  _tmpl_plurals = this_object()->query_plurals();
}

private void _apply_display_shadows()
{
  if (_tmpl_name)
    this_object()->set_name(_tmpl_name);
  if (_tmpl_short)
    this_object()->set_short(_tmpl_short);
  if (_tmpl_long)
    this_object()->set_long(_tmpl_long);
  if (_tmpl_main_plural)
    this_object()->set_main_plural(_tmpl_main_plural);
  if (_tmpl_aliases)
    this_object()->set_aliases(_tmpl_aliases);
  if (_tmpl_plurals)
    this_object()->set_plurals(_tmpl_plurals);
}

// Save this mob's data as a bestiary template (whole-object snapshot plus the
// display shadows). restore_template rebuilds a generic mob from one.
int save_template(string file)
{
  _capture_display_shadows();
  return !catch(save_object(file, 1));
}

int restore_template(string file)
{
  int r;

  r = restore_object(file, 1);
  _apply_display_shadows();
  return r;
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
