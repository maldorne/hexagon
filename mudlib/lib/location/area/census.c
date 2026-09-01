
// The census: the area's individual NPCs.
//
// A census row is a person the world refers to one by one -- a citizen staffing
// a role, the unique filling a POI vacancy, a posted watch. Each carries a uuid
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
#include <areas/vacancy.h>
#include <living/persisted.h>
#include <living/races.h>
#include <basic/gender.h>

// The area's individuals:
//   ([ uuid : ([ "source": template_id, "current_location": location_file,
//                "savefile": npc.o path, "vacancy"/"works_at": ... ]) ])
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

// What the books remember about a person, copied off them while they are in the
// world: enough to name and rank everybody in a report without loading anyone.
// The person is the authority; this is only ever read by reports, never by the
// world. Where they live is not here -- the houses know that already.
void update_npc_info(string uuid, object npc)
{
  mapping entry;
  mixed given;

  entry = query_npc_census()[uuid];
  if (!entry || !npc)
    return;

  given = npc->query_given_name();
  if (stringp(given) && strlen(given))
    entry["name"] = given;

  entry["gender"] = npc->query_gender();
  entry["level"] = npc->query_level();

  this_object()->save_me();
}

// Record one individual in the census and persist. The seam for the pieces that
// staff a post of their own -- a role slot, a vacancy -- and need the
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
    if (query_npc_census()[ids[i]][CENSUS_LOCATION] == location_file)
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
  mapping entry, role, t, timetable;
  int first, sentient, gender;

  entry = query_npc_census()[id];
  game = game_from_path((string)this_object()->query_area_path());

  source = entry["source"];

  // Every NPC is a generic NPC; what it can do comes from the components its
  // template asks for -- a guard's exit check among them.
  npc = clone_object(GENERIC_NPC);
  if (!npc)
    return nil;

  npc->set_npc_uuid(id);
  npc->set_npc_game(game);
  npc->set_npc_area_path((string)this_object()->query_area_path());
  npc->set_npc_source(source);
  if (entry["poi"])
    npc->set_npc_poi(entry["poi"]);

  // Somebody who holds a job is a named, self-gendered citizen; a monster or a
  // The vacancy is read here for the place it names.
  role = entry[CENSUS_VACANCY]
           ? (mapping)this_object()->query_vacancy(entry[CENSUS_VACANCY])
           : nil;
  t = (mapping)this_object()->query_area_template(game, source);

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

    // The census is what decides what an NPC is and where it belongs, so it
    // stamps those fields on top of whatever restore_object handed back: the
    // NPC's own savefile is a record of them, never the authority.
    npc->set_npc_area_path((string)this_object()->query_area_path());
    npc->set_npc_source(source);
    if (entry["poi"])
      npc->set_npc_poi(entry["poi"]);
  }
  else
    npc->set_gender((int)this_object()->decide_gender(game, source));
  gender = npc->query_gender();

  // A sentient citizen's proper name is generated once, using its gender, and
  // stored on the NPC itself (npc_given_name -> npc.o) because id.c's `name` is
  // static and never saved. Do it BEFORE the template: monster::set_name takes
  // only the first name, so ours wins and the template's generic one is a no-op.
  // On a restore the name was already re-seeded by restore_npc above.
  //
  // Keyed on the NPC lacking a name rather than on `first`, the way nationality
  // below is: somebody who was already alive when their type became sentient --
  // a fixed individual turned into a generic role -- was never named, and would
  // otherwise stay nameless for as long as they live.
  if (sentient && !npc->query_given_name())
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
  if (first && entry[CENSUS_VACANCY])
  {
    mapping job;
    string trade;

    job = (mapping)this_object()->query_vacancy(entry[CENSUS_VACANCY]);
    if (job && stringp(job[VACANCY_CLASS]) && strlen(job[VACANCY_CLASS]))
      npc->set_class_ob(job[VACANCY_CLASS]);

    // A job is a social object like a race or a citizenship, and the games
    // that model one keep it under obj/jobs. Somebody taken on for a job the
    // game has a file for is enrolled in it; a job with no file is just a name
    // the settlement uses, and nothing is stamped.
    trade = "/games/" + game + "/obj/jobs/" + entry[CENSUS_VACANCY] + ".c";
    if (file_size(trade) >= 0)
      npc->set_job_ob(trade);
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
  //
  // Keyed on the NPC having no people of its own as well as on being newborn,
  // the way the proper name above is: somebody whose type never handed one over
  // -- a template captured before races were stored where apply_template reads
  // them -- would otherwise keep the placeholder race for as long as they live.
  // A restore does not re-apply the template's social objects (re-applying the
  // class would reset the class level), so this is the only chance they get.
  if (sentient &&
      (first || !npc->query_race_ob() ||
       npc->query_race_ob() == DEFAULT_RACE_OB))
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
  else if (member_array(npc->query_home(),
                        (string *)this_object()->query_houses()) < 0)
  {
    // an address has to name a house. Unbuilding one clears the address of
    // whoever was in the world at the time and can do nothing for the rest, so
    // somebody who was away comes back holding the address of a house that is
    // no longer there.
    npc->set_home(nil);
    if (!first)
      npc->save_npc();
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
  if (!npc->query_work() && entry[CENSUS_WORKS_AT])
  {
    npc->set_work(entry[CENSUS_WORKS_AT]);
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

  // now that it is somewhere, its components can take up whatever needs the
  // world to see them -- a guard registering on the exit it watches
  npc->components_placed();

  // A sentient NPC with a workplace keeps the hours its type declares: out to
  // work at one hour, home at another, keyed on the game hour. Work is the
  // individual's own (npc.o); the timetable is the type's (template), int-keyed
  // here because JSON stored its hours as strings. Attached fresh each
  // materialization (so a schedule change is picked up); home is read live from
  // The areas handler drives it hour by hour and staggers the departures.
  //
  // A type that names no hours keeps none: it stays where it is put. Walking
  // somebody to a house every evening is a thing about them, not a thing every
  // person does, so it is said or it does not happen.
  work = npc->query_work();
  timetable = (t && mappingp(t["timetable"]))
                ? _int_keyed_hours(t["timetable"]) : nil;

  if (sentient && work && strlen(work) &&
      mappingp(timetable) && map_sizeof(timetable))
  {
    object sched;

    npc->add_component("schedule",
                       ([ "work": work, "timetable": timetable ]));

    // record its scheduled hours in the census index so the areas handler can
    // wake and dispatch it at those hours even while it is unloaded
    sched = npc->query_component_by_type("schedule");
    if (sched)
      this_object()->index_schedule_hours(id, sched->query_active_hours());

    // A trip is never saved, so somebody who was walking when this location
    // last unloaded is standing halfway with no route left. The timetable only
    // speaks at the hours it names, so without this it would wait there until
    // the next one; ask where this hour puts it and let it walk the rest.
    npc->resume_schedule((int)this_object()->query_game_hour());
  }
  else
  {
    // Somebody whose type used to name hours and no longer does: the component
    // rode in on its npc.o and would keep walking them about on its own.
    npc->remove_component("schedule");
    this_object()->index_schedule_hours(id, ({ }));
  }

  // A job that comes with a house houses whoever holds it, set on every
  // materialization so it survives death and replacement. Read live from the
  // vacancy, so rebinding the house reaches the holder without a respawn.
  if (entry[CENSUS_VACANCY])
  {
    mapping job;

    job = (mapping)this_object()->query_vacancy(entry[CENSUS_VACANCY]);
    if (job && job[VACANCY_HOME])
      npc->set_home(job[VACANCY_HOME]);
  }

  // Refresh what the books remember about this person. The npc.o is the
  // authority on who somebody is; these are a copy the census keeps so a
  // report can name and rank everybody, including those nobody has loaded.
  // Written on every materialization, which is the only moment the two are
  // side by side, so a level that changed in play does not leave a stale
  // number behind.
  update_npc_info(id, npc);

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

  // Parked until we decide how a settlement repopulates. This took somebody on
  // for every job held here that nobody holds, every time the place loaded,
  // which is a hiring policy nobody chose. The machinery stays -- see
  // staff_vacancies in vacancies.c -- but nothing hires behind our back.
  //
  // this_object()->staff_vacancies_at(file);

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
  if (query_npc_census()[uuid][CENSUS_LOCATION] != loc->query_file_name())
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
  if (uuid && query_npc_census()[uuid] && query_npc_census()[uuid][CENSUS_LOCATION] != file)
  {
    query_npc_census()[uuid][CENSUS_LOCATION] = file;
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
      if (uuid && query_npc_census()[uuid] && query_npc_census()[uuid][CENSUS_LOCATION] != file)
      {
        query_npc_census()[uuid][CENSUS_LOCATION] = file;
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
// something hanging off a census entry (a POI vacancy) and need
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
  mapping entry;

  entry = query_npc_census()[uuid];

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

  // Parked with the other half of this, in restore_location_npcs: a fixed post
  // used to be taken up again VACANCY_RESPAWN_DELAY after its holder fell. How
  // a settlement replaces its dead is a decision we have not made, so the post
  // simply stands empty until something asks for it to be staffed.
  //
  // if (entry && entry[CENSUS_VACANCY])
  // {
  //   mapping job;
  //
  //   job = (mapping)this_object()->query_vacancy(entry[CENSUS_VACANCY]);
  //   if (job && job[VACANCY_FIXED])
  //     call_out("staff_vacancies_at", VACANCY_RESPAWN_DELAY,
  //              job[VACANCY_WORKS_AT]);
  // }

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

