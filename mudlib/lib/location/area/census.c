
// The census: the area's individual NPCs.
//
// A census row is a person the world refers to one by one -- a citizen staffing
// a role, the unique filling a POI vacancy, a posted guard. Each carries a uuid
// and a savefile of its own, which is exactly what tells it apart from the
// anonymous monsters counted elsewhere. The row itself stays lean (who, where,
// what post); everything individual about the NPC -- gender, level, inventory,
// its generated name, its home and workplace -- lives on its own npc.o.
//
// This file owns `npc_census` and the whole materialize / drain / die cycle
// around it. Materialization is the crossroads of the area: it reads the type
// template, the role board, the POI vacancies and the citizenship, so most of
// what it does is asking other pieces for their part.

#include <room/location.h>
#include <areas/area.h>
#include <areas/poi.h>
#include <living/persisted.h>
#include <basic/gender.h>

// The area's individuals:
//   ([ uuid : ([ "source": template_id, "location": location_file,
//                "savefile": npc.o path, "poi"/"role"/"guard": ... ]) ])
mapping npc_census;

void create()
{
  npc_census = ([ ]);
}

// The census this area works from. An area that delegates its population reads
// and writes its community's census, not its own -- its own stays empty. The
// mapping is handed back by reference, so a caller that adds or edits a row is
// editing the community's, and save_me() writes it where it lives.
mapping query_npc_census()
{
  object owner;

  owner = (object)this_object()->query_root_area();
  return owner == this_object() ? npc_census
                                : (mapping)owner->query_npc_census();
}

// Record one individual in the census and persist. The seam for the pieces that
// staff a post of their own -- a role slot, a vacancy, a guard -- and need the
// person to exist before anything materializes it.
void add_census_entry(string uuid, mapping row)
{
  if (!uuid || !row)
    return;
  query_npc_census()[uuid] = row;
  this_object()->save_me();
}

// How many NPCs of `source` the census holds across the whole area, materialized
// or not. The counterpart of query_monster_live_count for the named half of the
// population; query_total_live_count adds the two.
int query_npc_live_count(string source)
{
  string * ids;
  int i, n;

  ids = map_indices(query_npc_census());
  for (i = 0; i < sizeof(ids); i++)
    if (query_npc_census()[ids[i]]["source"] == source)
      n++;

  return n;
}

// Census entries assigned to a given location file.
private string * query_census_uuids_at(string location_file)
{
  string * ids, * ret;
  int i;

  ids = map_indices(query_npc_census());
  ret = ({ });
  for (i = 0; i < sizeof(ids); i++)
    if (query_npc_census()[ids[i]]["location"] == location_file)
      ret += ({ ids[i] });

  return ret;
}

// Is `uuid` already materialized inside `loc`? Non-NPC contents answer nil to
// query_npc_uuid (DGD call_other to an undefined function returns nil).
private int has_live_uuid(object loc, string uuid)
{
  object * inv;
  int i;

  inv = all_inventory(loc);
  for (i = 0; i < sizeof(inv); i++)
    if (inv[i] && inv[i]->query_npc_uuid() == uuid)
      return 1;

  return 0;
}


// Assign a vacancy NPC: an individual with a uuid and a savefile, bound to
// a POI (not part of the statistical roster, so no npc_caps check), and
// the census entry is tagged with the owning POI location and role. Returns
// the uuid. The NPC materializes when the POI's location loads.
string assign_vacancy_npc(string source, string location_file,
                                  string role)
{
  string id, game;

  // source is a template id (add_vacancy built its template); no monster .c
  game = game_from_path((string)this_object()->query_area_path());

  id = UUID_OB->uuid();
  query_npc_census()[id] = ([ "source": source, "location": location_file,
                      "savefile": npc_save_dir(game, id) + NPC_SAVE_FILE,
                      "poi": location_file, "role": role ]);
  this_object()->save_me();

  return id;
}

