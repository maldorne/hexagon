
// Points of interest and their vacancies.
//
// A POI marks a location the world cares about -- a venture, a town entrance or
// square, a crossroads, a shrine -- and is the anchor for vacancies: named
// single-instance posts (a barman, a healer, a unique) that are placed at that
// location by hand rather than scattered by the statistical population sweep.
// Declaring a vacancy is also what takes an NPC source out of that sweep.
//
// This file owns the `pois` mapping and everything that reads or edits it. The
// concrete NPCs filling the slots are not owned here: they are ordinary census
// individuals, so anything touching the census is asked of the area.

#include <room/location.h>
#include <areas/poi.h>
#include <living/persisted.h>

//   ([ location_file : ([ kind, label, guard_dir, vacancies ]) ])
// See include/areas/poi.h for the field keys and the kind vocabulary.
mapping pois;

int set_vacancy_home(string role, string home);
void ensure_vacancies_assigned(string location_file);

void create()
{
  pois = ([ ]);
}

mapping query_pois() { return pois; }

// The POI attached to a location, or nil. A location holds at most one.
mapping query_poi(string location_file)
{
  return pois[location_file];
}

int is_poi(string location_file)
{
  return !undefinedp(pois[location_file]);
}

// Attach (or replace) a POI on a location. `kind` must be one of POI_KINDS;
// `label` is optional display text. Re-attaching preserves the existing
// vacancies so a reconversion does not drop them.
void add_poi(string location_file, string kind, varargs string label)
{
  mapping entry;

  if (!location_file || !strlen(location_file))
    return;
  if (member_array(kind, POI_KINDS) < 0)
    return;

  entry = pois[location_file];
  if (!entry)
    entry = ([ POI_FIELD_VACANCIES: ({ }) ]);

  entry[POI_FIELD_KIND] = kind;
  if (label && strlen(label))
    entry[POI_FIELD_LABEL] = label;
  if (!entry[POI_FIELD_VACANCIES])
    entry[POI_FIELD_VACANCIES] = ({ });

  pois[location_file] = entry;
  this_object()->save_me();
}

void remove_poi(string location_file)
{
  mapping census;
  string * ids;
  int i;

  // a POI's guards belong to it: drop them (destructing any live) before the
  // POI itself goes, so removing it never leaves an orphan guard behind
  census = (mapping)this_object()->query_npc_census();
  ids = map_indices(census);
  for (i = 0; i < sizeof(ids); i++)
    if (census[ids[i]]["guard"] &&
        census[ids[i]]["poi"] == location_file)
      this_object()->remove_guard(ids[i]);

  map_delete(pois, location_file);
  this_object()->save_me();
}

void set_poi_label(string location_file, string label)
{
  if (!pois[location_file])
    return;
  pois[location_file][POI_FIELD_LABEL] = label;
  this_object()->save_me();
}

// The exit direction a town_entrance POI's guards watch (the way into the
// town). Only meaningful on a town_entrance; ignored by square guards.
void set_poi_guard_dir(string location_file, string dir)
{
  if (!pois[location_file])
    return;
  pois[location_file][POI_FIELD_GUARD_DIR] = dir;
  this_object()->save_me();
}
string query_poi_guard_dir(string location_file)
{
  return pois[location_file] ? pois[location_file][POI_FIELD_GUARD_DIR] : nil;
}

// The vacancies of a POI, or an empty array when there is no POI there.
mapping * query_vacancies(string location_file)
{
  mapping entry;
  entry = pois[location_file];
  return entry ? entry[POI_FIELD_VACANCIES] : ({ });
}

// Declare a vacancy on a location's POI: a named `role` filled from a
// unique NPC `source`. Starts unfilled. No-op if the location
// has no POI or the role already exists.
void add_vacancy(string location_file, string role, string source)
{
  mapping entry;
  mapping * vs;
  int i;

  entry = pois[location_file];
  if (!entry || !role || !strlen(role) || !source || !strlen(source))
    return;

  vs = entry[POI_FIELD_VACANCIES];
  if (!vs) vs = ({ });
  for (i = 0; i < sizeof(vs); i++)
    if (vs[i][VACANCY_FIELD_ROLE] == role)
      return;

  // A vacancy is added by hand from an NPC blueprint path; build its template
  // now (the one time the source .c is loaded) and store the vacancy by its
  // template id, so filling it later never touches the .c.
  {
    string game;
    game = game_from_path((string)this_object()->query_area_path());
    if (!BESTIARY_HANDLER->has_template(game, source))
      BESTIARY_HANDLER->add_template(source);
    source = (string)this_object()->query_template_from_source(source);
  }

  vs += ({ ([ VACANCY_FIELD_ROLE:   role,
              VACANCY_FIELD_SOURCE: source,
              VACANCY_FIELD_UUID:   nil ]) });
  entry[POI_FIELD_VACANCIES] = vs;
  pois[location_file] = entry;

  // a vacancy source leaves the statistical roster
  this_object()->recompute_intended();
  this_object()->save_me();
}

