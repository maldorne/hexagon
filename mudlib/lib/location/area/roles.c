
// The role board: the jobs a settlement staffs.
//
// A role is a named post -- barman, mayor, farmer -- that the town wants filled
// by some number of interchangeable citizens. The board holds only the cap: how
// many, where the work is done, and which type they are drawn from. What those
// citizens are like is not here; that belongs to the type template. Who
// concretely fills a slot is not here either; those are ordinary census
// individuals, tagged with the role they hold.
//
// A role slot deliberately does not respawn on death. The person is gone; the
// post is refilled later by the settlement pass, with somebody else.

#include <room/location.h>
#include <living/persisted.h>
#include <basic/gender.h>
#include <namegen.h>

// The settlement's role board: the
// named jobs a town staffs with sentient citizens -- barman, mayor, guards,
// farmers -- each with a count and a real work location. A role slot is a
// census entry tagged "role" with no "poi"/"guard", so it does NOT auto-respawn
// like a POI vacancy: a slot emptied by death is refilled by the settlement
// pass (fill_area_roles), which the F6 prosperity tick will drive. For now the
// slots are filled from a transitional template `source`; the F4 generator
// replaces that behind assign_npc_to_role.
//   ([ role_name : ([ "count": n, "work": location_file,
//                     "source": template_id, "sentient": 1 ]) ])
mapping roles;

void create()
{
  roles = ([ ]);
}

// ---------------------------------------------------------------------------
// Role board
// ---------------------------------------------------------------------------
//
// A role is a named job (barman, mayor, guard, farmer) the settlement staffs
// with sentient citizens: a count and a real work location. A role slot is a
// census entry tagged "role" with no "poi"/"guard" -- so it never auto-respawns
// like a POI vacancy; a slot emptied by death is refilled by fill_area_roles
// (the settlement pass). Slots are filled from a transitional template `source`
// for now; the F4 generator will replace assign_npc_to_role's body, the single
// seam where the filler lives.

// The role board this area works from -- its community's, when it delegates. A
// job belongs to the people who hold it, not to the field they hold it in.
mapping query_roles()
{
  object owner;

  owner = (object)this_object()->query_root_area();
  return owner == this_object() ? roles : (mapping)owner->query_roles();
}

mapping query_role(string name) { return query_roles()[name]; }

// Declare (or replace) a role: the area-level cap for a kind of citizen. `count`
// is how many of it the settlement wants, `work` its work location, `source` the
// NPC blueprint filling its slots (snapshotted to a template now, the one time
// its .c is read). Stores the template id. Behaviour (sentient, equipment,
// timetable) is not stored here -- it lives on the type template, written by the
// builder through the bestiary; the area only holds the count and the workplace.
void add_role(string name, int count, string work, string source)
{
  object owner;

  if (!name || !strlen(name) || count < 0)
    return;

  // The job belongs to the community, so a delegated area posts it upwards.
  owner = (object)this_object()->query_root_area();
  if (owner != this_object())
  {
    owner->add_role(name, count, work, source);
    return;
  }

  if (source && strlen(source))
  {
    string game;
    game = game_from_path((string)this_object()->query_area_path());
    if (!BESTIARY_HANDLER->has_template(game, source))
      BESTIARY_HANDLER->add_template(source);
    source = (string)this_object()->query_template_from_source(source);
  }

  roles[name] = ([ "count":  count,
                   "work":   work,
                   "source": source ]);
  this_object()->save_me();
}

// Give one NPC the kit a role defines: monster::add_clone clones each blueprint
// straight into it, then init_equip wears / wields the lot. Used both when a
// role NPC first materializes and when a live role's kit is changed.
void equip_npc(object npc, string * paths)
{
  int i;

  if (!npc || !pointerp(paths) || !sizeof(paths))
    return;
  for (i = 0; i < sizeof(paths); i++)
    npc->add_clone(paths[i], 1);
  npc->init_equip();
}

// Resolve a role's equipment spec into one concrete kit. The spec is an array
// of slots; each slot is an array of interchangeable blueprints and one is
// picked at random (a fixed item is just a one-element slot). This is rolled
// ONCE per NPC at assignment and stored on the census entry, so a citizen keeps
// the same weapon for life instead of re-rolling every time it materializes.
string * resolve_equipment(mixed * spec)
{
  string * kit;
  int i;

  kit = ({ });
  if (!pointerp(spec))
    return kit;

  for (i = 0; i < sizeof(spec); i++)
  {
    mixed slot;
    slot = spec[i];
    if (pointerp(slot) && sizeof(slot))
      kit += ({ slot[random(sizeof(slot))] });
    else if (stringp(slot))
      kit += ({ slot });
  }

  return kit;
}