// Materialize a census NPC into `loc`: a generic NPC with the source's data
// template applied. If it already has a savefile (it was live before), restore
// its state on top; otherwise this is its first materialization (freshly
// assigned by the population sweep) and we save it so its state persists.
// Rebuild a template timetable read from JSON (string hour keys "6"/"20") into
// the int-keyed mapping the schedule component and the hour index expect. Entry
// values (goto/msg) keep their string keys untouched.
private mapping _int_keyed_hours(mapping m)
{
  mapping out;
  string * keys;
  int i;

  out = ([ ]);
  if (!mappingp(m))
    return out;
  keys = map_indices(m);
  for (i = 0; i < sizeof(keys); i++)
    out[atoi(keys[i])] = m[keys[i]];

  return out;
}

private object npc_restore(string id, object loc)
{
  object npc;
  string game, source, savefile, work;
  mapping entry, role, t;
  int first, sentient, gender;

  entry = query_npc_census()[id];
  game = game_from_path((string)this_object()->query_area_path());

  source = entry["source"];

  // Every NPC is a generic NPC; a guard census entry additionally gets the
  // "guard" component below (at placement), which carries the exit check.
  npc = clone_object(GENERIC_NPC);
  if (!npc)
    return nil;

  npc->set_npc_uuid(id);
  npc->set_npc_game(game);
  npc->set_npc_area_path((string)this_object()->query_area_path());
  npc->set_npc_source(source);
  if (entry["poi"])
    npc->set_npc_poi(entry["poi"]);

  // A role slot is a named, self-gendered citizen; anything else (a monster, a
  // POI vacancy, a guard) takes its gender from the template. A slot is a
  // role-board slot only when tagged "role" with no "poi"/"guard"; the role is
  // read here for the workplace it names.
  role = (entry["role"] && !entry["poi"] && !entry["guard"]) ? ((mapping)this_object()->query_roles())[entry["role"]] : nil;
  t = BESTIARY_HANDLER->query_template(game, source);

  // sentience is a fact about the type, so it comes from the template
  sentient = t && t["sentient"];

  savefile = entry["savefile"];
  first = !(savefile && file_size(savefile) >= 0);

  // Gender, level and inventory live on the NPC's own npc.o (save_object
  // persists them), not the census. On restore they come back with the object;
  // on the first materialization they are decided once here and the save at the
  // end persists them. Gender comes from the type either way: a template that
  // fixes one hands it over, a bimodal one rolls between the genders it can
  // actually describe. Sentience does not enter into it -- an NPC with a proper
  // name still cannot be a gender its template has no words for.
  if (!first)
  {
    npc->restore_npc();

    // restore_object hands back the identity fields exactly as they were
    // written, so the census -- which is what decides what an NPC is and where
    // it belongs -- stamps them again on top. Without this a renamed source (a
    // template id replacing the original .c path) loses to the stale saved
    // value on every load, and the entry never settles.
    npc->set_npc_area_path((string)this_object()->query_area_path());
    npc->set_npc_source(source);
    if (entry["poi"])
      npc->set_npc_poi(entry["poi"]);
  }
  else
    npc->set_gender((int)this_object()->decide_gender(game, source));
  gender = npc->query_gender();

  // A sentient citizen's proper name is generated once, on the first
  // materialization, using its gender, and stored on the NPC itself
  // (npc_given_name -> npc.o) because id.c's `name` is static and never saved.
  // Do it BEFORE the template: monster::set_name takes only the first name, so
  // ours wins and the template's generic one is a no-op. On a restore the name
  // was already re-seeded by restore_npc above.
  if (first && sentient)
  {
    mixed gname;

    // No (string) cast here: that is a conversion kfun, not a type assertion,
    // and it errors on nil -- which is what a citizenship with no name style
    // hands back. The error would abort the whole materialization.
    gname = this_object()->generate_citizen_name(gender);
    if (stringp(gname) && strlen(gname))
      npc->set_given_name(gname);
  }

  npc->apply_template(t, first);

  // The trade's class, when its role declares one. Set before the level:
  // set_class_ob resets class_level to 1, so a class applied afterwards would
  // undo the level this NPC was just given.
  if (first && entry["role"])
  {
    mapping role;

    role = (mapping)this_object()->query_role(entry["role"]);
    if (role && stringp(role["class"]) && strlen(role["class"]))
      npc->set_class_ob(role["class"]);
  }