void remove_vacancy(string location_file, string role)
{
  mapping entry;
  mapping * vs, * out;
  int i;

  entry = pois[location_file];
  if (!entry) return;

  vs = entry[POI_FIELD_VACANCIES];
  if (!vs) return;

  // drop the census NPC that fills this vacancy (destruct any live copy and
  // delete its savefile) so removing the vacancy leaves no orphan that would
  // re-materialize on the next load -- mirrors remove_poi culling its guards
  {
    mapping census;
    string * ids;
    int j;

    census = (mapping)this_object()->query_npc_census();
    ids = map_indices(census);
    for (j = 0; j < sizeof(ids); j++)
    {
      mapping e;
      object npc;

      e = census[ids[j]];
      if (e["role"] == role && e["poi"] == location_file)
      {
        npc = (object)this_object()->live_census_npc(location_file, ids[j]);
        if (npc)
          npc->dest_me();
        if (e["savefile"] && file_size(e["savefile"]) >= 0)
          remove_file(e["savefile"]);
        this_object()->drop_census_entry(ids[j]);
      }
    }
  }

  out = ({ });
  for (i = 0; i < sizeof(vs); i++)
    if (vs[i][VACANCY_FIELD_ROLE] != role)
      out += ({ vs[i] });

  entry[POI_FIELD_VACANCIES] = out;
  pois[location_file] = entry;
  this_object()->recompute_intended();
  this_object()->save_me();
}

// Record which concrete NPC (uuid) currently fills a vacancy role, or clear
// it (uuid nil) when the NPC dies so the fill pass respawns it.
void set_vacancy_uuid(string location_file, string role, string uuid)
{
  mapping entry;
  mapping * vs;
  int i;

  entry = pois[location_file];
  if (!entry) return;

  vs = entry[POI_FIELD_VACANCIES];
  for (i = 0; vs && i < sizeof(vs); i++)
    if (vs[i][VACANCY_FIELD_ROLE] == role)
    {
      vs[i][VACANCY_FIELD_UUID] = uuid;
      this_object()->save_me();
      return;
    }
}

// The set of NPC sources claimed by a vacancy anywhere in the area,
// as ([ source : 1 ]). Used by the roster recompute to keep vacancy uniques
// out of the statistical population.
mapping query_vacancy_sources()
{
  mapping ret;
  string * locs;
  int i, j;

  ret = ([ ]);
  locs = map_indices(pois);
  for (i = 0; i < sizeof(locs); i++)
  {
    mapping * vs;
    vs = pois[locs[i]][POI_FIELD_VACANCIES];
    for (j = 0; vs && j < sizeof(vs); j++)
      ret[vs[j][VACANCY_FIELD_SOURCE]] = 1;
  }

  return ret;
}

// Bind a POI vacancy (identified by role) to a fixed home (a house location), so
// whoever fills that vacancy always lives there. Searches every POI's vacancies
// for the role, sets the home on each match, and updates the live NPC if it is
// materialized. Returns the number of vacancies bound (0 if the role is unknown).
int set_vacancy_home(string role, string home)
{
  string * locs;
  mixed * vs;
  int i, vi, found;

  locs = map_indices(pois);
  found = 0;

  for (i = 0; i < sizeof(locs); i++)
  {
    vs = pois[locs[i]][POI_FIELD_VACANCIES];
    for (vi = 0; vs && vi < sizeof(vs); vi++)
      if (vs[vi][VACANCY_FIELD_ROLE] == role)
      {
        vs[vi][VACANCY_FIELD_HOME] = home;
        found++;

        // update the live NPC filling this slot, if any
        if (vs[vi][VACANCY_FIELD_UUID])
        {
          object npc;
          npc = (object)this_object()->live_census_npc(
                  locs[i], vs[vi][VACANCY_FIELD_UUID]);
          if (npc)
          {
            npc->set_home(home);
            npc->save_npc();
          }
        }
      }
  }

  if (found)
    this_object()->save_me();
  return found;
}