// Re-roll and equip the kit for this role's live holders that still carry
// nothing, reading the kit from the type template (its authoritative home for
// equipment). A holder that already has gear keeps it -- a saved citizen's gear
// never changes. Called after the builder changes a role's kit on the template,
// so existing empty-handed holders pick it up without a respawn. New holders
// roll their kit at first materialization (npc_restore, from the template).
void reequip_role_holders(string name)
{
  object loc;
  object * inv;
  mapping template;
  mixed * spec;
  string source;
  int i;

  if (!query_roles()[name])
    return;

  source = query_roles()[name]["source"];
  template = (source && strlen(source))
               ? BESTIARY_HANDLER->query_template(game_from_path((string)this_object()->query_area_path()), source)
               : nil;
  spec = template ? template["equipment"] : nil;
  if (!pointerp(spec) || !sizeof(spec))
    return;

  loc = (object)this_object()->query_loaded_location(query_roles()[name]["work"]);
  if (!loc)
    return;
  inv = all_inventory(loc);
  for (i = 0; i < sizeof(inv); i++)
  {
    string uuid;
    mapping e;

    if (!inv[i] || !inv[i]->query_persisted())
      continue;
    uuid = inv[i]->query_npc_uuid();
    e = uuid ? ((mapping)this_object()->query_npc_census())[uuid] : nil;
    if (!e || e["role"] != name || e["poi"] || e["guard"])
      continue;
    // gear a holder that has none yet (created before the kit was set); one
    // that already carries gear keeps it -- a saved citizen's gear never
    // changes. The kit is rolled, equipped and saved onto the NPC's own npc.o.
    if (sizeof(all_inventory(inv[i])))
      continue;
    equip_npc(inv[i], resolve_equipment(spec));
    inv[i]->save_npc();
  }
}

// Where the next holder of `name` should work.
//
// A role records one work location -- wherever the builder stood when adding it
// -- but a job is rarely done in a single spot: a farm is a set of fields, a
// market a row of stalls. So its holders are spread over every location of the
// area that carries the same working component as the role's own spot, handed
// out in turn. With one such location, or none, the recorded spot is used and
// nothing changes.
//
// Deciding here rather than on the individual is what makes it survive death:
// the replacement is placed by the same rule instead of inheriting whatever
// spot the last one happened to hold.
string work_spot_for(string name, int nth)
{
  mapping role;
  object seat;
  object * comps;
  string * kinds, * files, * candidates;
  int i, j;

  role = query_role(name);
  if (!role || !role["work"])
    return "";

  seat = (object)this_object()->load_location(role["work"]);
  if (!seat)
    return role["work"];

  // what makes that spot a workplace, ignoring what every open-air location has
  kinds = ({ });
  comps = seat->query_components();
  for (i = 0; i < sizeof(comps); i++)
    if (comps[i]->query_type() != LOCATION_COMPONENT_OUTSIDE)
      kinds += ({ comps[i]->query_type() });

  if (!sizeof(kinds))
    return role["work"];

  candidates = ({ });
  files = map_indices((mapping)this_object()->query_locations());
  for (i = 0; i < sizeof(files); i++)
  {
    object loc;

    loc = (object)this_object()->load_location(files[i]);
    if (!loc)
      continue;
    for (j = 0; j < sizeof(kinds); j++)
      if (loc->query_component_by_type(kinds[j]))
      {
        candidates += ({ files[i] });
        break;
      }
  }

  if (sizeof(candidates) < 2)
    return role["work"];

  return candidates[nth % sizeof(candidates)];
}

// Live count of a role's staff: census entries tagged with this role that are
// role-board slots (no POI, no guard), so this never counts a POI vacancy or a
// guard that happens to share the role name.
int count_role_npcs(string name)
{
  string * ids;
  int i, n;

  ids = map_indices((mapping)this_object()->query_npc_census());
  for (i = 0; i < sizeof(ids); i++)
  {
    mapping e;
    e = ((mapping)this_object()->query_npc_census())[ids[i]];
    if (e["role"] == name && !e["poi"] && !e["guard"])
      n++;
  }

  return n;
}