  // level: decided once from the area on the first materialization; on restore
  // it came back with the object
  if (first)
    npc->set_level((int)this_object()->decide_level(game, source));

  // Nationality. Persisted with the NPC, so it is stamped once -- but keyed on
  // the NPC lacking one rather than on `first`, so an NPC that predates its
  // area having a citizenship picks one up the next time it wakes.
  if (!npc->query_city_ob())
  {
    mixed cpath;

    cpath = this_object()->query_root_citizenship_path();
    if (stringp(cpath) && strlen(cpath))
      npc->set_city_ob(cpath);
  }

  // People, like nationality, come from where the citizen is born rather than
  // from its type: the citizenship declares which races it is made of and each
  // one born here draws from that pool. A trade authored as human then staffs
  // an elf town without a second source, and a mixed citizenship comes out
  // mixed. Done after the template so it overrides the type's race, and only
  // for a generated citizen -- fauna and the unnamed filler keep their own.
  //
  // set_race_ob unwinds the previous race's bonuses, languages and aliases
  // before applying the new one, so it is safe on top of what the template set.
  if (first && sentient)
  {
    mixed cpath;

    cpath = npc->query_city_ob();
    if (stringp(cpath) && strlen(cpath))
    {
      mixed race;

      race = load_object(cpath)->query_random_race();
      if (stringp(race) && strlen(race))
        npc->set_race_ob(race);
    }
  }

  // Finish a named individual. Its short stays the template's kind word --
  // npc::query_cap_name shows that in room lists and combat so it does not read
  // like a player; the proper name is the find_living id and shows on examine.
  // Re-assert its own gender over a single-gender template's, and keep the
  // template's kind word as an alias so the NPC still answers to its trade as
  // well as to its proper name.
  if (npc->query_given_name())
  {
    mixed kind;

    npc->set_gender(gender);

    // the template's trade word, per gender (a fixed template stores a string,
    // a bimodal one a per-gender map keyed by the gender id as a string)
    kind = t ? t["name"] : nil;
    if (mappingp(kind))
      kind = kind["" + gender];
    if (stringp(kind) && strlen(kind))
      npc->add_alias(kind);
  }

  // Its address, read back from the house. A housed NPC stores its home on its
  // own .o, but the house also lists it as a resident, and the house is the end
  // that survives: an NPC rebuilt from the census comes back with no address
  // while its house still names it. Reading the link back here heals the pair
  // instead of leaving the NPC homeless in a house that expects it.
  if (!npc->query_home())
  {
    mixed house;

    house = this_object()->query_house_of(id);
    if (stringp(house) && strlen(house))
    {
      npc->set_home(house);
      if (!first)
        npc->save_npc();
    }
  }
  else
    // it already knows its address; make sure the house agrees. A vacancy
    // re-homes its replacement by writing only the NPC's side, so without this
    // the house would still name the holder before last.
    this_object()->claim_house(id, npc->query_home());

  // Per-individual assignment on the npc.o: this NPC's concrete workplace, taken
  // from the role it fills. A restored NPC already carries one. A
  // first-materialize NPC is persisted by the equipment save below; one that had
  // none saves here. (The roster area is npc_area_path, stamped above.)
  if (!npc->query_work() && role && role["work"])
  {
    npc->set_work(role["work"]);
    if (!first)
      npc->save_npc();
  }

  // Equipment on the npc.o: on the first materialization the NPC rolls its kit
  // once from the template, equips it and the save below persists it; on restore
  // the inventory came back with restore_npc, so just re-wear/wield it.
  if (first)
  {
    mixed * equipment;

    equipment = t ? t["equipment"] : nil;
    if (pointerp(equipment) && sizeof(equipment))
      this_object()->equip_npc(npc, (string *)this_object()->resolve_equipment(equipment));
    npc->save_npc();
  }
  else
    npc->init_equip();

  npc->move(loc);

