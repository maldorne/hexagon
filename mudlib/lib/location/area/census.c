
// The census: the area's individual NPCs.
//
// A census row is a person the world refers to one by one -- a citizen staffing
// a job, the unique filling a POI vacancy, a posted watch. Each carries a uuid
// and a savefile of its own, which is exactly what tells it apart from the
// anonymous monsters counted elsewhere. The row itself stays lean (who, where,
// what post); everything individual about the NPC -- gender, level, inventory,
// its generated name, its home and workplace -- lives on its own npc.o, written
// whole when the person is created (create_npc).
//
// This file owns `npc_census` and the whole create / materialize / drain / die
// cycle around it. Creation is the crossroads of the area: it reads the type
// template, the jobs the settlement offers and the citizenship, so most of
// what it does is asking other pieces for their part.

#include <room/location.h>
#include <areas/area.h>
#include <areas/poi.h>
#include <areas/vacancy.h>
#include <living/persisted.h>
#include <living/races.h>
#include <living/family.h>
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

// A generic NPC stamped with a census row's identity, not yet read from disk or
// placed anywhere.
private object _clone_npc(string id)
{
  mapping entry;
  object npc;

  entry = query_npc_census()[id];
  npc = clone_object(GENERIC_NPC);
  if (!npc)
    return nil;

  npc->set_uuid(id);
  npc->set_npc_game(game_from_path((string)this_object()->query_area_path()));
  npc->set_npc_area_path((string)this_object()->query_area_path());
  npc->set_npc_source(entry["source"]);
  if (entry["poi"])
    npc->set_npc_poi(entry["poi"]);

  return npc;
}