// A generated given-name (lowercase) for one of this area's citizens: the name
// generator draws it from the area citizenship's name style, in the form
// matching the given gender (a GENDER_* id). It does not depend on the role.
// Returns nil when the area has no citizenship, the citizenship declares no
// name style, or the generator has no wordlist for it -- the NPC then keeps its
// template's name.
string generate_citizen_name(int gender)
{
  string cpath, style, word;
  object cit;

  // the naming style is a trait of the nationality, not of the town: every
  // settlement under the same country draws its citizens' names from one pool,
  // and an area with no citizenship of its own borrows the pool of the region
  // it sits in without taking its nationality
  cpath = (string)this_object()->query_naming_citizenship_path();
  if (!strlen(cpath))
    return nil;

  cit = load_object(cpath);
  if (!cit)
    return nil;

  style = cit->query_name_style();
  if (!style || !strlen(style))
    return nil;

  word = (gender == GENDER_FEMALE) ? "female" : "male";
  // order 3 with a length window keeps results name-like without copying the
  // source list; see packages/namegen for the quality/size trade-off
  return NAMEGEN_OB->generate_for(style, word, 3, 4, 9);
}

// Record one census NPC for a role, at its work location. Data-only -- it
// materializes when the work location loads (npc_restore). This is the seam
// the F4 generator plugs into: a sentient slot is given a generated name here
// (the rest of its body still comes from the transitional template); a
// non-sentient slot is a plain template clone. Returns the uuid.
private string assign_npc_to_role(string name, mapping role)
{
  string id, game, source, work;

  source = role["source"];
  work = role["work"];
  if (!source || !strlen(source) || !work || !strlen(work))
    return nil;

  game = game_from_path((string)this_object()->query_area_path());
  id = UUID_OB->uuid();

  // Data-only roster entry. The individual (gender, generated name, level,
  // equipment) is materialized and persisted on its own npc.o at first load
  // (npc_restore); nothing per-individual is stored in the census.
  this_object()->add_census_entry(id, ([ "source":   source,
                          "location": work,
                          "savefile": npc_save_dir(game, id) + NPC_SAVE_FILE,
                          "role":     name ]));

  return id;
}

// Staff a role up to its count: add census slots until the live count reaches
// it. A slot emptied by a death (role NPCs do not auto-respawn) is refilled
// here, not instantly at the point of death.
void fill_role(string name)
{
  mapping role;
  object loc;
  int have, want, i;

  role = query_roles()[name];
  if (!role)
    return;

  want = role["count"];
  have = count_role_npcs(name);
  for (i = have; i < want; i++)
  {
    mapping spread;
    string spot;

    // each holder gets its own spot in the work area, in turn
    spot = work_spot_for(name, i);
    spread = ([ ]) + role;
    if (strlen(spot))
      spread["work"] = spot;

    assign_npc_to_role(name, spread);
  }

  // if the work location is already resident, materialize the new slots now
  // (mirrors the vacancy refill); otherwise they come in when it next loads
  loc = (object)this_object()->query_loaded_location(role["work"]);
  if (loc)
    this_object()->restore_location_npcs(loc);
}

// Staff every role in the area -- the settlement pass. Idempotent: a role
// already at its count adds nothing.
void fill_area_roles()
{
  string * names;
  int i;

  names = map_indices(query_roles());
  for (i = 0; i < sizeof(names); i++)
    fill_role(names[i]);
}

// Drop a role: cull its census NPCs (destruct any live, delete their savefiles)
// and forget the role, so removing it leaves no orphan behind.
void remove_role(string name)
{
  string * ids;
  object owner;
  int i;

  owner = (object)this_object()->query_root_area();
  if (owner != this_object())
  {
    owner->remove_role(name);
    return;
  }

  if (!roles[name])
    return;

  ids = map_indices((mapping)this_object()->query_npc_census());
  for (i = 0; i < sizeof(ids); i++)
  {
    mapping e;
    object npc;

    e = ((mapping)this_object()->query_npc_census())[ids[i]];
    if (e["role"] == name && !e["poi"] && !e["guard"])
    {
      npc = (object)this_object()->query_live_npc_at(e["location"], ids[i]);
      if (npc)
        npc->dest_me();
      if (e["savefile"] && file_size(e["savefile"]) >= 0)
        remove_file(e["savefile"]);
      this_object()->drop_census_entry(ids[i]);
    }
  }

  map_delete(roles, name);
  this_object()->save_me();
}