  // A sentient NPC with a workplace gets a daily timetable keyed on the game
  // hour: out to work in the morning, home in the evening. Work is the
  // individual's own (npc.o); the timetable is the type's (template), int-keyed
  // here because JSON stored its hours as strings, or a sensible default when
  // the type declares none. Attached fresh
  // each materialization (so a schedule change is picked up); home is read live
  // from the NPC. Guards are excluded above (a role slot is never a guard entry).
  // The areas handler drives it hour by hour and staggers the departures.
  work = npc->query_work();
  if (sentient && work && strlen(work))
  {
    mapping timetable;
    object sched;

    timetable = (t && mappingp(t["timetable"]))
                  ? _int_keyed_hours(t["timetable"]) : nil;
    if (!mappingp(timetable) || !map_sizeof(timetable))
      timetable = ([ 6  : ([ "goto" : "work" ]),
                     20 : ([ "goto" : "home" ]) ]);

    npc->add_component("schedule",
                       ([ "work": work, "timetable": timetable ]));

    // record its scheduled hours in the census index so the areas handler can
    // wake and dispatch it at those hours even while it is unloaded
    sched = npc->query_component_by_type("schedule");
    if (sched)
      this_object()->index_schedule_hours(id, sched->query_active_hours());
  }

  // A POI vacancy (the pub's barman, the shop's keeper) with a fixed home lives
  // in its designated house; set it every materialization so it survives death
  // and respawn. Read live from the vacancy so a rebind is picked up.
  if (entry["poi"] && entry["role"])
  {
    mapping poi;
    mixed * vs;
    int vi;

    poi = ((mapping)this_object()->query_pois())[entry["poi"]];
    vs = poi ? poi[POI_FIELD_VACANCIES] : nil;
    for (vi = 0; vs && vi < sizeof(vs); vi++)
      if (vs[vi][VACANCY_FIELD_ROLE] == entry["role"] &&
          vs[vi][VACANCY_FIELD_HOME])
      {
        npc->set_home(vs[vi][VACANCY_FIELD_HOME]);
        break;
      }
  }

  // A guard carries the area's citizenship as its city_ob (so diplomacy can
  // resolve its loyalty) and, at an entrance, watches the entry direction: it
  // registers on that exit so the exit handler consults its guardian_check.
  // Square guards just stand there (no direction). Both are derived here from
  // the live POI / area rather than the census, so a change to guard_dir or the
  // citizenship is picked up the next time the guard materializes; none of it
  // persists on the NPC.
  if (entry["guard"])
  {
    string cpath, gdir;
    mapping poi;

    // a guard carries the same nationality as any other NPC of the area; what
    // differs per town is which guard template it is built from, not who it
    // answers to
    cpath = (string)this_object()->query_root_citizenship_path();
    if (strlen(cpath))
      npc->set_city_ob(cpath);

    // only an entrance guard watches a direction; a square guard is presence
    // only, so it never registers on an exit even if a stale guard_dir lingers
    poi = ((mapping)this_object()->query_pois())[entry["poi"]];
    gdir = (poi && poi[POI_FIELD_KIND] == POI_KIND_TOWN_ENTRANCE)
             ? poi[POI_FIELD_GUARD_DIR] : nil;

    // every guard carries the guard component: it makes the NPC recognisable as
    // a guard (so, for example, the home system posts it to a barracks rather
    // than a family house) and gives it the citizenship challenge. An entrance
    // guard additionally watches its direction and registers on that exit; a
    // square guard keeps the component with no direction (it challenges nobody).
    npc->add_component("guard", gdir ? ([ "direction" : gdir ]) : ([ ]));
    if (gdir)
      loc->register_guard(npc, gdir);
  }

  return npc;
}