// Write the npc.o of a census row's person, complete. Who somebody is comes from
// its owners: the type's template says how the people of this trade look, talk
// and behave; the citizenship draws the race and the name; the area sets the
// level and the stats; the post gives the class, the workplace and the kit. All
// of it is decided here, once. The body is a passing clone, never placed in the
// world: it exists to be saved and is gone when this returns. Returns 1 if the
// npc.o was written.
private int _build_npc(string id)
{
  mapping entry, job, t;
  mixed * equipment;
  mixed cpath;
  object npc;
  string game, source;
  int ok;

  entry = query_npc_census()[id];
  if (!entry)
    return 0;

  npc = _clone_npc(id);
  if (!npc)
    return 0;

  game = (string)npc->query_npc_game();
  source = entry["source"];
  job = entry[CENSUS_VACANCY]
          ? (mapping)this_object()->query_vacancy(entry[CENSUS_VACANCY])
          : nil;
  t = (mapping)this_object()->query_area_template(game, source);

  // Gender comes from the type: a template that fixes one hands it over, a
  // bimodal one rolls between the genders it can actually describe. Sentience
  // does not enter into it -- an NPC with a proper name still cannot be a gender
  // its template has no words for.
  npc->set_gender((int)this_object()->decide_gender(game, source));

  // A sentient citizen's proper name is generated using its gender and stored
  // on the NPC itself (npc_given_name -> npc.o) because id.c's `name` is static
  // and never saved. Before the template: monster::set_name takes only the
  // first name, so ours wins and the template's generic one becomes an alias.
  if (t && t["sentient"])
  {
    mixed gname;

    // No (string) cast here: that is a conversion kfun, not a type assertion,
    // and it errors on nil -- which is what a citizenship with no name style
    // hands back.
    gname = this_object()->generate_citizen_name((int)npc->query_gender());
    if (stringp(gname) && strlen(gname))
      npc->set_given_name(gname);
  }

  npc->apply_template(t, 1);

  // The trade's class, when its job declares one. Set before the level:
  // set_class_ob resets class_level to 1, so a class applied afterwards would
  // undo the level this NPC was just given.
  if (entry[CENSUS_VACANCY])
  {
    string trade;

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

  npc->set_level((int)this_object()->decide_level(game, source));

  // Nationality: the citizenship at the top of the area's parent chain. A place
  // with none, a road between two towns, makes nobody its subject.
  cpath = this_object()->query_root_citizenship_path();
  if (stringp(cpath) && strlen(cpath))
    npc->set_city_ob(cpath);

  // What people this one is: the citizenship declares which races it is made
  // of and each citizen born here draws from that pool, so a trade authored
  // for one town staffs another without a second source and a mixed
  // citizenship comes out mixed. Fauna and the unnamed filler keep the race
  // their type gives them.
  //
  // The pool is the naming citizenship's, not the nationality's: a road
  // between two towns makes nobody its subject, but its travellers are still
  // people of the region they walk through.
  //
  // set_race_ob unwinds the previous race's bonuses, languages and aliases
  // before applying the new one, so it is safe on top of what the template set.
  if (t && t["sentient"])
  {
    cpath = this_object()->query_naming_citizenship_path();
    if (stringp(cpath) && strlen(cpath))
    {
      mixed race;

      race = load_object(cpath)->query_random_race();
      if (stringp(race) && strlen(race))
        npc->set_race_ob(race);
    }
  }

  // this individual's concrete workplace, taken from the job it holds
  if (entry[CENSUS_WORKS_AT])
    npc->set_work(entry[CENSUS_WORKS_AT]);

  // The kit is rolled once and carried for life. It is the job's where there is
  // one -- the same trade is armed differently from town to town -- and the
  // type's for somebody who holds no post. Worn and wielded when the person is
  // placed in the world, not here.
  equipment = (job && pointerp(job[VACANCY_EQUIPMENT]))
                ? job[VACANCY_EQUIPMENT] : (t ? t["equipment"] : nil);
  if (pointerp(equipment) && sizeof(equipment))
  {
    string * kit;
    int i;

    kit = (string *)this_object()->resolve_equipment(equipment);
    for (i = 0; i < sizeof(kit); i++)
      npc->add_clone(kit[i], 1);
  }

  ok = npc->save_npc();

  // the books keep a copy of who somebody is from the moment they exist
  update_npc_info(id, npc);

  npc->dest_me();
  return ok;
}

// Take a person onto the census, whole: the row and their npc.o. `row` carries
// what places them (the post, where they work, where they stand); everything
// individual is decided now and written to the npc.o. Nothing is loaded or
// placed: the person appears when the location they stand in loads. Returns the
// uuid, or nil if their npc.o could not be written.
string create_npc(string source, mapping row)
{
  string id, game;

  if (!source || !strlen(source))
    return nil;

  game = game_from_path((string)this_object()->query_area_path());
  id = UUID_OB->uuid();

  row = ([ ]) + (row ? row : ([ ]));
  row["source"] = source;
  row["savefile"] = npc_save_dir(game, id) + NPC_SAVE_FILE;
  query_npc_census()[id] = row;

  if (!_build_npc(id))
  {
    map_delete(query_npc_census(), id);
    return nil;
  }

  this_object()->save_me();
  return id;
}

// The person behind a census row, read from their npc.o: the one in the world
// when they are there, otherwise a body that is not placed anywhere, so nothing
// is loaded to reach them. Their template's presentation -- names, aliases,
// descriptions, skills, components -- is not saved, so it is applied again on
// top. Hand an unplaced one back to unload_npc. Nil for an unknown id or a
// missing npc.o.
object load_npc(string id)
{
  mapping entry;
  object npc;

  npc = find_living(id);
  if (npc)
    return npc;

  entry = query_npc_census()[id];
  if (!entry)
    return nil;

  npc = _clone_npc(id);
  if (!npc)
    return nil;

  if (!npc->restore_npc())
  {
    npc->dest_me();
    return nil;
  }

  // The census is what decides what an NPC is, so it stamps those fields on top
  // of whatever restore_object handed back: the NPC's own savefile is a record
  // of them, never the authority.
  npc->set_npc_source(entry["source"]);
  if (entry["poi"])
    npc->set_npc_poi(entry["poi"]);

  npc->apply_template((mapping)this_object()->query_area_template(
                        (string)npc->query_npc_game(), entry["source"]));

  // aliases are not saved either: the surname and the race it answers to, put
  // back after the template, since applying one replaces the alias list
  npc->start_family();
  npc->start_race();

  // the purse, rebuilt from the saved money_array, as a player's is
  npc->start_money();

  return npc;
}

// Save a loaded person; one that is not placed anywhere is then gone.
void unload_npc(object npc)
{
  if (!npc)
    return;

  npc->save_npc();
  if (!environment(npc))
    npc->dest_me();
}

// Put a census person into `loc`, the location the census says they are in:
// read them from disk, dress them, move them in and set them going. Nobody is
// placed twice, and a stale position (the census no longer puts them here) is
// ignored. Returns the person, or nil.
object place_npc(string id, object loc)
{
  object npc;
  string work;
  mapping entry, job, t, timetable;

  entry = query_npc_census()[id];
  if (!entry || !loc || entry[CENSUS_LOCATION] != loc->query_file_name())
    return nil;
  if (find_living(id))
    return nil;

  npc = load_npc(id);
  if (!npc)
    return nil;

  // the area placing somebody is the one that rosters them
  npc->set_npc_area_path((string)this_object()->query_area_path());

  job = entry[CENSUS_VACANCY]
           ? (mapping)this_object()->query_vacancy(entry[CENSUS_VACANCY])
           : nil;
  t = (mapping)this_object()->query_area_template(
                 (string)npc->query_npc_game(), entry["source"]);

  // Its address, read back from the house. A housed NPC stores its home on its
  // own .o, but the house also lists it as a resident, and the house is the end
  // that survives. Reading the link back here heals the pair instead of leaving
  // the NPC homeless in a house that expects it.
  if (!npc->query_home())
  {
    mixed house;

    house = this_object()->query_house_of(id);
    if (stringp(house) && strlen(house))
      npc->set_home(house);
  }
  else if (member_array(npc->query_home(),
                        (string *)this_object()->query_houses()) < 0)
    // an address has to name a house. Unbuilding one clears the address of
    // whoever was in the world at the time and can do nothing for the rest, so
    // somebody who was away comes back holding the address of a house that is
    // no longer there.
    npc->set_home(nil);
  else
    // it already knows its address; make sure the house agrees. A vacancy
    // re-homes its replacement by writing only the NPC's side, so without this
    // the house would still name the holder before last.
    this_object()->claim_house(id, npc->query_home());

  // the inventory came back with the npc.o; wear and wield it
  npc->init_equip();

  npc->move(loc);

  // now that it is somewhere, its components can take up whatever needs the
  // world to see them -- a guard registering on the exit it watches
  npc->components_placed();

  // A sentient NPC with a workplace keeps the hours of the job it holds: out to
  // work at one hour, home at another, keyed on the game hour. Work is the
  // individual's own (npc.o); the hours are the job's, falling back to the
  // type's for somebody who holds no post. Int-keyed here because JSON stored
  // them as strings. Attached fresh each time (so a change is picked up); home
  // is read live. The areas handler drives it hour by hour and staggers the
  // departures.
  //
  // Naming no hours keeps none: the NPC stays where it is put. Walking somebody
  // to a house every evening is a thing about them, not a thing every person
  // does, so it is said or it does not happen.
  work = npc->query_work();
  timetable = (job && mappingp(job[VACANCY_TIMETABLE]))
                ? _int_keyed_hours(job[VACANCY_TIMETABLE])
                : ((t && mappingp(t["timetable"]))
                     ? _int_keyed_hours(t["timetable"]) : nil);

  if (t && t["sentient"] && work && strlen(work) &&
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

  // A job that comes with a house houses whoever holds it, set every time so it
  // survives death and replacement. Read live from the vacancy, so rebinding the
  // house reaches the holder without a respawn.
  if (job && job[VACANCY_HOME])
  {
    npc->set_home(job[VACANCY_HOME]);
    // the job writes the holder's side; the house learns who lives in it here,
    // which is what a report of the barracks reads
    this_object()->claim_house(id, job[VACANCY_HOME]);
  }

  npc->save_npc();

  // Refresh what the books remember about this person. The npc.o is the
  // authority on who somebody is; these are a copy the census keeps so a
  // report can name and rank everybody, including those nobody has loaded.
  // A level that changed in play does not leave a stale number behind.
  update_npc_info(id, npc);

  return npc;
}

// Called when a location loads (prewarm / movement): bring back exactly the
// NPCs the census says are here -- the ones that were in the location when it
// last unloaded -- and its monsters. No new NPCs are created here. Idempotent
// (anybody already in the world, here or elsewhere, is left alone).
void populate_location(object loc)
{
  string file;
  string * ids, * fids;
  mapping foreign;
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

  // the census position is where somebody was when their location last
  // unloaded, and they may have walked on since: place_npc skips anybody
  // already in the world
  ids = query_census_uuids_at(file);
  for (i = 0; i < sizeof(ids); i++)
    place_npc(ids[i], loc);

  // the anonymous half of the population: cloned fresh from this location's
  // bucket, with no state carried over from the last time it was loaded
  this_object()->restore_location_monsters(loc, file);

  // Roamers rostered in another area but resting here: the areas handler indexes
  // them by location, so each is placed by its own roster area (which owns its
  // census and template). The handler index is only a hint: place_npc checks
  // the census still puts them here.
  foreign = AREA_HANDLER->foreign_positions_at(file);
  fids = map_indices(foreign);
  for (i = 0; i < sizeof(fids); i++)
  {
    object rarea;
    rarea = (foreign[fids[i]] == (string)this_object()->query_area_path())
              ? this_object() : AREA_HANDLER->query_area(foreign[fids[i]]);
    if (rarea)
      rarea->place_npc(fids[i], loc);
  }
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
// so populate_location brings the NPC back on the next load.
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

    uuid = inv[i]->query_uuid();
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
    this_object()->index_schedule_hours(uuid, ({ }));
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

  // and out of the hourly index, or the round would keep waking the dead
  this_object()->index_schedule_hours(uuid, ({ }));

  // stop its house expecting it back; a vacancy's house is rebound to whoever
  // fills the post next, a roster citizen's frees a bed for its replacement
  this_object()->release_house(uuid);

  // and tell its house. The savefile goes with the death, so the family record
  // is the only thing that will still be able to name this person afterwards:
  // it moves them into its history, widows whoever they were married to, and if
  // they were the last one left the house dies out and frees its property.
  handler("families", this_object())->member_died(FAMILY_NPC + uuid);

  // drop any cross-area position index for it, so a dead roamer is never
  // rematerialized when the foreign location it last rested in reloads
  AREA_HANDLER->set_foreign_position(uuid, nil, nil);

  // Parked with the other half of this, in populate_location: a fixed post
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


