
// Guards, and the citizenship that fields them.
//
// Guards are census NPCs a citizenship posts at its town entrances and squares.
// Unlike a vacancy -- a single named post -- a guarded POI wants a *count* of
// identical guards: the citizenship's security level, all from the citizenship's
// guard source. Nothing about a guard's loyalty or watched direction is stored
// on the census entry; both are derived at materialization from the POI and the
// area's current citizenship, so changing either is picked up on the next fill
// without rewriting anything. Flipping the citizenship and re-posting is the
// invasion path.
//
// This file owns the `citizenship` name and the guard reconcile. The census rows
// the guards occupy belong to the area, so anything that writes them is asked of
// it.
//
// Inherited by /lib/location/area.c. Calls into the rest of the area go through
// this_object(): an area is a single object carrying the whole inheritance tree,
// so the call resolves at run time against the complete program. Only public
// functions are reachable that way, and results come back as mixed, hence the
// casts.

#include <areas/poi.h>
#include <areas/diplomacy.h>
#include <living/persisted.h>

// The citizenship this area belongs to (a name in the diplomacy graph).
// Guards fielded at the area's town entrances and squares follow this
// name, so flipping it is how an invaded town swaps its guards for the
// conqueror's.
string citizenship;

void repost_guards(string poi_file);
void ensure_guards_assigned(string location_file);
private string * guard_census_at(string poi_file, string source);

void create()
{
  citizenship = "";
}

// The area's citizenship (a diplomacy-graph name), or "" if none. Guards are
// fielded from it; see the guard fill below.
string query_citizenship() { return citizenship; }
void set_citizenship(string name)
{
  string * locs;
  int i;

  citizenship = name ? name : "";
  this_object()->save_me();

  // which guard source the roster excludes depends on the citizenship, so
  // recompute and persist it
  this_object()->recompute_intended();
  this_object()->save_me();

  // re-post guards at every guarded POI: drop the old citizenship's guards and
  // field the new one's. This is the invasion path -- flip the citizenship and
  // the guards become the conqueror's.
  locs = map_indices((mapping)this_object()->query_pois());
  for (i = 0; i < sizeof(locs); i++)
    repost_guards(locs[i]);
}

// The object path of the area's citizenship social object, or "" if unset.
// Guards carry this as their city_ob so diplomacy can resolve their loyalty.
string query_citizenship_path()
{
  string game;
  if (!strlen(citizenship))
    return "";
  game = game_from_path((string)this_object()->query_area_path());
  return "/games/" + game + "/obj/citizenships/" + citizenship;
}

// Guard census ids at a POI whose source matches `source`. A citizenship
// change swaps the source, so guards from the old one no longer match and are
// treated as stale by the reconcile below.
private string * guard_census_at(string poi_file, string source)
{
  mapping census;
  string * ids, * out, want;
  int i;

  want = (string)this_object()->_template_id(source);
  census = (mapping)this_object()->query_npc_census();
  ids = map_indices(census);
  out = ({ });
  for (i = 0; i < sizeof(ids); i++)
  {
    mapping e;
    e = census[ids[i]];
    if (e["guard"] && e["poi"] == poi_file &&
        (string)this_object()->_template_id(e["source"]) == want)
      out += ({ ids[i] });
  }
  return out;
}

// Drop a guard: remove its census entry first (so a death callback becomes a
// no-op), then destruct the live object if it is materialized, and delete its
// leftover savefile.
void remove_guard(string id)
{
  mapping e;
  object npc;

  e = ((mapping)this_object()->query_npc_census())[id];
  if (!e)
    return;

  this_object()->drop_census_entry(id);

  npc = (object)this_object()->live_census_npc(e["poi"], id);
  if (npc)
    npc->dest_me();

  if (e["savefile"] && file_size(e["savefile"]) >= 0)
    remove_file(e["savefile"]);
}

// Reconcile the guards of the POI at `location_file`: if it is a guarded kind,
// hold exactly the area citizenship's security level of guards, all from the
// citizenship's current guard source. Data-only; materialization happens in
// the restore loop. Guards from a stale source (after an invasion) or beyond
// the wanted count are removed, and the shortfall topped up.
void ensure_guards_assigned(string location_file)
{
  mapping entry, census;
  string kind, game, source;
  string * live, * ids;
  int want, have, i;
  object dh;

  entry = ((mapping)this_object()->query_pois())[location_file];
  if (!entry)
    return;
  kind = entry[POI_FIELD_KIND];
  if (kind != POI_KIND_TOWN_ENTRANCE && kind != POI_KIND_TOWN_SQUARE)
    return;

  // the area's citizenship supplies both the count (security) and the NPC
  // source (guard); with no citizenship, or none configured, field no guards
  game = game_from_path((string)this_object()->query_area_path());
  dh = load_object(DIPLOMACY_HANDLER);
  source = strlen(citizenship) ? dh->query_guard_path(game, citizenship) : "";
  want = strlen(source) ? dh->query_security_level(game, citizenship) : 0;

  // guards already here from the current source
  live = strlen(source) ? guard_census_at(location_file, source) : ({ });
  have = sizeof(live);

  // cull every guard at this POI that is not a current-source guard (stale
  // source after a citizenship change)
  census = (mapping)this_object()->query_npc_census();
  ids = map_indices(census);
  for (i = 0; i < sizeof(ids); i++)
  {
    mapping e;
    e = census[ids[i]];
    if (e["guard"] && e["poi"] == location_file &&
        member_array(ids[i], live) == -1)
      remove_guard(ids[i]);
  }

  // trim current-source guards down if security dropped
  for (i = want; i < have; i++)
    remove_guard(live[i]);

  // top up to the wanted count from the current source
  for (i = have; i < want; i++)
    this_object()->assign_guard_npc(source, location_file);
}

// call_out target: re-post a guard some time after one died at `file`.
void _refill_guards(string file)
{
  object loc;

  if (!((mapping)this_object()->query_pois())[file])
    return;

  ensure_guards_assigned(file);

  loc = (object)this_object()->loaded_location(file);
  if (loc)
    this_object()->restore_location_npcs(loc);
}

// Ensure every guarded POI in the area holds its citizenship's guards and,
// where the location is loaded, materialize them. Safe to call repeatedly.
void fill_guards()
{
  string * locs;
  int i;

  locs = map_indices((mapping)this_object()->query_pois());
  for (i = 0; i < sizeof(locs); i++)
  {
    object loc;

    ensure_guards_assigned(locs[i]);
    loc = (object)this_object()->loaded_location(locs[i]);
    if (loc)
      this_object()->restore_location_npcs(loc);
  }
}

// Rebuild the guards of a POI from the current citizenship / POI config: drop
// the ones there now (destructing any live) and re-derive. Callers use it after
// editing the citizenship or a guard_dir, so live guards reflect the config
// without a reload.
void repost_guards(string poi_file)
{
  mapping census;
  string * ids;
  int i;
  object loc;

  if (!((mapping)this_object()->query_pois())[poi_file])
    return;

  // remove every guard currently assigned here, whatever its source
  census = (mapping)this_object()->query_npc_census();
  ids = map_indices(census);
  for (i = 0; i < sizeof(ids); i++)
  {
    mapping e;
    e = census[ids[i]];
    if (e["guard"] && e["poi"] == poi_file)
      remove_guard(ids[i]);
  }

  ensure_guards_assigned(poi_file);

  loc = (object)this_object()->loaded_location(poi_file);
  if (loc)
    this_object()->restore_location_npcs(loc);
}