// Called when a location of this area loads (prewarm / movement): bring back
// exactly the NPCs the census says live here -- the ones that were in the
// location when it last unloaded. Restore-only: no new NPCs are created here.
// Idempotent (already-present uuids are left alone).
void restore_location_npcs(object loc)
{
  string file;
  string * ids;
  int i;

  if (!loc)
    return;

  file = loc->query_file_name();
  if (!file || !strlen(file))
    return;

  // A POI location fills its vacancies first: assign a census NPC to any
  // empty or dead vacancy slot, so the materialize loop below brings it in
  // alongside the location's regular census NPCs. A guarded POI (town entrance
  // or square) likewise tops up its citizenship's guards.
  this_object()->ensure_vacancies_assigned(file);
  this_object()->ensure_guards_assigned(file);

  ids = query_census_uuids_at(file);
  for (i = 0; i < sizeof(ids); i++)
    if (!has_live_uuid(loc, ids[i]))
      npc_restore(ids[i], loc);

  // the anonymous half of the population: cloned fresh from this location's
  // bucket, with no state carried over from the last time it was loaded
  this_object()->restore_location_monsters(loc, file);

  // Roamers rostered in another area but resting here: the areas handler indexes
  // them by location, so bring each back through its own roster area (which owns
  // its census and template). This area's own census-of-location never lists them.
  {
    mapping foreign;
    string * fids;

    foreign = AREA_HANDLER->foreign_positions_at(file);
    fids = map_indices(foreign);
    for (i = 0; i < sizeof(fids); i++)
    {
      object rarea;
      rarea = (foreign[fids[i]] == (string)this_object()->query_area_path())
                ? this_object() : AREA_HANDLER->query_area(foreign[fids[i]]);
      if (rarea)
        rarea->restore_one_npc(fids[i], loc);
    }
  }
}

// Materialize one of this area's census NPCs into `loc`, called by another area's
// restore when a roamer rostered here is resting in that area's location (this
// area's own restore never runs for a foreign location). Guards against a
// duplicate (already live anywhere) and against a stale index entry: the census
// position is authoritative, the handler index only a hint, so the census must
// still place this NPC at exactly this location.
void restore_one_npc(string uuid, object loc)
{
  if (!uuid || !loc || !query_npc_census()[uuid])
    return;
  if (query_npc_census()[uuid]["location"] != loc->query_file_name())
    return;
  if (AREA_HANDLER->find_live_npc(uuid))
    return;
  npc_restore(uuid, loc);
}

// Update this area's census position for one of its roamers to `file`. Called by
// the foreign area whose location the roamer walked into, when that location
// unloads, so the roster keeps an accurate position for scheduling and reload.
void set_census_location(string uuid, string file)
{
  if (uuid && query_npc_census()[uuid] && query_npc_census()[uuid]["location"] != file)
  {
    query_npc_census()[uuid]["location"] = file;
    this_object()->save_me();
  }
}

// Everything of `source` the area holds, both halves of the population: the
// named NPCs in the census and the anonymous monsters counted in the buckets.
// This is the number the population sweep measures against that source's cap.
int query_total_live_count(string source)
{
  return query_npc_live_count(source) + (int)this_object()->query_monster_live_count(source);
}

// Called from a location's dest_me before its contents are torn down: persist
// each of our NPCs so its state survives the unload. The census entry stays,
// so restore_location_npcs brings the NPC back on the next load.
void drain_location(object loc)
{
  object * inv;
  int i, changed;
  string file;

  if (!loc)
    return;

  file = loc->query_file_name();
  inv = all_inventory(loc);
  for (i = 0; i < sizeof(inv); i++)
  {
    string uuid, roster;

    if (!inv[i] || !inv[i]->query_persisted())
      continue;

    uuid = inv[i]->query_npc_uuid();
    roster = inv[i]->query_npc_area_path();

    if (roster == (string)this_object()->query_area_path())
    {
      // one of our own: record where it actually is now (where it walked to on
      // its schedule) so it comes back here rather than at its census spot, and
      // drop any stale foreign-index entry -- it is home, in its roster area
      if (uuid && query_npc_census()[uuid] && query_npc_census()[uuid]["location"] != file)
      {
        query_npc_census()[uuid]["location"] = file;
        changed = 1;
      }
      if (uuid)
        AREA_HANDLER->set_foreign_position(uuid, roster, nil);
    }
    else if (roster && strlen(roster))
    {
      // a roamer rostered elsewhere, resting in our area: tell its roster area
      // where it is (for scheduling / reload) and index it here so this location
      // rematerializes it on load
      object rarea;
      rarea = AREA_HANDLER->query_area(roster);
      if (rarea)
        rarea->set_census_location(uuid, file);
      AREA_HANDLER->set_foreign_position(uuid, roster, file);
    }

    inv[i]->save_npc();
  }

  if (changed)
    this_object()->save_me();
}