// Bind a house to a POI vacancy in one step (the builder-ring flow): given a
// location and a role, turn a bare plot there into a house, make the vacancy's
// NPC its resident, and set the vacancy's fixed home to it. Returns the number
// of vacancies bound (0 if the role is unknown). If the location is already a
// house it is reused; the vacancy NPC is added as a resident.
int bind_vacancy_house(string role, string location)
{
  object loc, home_comp;
  string * locs;
  mixed * vs;
  int i, vi;
  string uuid;

  loc = load_object(LOCATION_HANDLER)->load_location(location);
  if (!loc)
    return 0;

  // find the uuid currently filling this vacancy (to record as resident)
  uuid = nil;
  locs = map_indices(pois);
  for (i = 0; i < sizeof(locs); i++)
  {
    vs = pois[locs[i]][POI_FIELD_VACANCIES];
    for (vi = 0; vs && vi < sizeof(vs); vi++)
      if (vs[vi][VACANCY_FIELD_ROLE] == role)
        uuid = vs[vi][VACANCY_FIELD_UUID];
  }

  if (loc->query_component_by_type(LOCATION_COMPONENT_PLOT))
  {
    // a bare plot becomes this vacancy's house
    loc->remove_component(LOCATION_COMPONENT_PLOT);
    loc->add_component(LOCATION_COMPONENT_HOME,
                       ([ "residents": uuid ? ({ uuid }) : ({ }) ]));
    loc->save_me();
    this_object()->door_house_exits(loc);
    this_object()->remove_plot(location);
    this_object()->log_event("Raised a house at " + location + " for the " +
                             role + " vacancy.");
  }
  else if ((home_comp = loc->query_component_by_type(LOCATION_COMPONENT_HOME))
           && uuid)
  {
    // already a house: ensure the vacancy NPC is a resident
    home_comp->add_resident(uuid);
    loc->save_me();
  }

  return set_vacancy_home(role, location);
}

// Ensure every vacancy of the POI at `location_file` has a census NPC
// assigned to that location. A vacancy is (re)assigned when it is empty or
// when its recorded NPC is no longer in the census (it died). Data-only:
// materialization happens in the normal restore loop.
void ensure_vacancies_assigned(string location_file)
{
  mapping entry, census;
  mapping * vs;
  int i;
  int changed;

  entry = pois[location_file];
  if (!entry)
    return;

  census = (mapping)this_object()->query_npc_census();
  vs = entry[POI_FIELD_VACANCIES];
  changed = 0;
  for (i = 0; vs && i < sizeof(vs); i++)
  {
    string uuid;

    uuid = vs[i][VACANCY_FIELD_UUID];
    if (uuid && census[uuid])
      continue;   // already filled and alive

    vs[i][VACANCY_FIELD_UUID] =
      (string)this_object()->assign_vacancy_npc(
        vs[i][VACANCY_FIELD_SOURCE], location_file, vs[i][VACANCY_FIELD_ROLE]);
    changed = 1;
  }

  if (changed)
    this_object()->save_me();
}

// If `uuid` filled a vacancy, clear that slot and return ({ file, role });
// otherwise nil. Used on death so the vacancy can be refilled.
mixed * clear_vacancy_by_uuid(string uuid)
{
  string * locs;
  int i, j;

  locs = map_indices(pois);
  for (i = 0; i < sizeof(locs); i++)
  {
    mapping * vs;
    vs = pois[locs[i]][POI_FIELD_VACANCIES];
    for (j = 0; vs && j < sizeof(vs); j++)
      if (vs[j][VACANCY_FIELD_UUID] == uuid)
      {
        vs[j][VACANCY_FIELD_UUID] = nil;
        this_object()->save_me();
        return ({ locs[i], vs[j][VACANCY_FIELD_ROLE] });
      }
  }

  return nil;
}

// call_out target: respawn a vacancy NPC some time after its holder died.
// Re-assigns the slot and, if the POI's location is loaded, materializes the
// NPC at once; otherwise it comes back the next time the location loads.
void _refill_vacancy(string file)
{
  object loc;

  if (!pois[file])
    return;

  ensure_vacancies_assigned(file);

  loc = (object)this_object()->loaded_location(file);
  if (loc)
    this_object()->restore_location_npcs(loc);
}

// Ensure every POI vacancy in the area is assigned and, where the location
// is loaded, materialized. Safe to call repeatedly (idempotent per slot).
void fill_vacancies()
{
  string * locs;
  int i;

  locs = map_indices(pois);
  for (i = 0; i < sizeof(locs); i++)
  {
    object loc;

    ensure_vacancies_assigned(locs[i]);
    loc = (object)this_object()->loaded_location(locs[i]);
    if (loc)
      this_object()->restore_location_npcs(loc);
  }
}