// Drop one row from the census and persist. A seam for the pieces that own
// something hanging off a census entry (a POI vacancy, a guard post) and need
// to retire the individual filling it without going through a death.
void drop_census_entry(string uuid)
{
  if (uuid && query_npc_census()[uuid])
  {
    map_delete(query_npc_census(), uuid);
    this_object()->save_me();
  }
}

// Called by a persisted NPC (via monster::do_death) when it dies: drop its
// census slot so the population frees up and a replacement may spawn later.
// The savefile itself is removed by the NPC's own delete_npc_save().
//
// If the dead NPC filled a vacancy, free the slot and schedule a delayed
// respawn at its POI.
void npc_died(string uuid)
{
  mixed * vacancy;
  mapping entry;
  string guard_poi;

  // note a fallen guard's POI before we drop the census entry, so we can
  // re-post a replacement there after the cooldown
  entry = query_npc_census()[uuid];
  guard_poi = (entry && entry["guard"]) ? entry["poi"] : nil;

  if (entry)
  {
    map_delete(query_npc_census(), uuid);
    this_object()->save_me();
  }

  // stop its house expecting it back; a vacancy's house is rebound to whoever
  // fills the post next, a roster citizen's frees a bed for its replacement
  this_object()->release_house(uuid);

  // drop any cross-area position index for it, so a dead roamer is never
  // rematerialized when the foreign location it last rested in reloads
  AREA_HANDLER->set_foreign_position(uuid, nil, nil);

  vacancy = (mixed *)this_object()->clear_vacancy_by_uuid(uuid);
  if (vacancy)
    call_out("_refill_vacancy", VACANCY_RESPAWN_DELAY, vacancy[0]);

  if (guard_poi)
    call_out("_refill_guards", VACANCY_RESPAWN_DELAY, guard_poi);
}

// --- guards --------------------------------------------------------------
// Guards are census NPCs a citizenship fields at its town entrances and
// squares. Unlike a vacancy (a single named role), a guarded POI wants a
// count -- the citizenship's security level -- of identical guards, all from
// the citizenship's guard NPC source. They are data-only like every census
// NPC: they materialize when the POI's location loads (npc_restore clones the
// guard base, stamps the citizenship and, at an entrance, registers the exit).

// One guard assigned to a POI as census data. The guard's identity is its
// source; its watched direction and citizenship are not stored -- they are
// derived at materialization from the POI's guard_dir and the area's current
// citizenship, so a later change to either is picked up without rewriting the
// census. Returns the uuid.
string assign_guard_npc(string source, string poi_file)
{
  string id, game;

  // The guard source is a diplomacy-placed unique, not part of room conversion,
  // so its template is built here the first time one is fielded (the one place
  // a guard's monster .c is loaded, guarded by has_template so it happens once).
  // The census then stores the template id, and filling/refilling never reloads
  // the .c.
  game = game_from_path((string)this_object()->query_area_path());
  if (!BESTIARY_HANDLER->has_template(game, source))
    BESTIARY_HANDLER->add_template(source);
  source = (string)this_object()->query_template_from_source(source);

  id = UUID_OB->uuid();
  query_npc_census()[id] = ([ "source": source, "location": poi_file,
                      "savefile": npc_save_dir(game, id) + NPC_SAVE_FILE,
                      "poi": poi_file, "guard": 1 ]);
  this_object()->save_me();

  return id;
}

// The live object for a census uuid inside its (loaded) POI, or nil. Works for
// any census NPC (a guard or a vacancy unique), matched by its uuid.
object query_live_npc_at(string poi_file, string uuid)
{
  object loc;
  object * inv;
  int i;

  loc = (object)this_object()->query_loaded_location(poi_file);
  if (!loc)
    return nil;
  inv = all_inventory(loc);
  for (i = 0; i < sizeof(inv); i++)
    if (inv[i] && inv[i]->query_npc_uuid() == uuid)
      return inv[i];
  return nil;
}

