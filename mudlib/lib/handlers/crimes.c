/*
 * Crimes handler.
 *
 * Applies the law of the place where an offence happens and keeps each
 * player's criminal record. An offence (theft, assault, ...) is reported by
 * whoever detects it -- the steal skill, a combat hook -- with report_crime();
 * the handler decides whether it is a crime at all, who knows about it, how
 * many points it is worth, and records it.
 *
 * Every game has its own handler: /games/<game>/handlers/crimes.c inherits this
 * one and points query_save_file at its own file. Reach it with
 * handler("crimes", ob), never by path, so the object you get is the one
 * belonging to ob's game. Every default number comes from <crimes.h>; a game's
 * handler overrides any of the query_* functions below to change it.
 *
 * The law itself (which offences cost how many points, whether the guards
 * kill, where the prisoners go) is stored per citizenship in the diplomacy
 * graph; the points a player has are kept here, per root citizenship (the
 * country at the top of the parent chain), so a crime in a town counts in its
 * whole country.
 *
 * Points decay in real time, lazily: they are brought up to date from a stored
 * timestamp whenever a record is read or written, never by a heart beat.
 *
 * A recorded crime where no guard stands sends the nearest free guards of the
 * place's community after the culprit (see dispatch_guards).
 *
 * Combat reports fights (report_attack) and deaths (report_death), and the
 * exit handler reports a guard slipped past (report_crime with trespass).
 *
 * Justice (see crimes.md): a guard of a citizenship that does not kill
 * (query_lethal) never lands the blow that would kill (spare_blow, asked by
 * the health code); the loser is passed out and arrested -- taken to one of the
 * citizenship's cells, at random, for a sentence, or fined on the spot where
 * there is none.
 * A sentence is kept here and served in a location with the jail component;
 * the fines office (fines component) charges what a record owes.
 *
 * Capture: a player who beats, in non-lethal combat, an NPC that is the target
 * of a capture objective of a quest (see capture) gets a free guard of the
 * community to take it to a cell; the NPC is held unconscious until it is
 * jailed, and the objective counts when it is.
 */

#include <mud/config.h>
#include <common/properties.h>
#include <areas/diplomacy.h>
#include <areas/area.h>
#include <areas/poi.h>
#include <areas/vacancy.h>
#include <room/location.h>
#include <sector/sector.h>
#include <crimes.h>
#include <living/quests.h>
#include <language.h>

inherit "/lib/core/object.c";

// player name -> ([ root citizenship : ([
//   "points" : current points (before decay: see find_entry),
//   "stamp"  : the time the points were last brought up to date,
//   "first"  : time of the first offence,
//   "last"   : time of the last offence,
//   "recent" : ({ ({ type, time, victim name, place short }) }), newest last
// ]) ])
mapping records;

// Sentences being served, by player name: ([ name : ([
//   "prison" : the location file the prisoner was taken to,
//   "until"  : the time the sentence ends,
//   "root"   : the root citizenship that sentenced them
// ]) ]). The sentences of NPCs are kept apart and never saved.
mapping jailed;
static mapping jailed_npcs;

// Arrests under way, not saved: the NPC being taken -> ([
//   "winner"      : name of the player who beat it,
//   "citizenship" : the citizenship whose cells receive it,
//   "guard"       : the guard taking it,
//   "until"       : the time the arrest is given up
// ])
static mapping captures;

// Who started each fight, not saved: victim -> ([ aggressor : time ]). Lets a
// repeated attack go unreported and a death be told from self-defence.
static mapping fights;

string query_save_file();
void release(object who);
void hold_captive(object creature);

void create()
{
  records = ([ ]);
  jailed = ([ ]);
  jailed_npcs = ([ ]);
  captures = ([ ]);
  fights = ([ ]);
  ::create();
  restore_object(query_save_file(), 1);
}

/**
 * Where the records are kept. A game's own handler overrides it to point at
 * /save/games/<game>/crimes.o.
 */
string query_save_file()
{
  return CRIMES_SAVE;
}

void setup()
{
  string name;
  int cnum;

  // Anticloning: only the master instance loaded through handler() may exist.
  if (sscanf(file_name(this_object()), "%s#%d", name, cnum) == 2)
    dest_me();
}

void save_handler()
{
  save_object(query_save_file(), 1);
}

void dest_me()
{
  save_handler();
  ::dest_me();
}

// --- Tunables: a game's handler overrides any of these ----------------------

/** Points from which a status applies. */
int query_threshold(string status)
{
  switch (status)
  {
    case CRIME_STATUS_SUSPECT:
      return CRIME_SUSPECT_POINTS;
    case CRIME_STATUS_WANTED:
      return CRIME_WANTED_POINTS;
    case CRIME_STATUS_OUTLAW:
      return CRIME_OUTLAW_POINTS;
  }

  return 0;
}

/** The law code used where a citizenship (and its parents) sets none. */
mapping query_default_law() { return CRIME_DEFAULT_LAW; }
int query_default_lethal() { return CRIME_DEFAULT_LETHAL; }

/** Points lost every decay period. */
int query_decay_points() { return CRIME_DECAY_POINTS; }

/** Length of the decay period, in seconds. */
int query_decay_period() { return CRIME_DECAY_PERIOD; }

/** Recent offences kept for each player and citizenship. */
int query_recent_max() { return CRIME_RECENT_MAX; }

/** Whether the offences of NPCs are recorded. */
int query_records_npcs() { return CRIME_RECORD_NPCS; }

/** Minimum light for a witness with no race to judge it to see. */
int query_witness_min_light() { return CRIME_WITNESS_MIN_LIGHT; }

/** Seconds in which the same attack is not reported twice. */
int query_fight_window() { return CRIME_FIGHT_WINDOW; }

/** Seconds a fight start is remembered. */
int query_fight_memory() { return CRIME_FIGHT_MEMORY; }

/** Percentage of the points that survive a death at the hands of a guard. */
int query_death_keep_percent() { return CRIME_DEATH_KEEP_PERCENT; }

/** Seconds before a pursuing guard gives up. */
int query_pursuit_give_up() { return CRIME_PURSUIT_GIVE_UP; }

/** Guards that answer a crime. */
int query_responders() { return CRIME_RESPONDERS; }

/** Fine, in base coins, for each point. */
int query_fine_per_point() { return CRIME_FINE_PER_POINT; }

/** Seconds of jail for each point. */
int query_jail_seconds_per_point() { return CRIME_JAIL_SECONDS_PER_POINT; }

/** Seconds a living beaten by a non-lethal guard stays passed out. */
int query_passed_out_seconds() { return CRIME_PASSED_OUT_SECONDS; }

/** Longest sentence, in seconds. */
int query_jail_max_seconds() { return CRIME_JAIL_MAX_SECONDS; }

/** Percentage of the points that remain once a sentence is served. */
int query_release_keep_percent() { return CRIME_RELEASE_KEEP_PERCENT; }

/** Points a sentence is counted from for an NPC with no record. */
int query_npc_arrest_points() { return CRIME_NPC_ARREST_POINTS; }

/** Closest a guard must stand to the place a target fell to be sent for it. */
int query_arrest_guard_range() { return CRIME_ARREST_GUARD_RANGE; }

/** Seconds an arrest may take before it is given up. */
int query_arrest_seconds() { return CRIME_ARREST_SECONDS; }

/** Percentage the fine grows by for each level of whoever pays it. */
int query_fine_level_percent() { return CRIME_FINE_LEVEL_PERCENT; }

/** Base coins a bribe costs for each point owed. */
int query_bribe_cost_per_point() { return CRIME_BRIBE_COST_PER_POINT; }

/** Chance, in percent, that a bribe works. */
int query_bribe_chance() { return CRIME_BRIBE_CHANCE; }

/** Points a failed bribe adds. */
int query_bribe_fail_points() { return CRIME_BRIBE_FAIL_POINTS; }

// --- The law ----------------------------------------------------------------

/**
 * The diplomacy graph of this handler's game.
 */
private object diplomacy()
{
  return handler("diplomacy", this_object());
}

/**
 * The country at the top of a citizenship's parent chain.
 */
private string root_of(string citizenship)
{
  string root;

  root = diplomacy()->query_root_citizenship(citizenship);
  return strlen(root) ? root : citizenship;
}

/**
 * The citizenship whose law applies in a place.
 *
 * @param place the location where something happened
 * @return the effective citizenship of the place's area, or "" when the place
 *         has no law: no area, a stateless area or a lawless place
 */
string query_jurisdiction(object place)
{
  object area;

  if (!place || place->query_property(LAWLESS_PROP))
    return "";

  if (!place->query_location())
    return "";

  area = (object)place->query_area();
  if (!area)
    return "";

  return (string)area->query_effective_citizenship();
}

/**
 * What an offence is worth under a citizenship's law.
 *
 * @param citizenship a diplomacy-graph name
 * @param offence the offence type
 * @return the points; 0 means it is legal there
 */
int query_law_points(string citizenship, string offence)
{
  mapping defaults;
  int points;

  points = diplomacy()->query_law_points(citizenship, offence);
  if (points >= 0)
    return points;

  defaults = query_default_law();
  return intp(defaults[offence]) ? defaults[offence] : 0;
}

/**
 * The law code in force for a citizenship, inheritance and defaults applied.
 *
 * @param citizenship a diplomacy-graph name
 * @return ([ offence : points ]) with every offence type
 */
mapping query_law(string citizenship)
{
  mapping law;
  string * types;
  int i;

  law = ([ ]);
  types = CRIME_TYPES;
  for (i = 0; i < sizeof(types); i++)
    law[types[i]] = query_law_points(citizenship, types[i]);

  return law;
}

/** Whether the guards of a citizenship kill: its own setting, or the default. */
int query_lethal(string citizenship)
{
  int lethal;

  lethal = diplomacy()->query_lethal(citizenship);
  return (lethal < 0) ? query_default_lethal() : lethal;
}

/** The cells a citizenship takes its prisoners to, empty when none. */
string * query_prisons(string citizenship)
{
  return diplomacy()->query_prisons(citizenship);
}

// --- Records ----------------------------------------------------------------

/**
 * Brings an entry's points up to date with the time elapsed since its stamp.
 */
private void decay(mapping entry)
{
  int periods, period;

  period = query_decay_period();

  if (entry["points"] <= 0 || period < 1)
  {
    entry["points"] = 0;
    entry["stamp"] = time();
    return;
  }

  periods = (time() - entry["stamp"]) / period;
  if (periods < 1)
    return;

  entry["points"] -= periods * query_decay_points();

  if (entry["points"] <= 0)
  {
    entry["points"] = 0;
    entry["stamp"] = time();
  }
  else
    entry["stamp"] += periods * period;
}

/**
 * The record of a player under a root citizenship, decayed up to now; nil when
 * there is none and `create` is 0.
 */
private mapping find_entry(string name, string root, int create)
{
  mapping entry;

  if (!records[name])
  {
    if (!create)
      return nil;
    records[name] = ([ ]);
  }

  entry = records[name][root];

  if (!entry)
  {
    if (!create)
      return nil;
    entry = ([ "points" : 0, "stamp" : time(), "first" : 0, "last" : 0,
               "recent" : ({ }) ]);
    records[name][root] = entry;
  }

  decay(entry);
  return entry;
}

/**
 * The status a number of points amounts to.
 */
private string status_for(int points)
{
  string * statuses;
  int i;

  statuses = CRIME_STATUSES;
  for (i = sizeof(statuses) - 1; i > 0; i--)
    if (points >= query_threshold(statuses[i]))
      return statuses[i];

  return statuses[0];
}

/**
 * The points a player has in a citizenship, decay applied.
 *
 * @param name the player's name
 * @param citizenship a diplomacy-graph name (its root is used)
 * @return the points
 */
int query_points(string name, string citizenship)
{
  mapping entry;

  entry = find_entry(name, root_of(citizenship), 0);
  return entry ? entry["points"] : 0;
}

/**
 * The status of a player, by name, in a citizenship. Being an outlaw in an
 * allied citizenship counts as being an outlaw here.
 *
 * @param name the player's name
 * @param citizenship a diplomacy-graph name (its root is used)
 * @return one of the CRIME_STATUS_* values
 */
string query_status_of(string name, string citizenship)
{
  string root, status;
  string * others;
  int i;

  root = root_of(citizenship);
  status = status_for(query_points(name, root));

  if (status == CRIME_STATUS_OUTLAW || !records[name])
    return status;

  others = map_indices(records[name]);
  for (i = 0; i < sizeof(others); i++)
    if (others[i] != root &&
        status_for(query_points(name, others[i])) == CRIME_STATUS_OUTLAW &&
        diplomacy()->is_ally(root, others[i]))
      return CRIME_STATUS_OUTLAW;

  return status;
}

/** query_status_of() for a living. */
string query_status(object who, string citizenship)
{
  if (!who)
    return CRIME_STATUS_CLEAN;

  return query_status_of(who->query_name(), citizenship);
}

/**
 * Wipes a player's record in a citizenship.
 *
 * @param name the player's name
 * @param citizenship a diplomacy-graph name (its root is used)
 * @return 1 if there was a record
 */
int pardon(string name, string citizenship)
{
  string root;

  root = root_of(citizenship);
  if (!records[name] || !records[name][root])
    return 0;

  records[name][root] = nil;
  if (!map_sizeof(records[name]))
    records[name] = nil;

  save_handler();
  return 1;
}

/**
 * Keeps only a percentage of a player's points in a citizenship.
 *
 * @param name the player's name
 * @param citizenship a diplomacy-graph name (its root is used)
 * @param percent the percentage kept: 50 halves the points, 0 clears them
 * @return the points left
 */
int reduce(string name, string citizenship, int percent)
{
  mapping entry;

  entry = find_entry(name, root_of(citizenship), 0);
  if (!entry)
    return 0;

  if (percent < 0)
    percent = 0;
  if (percent > 100)
    percent = 100;

  entry["points"] = entry["points"] * percent / 100;
  save_handler();
  return entry["points"];
}

/**
 * A player's whole record.
 *
 * @param name the player's name
 * @return ([ root citizenship : ([ "points", "status", "first", "last",
 *         "recent" ]) ]), a copy, empty when the player has none
 */
mapping query_record(string name)
{
  mapping record, entry;
  string * cits;
  int i;

  record = ([ ]);
  if (!records[name])
    return record;

  cits = map_indices(records[name]);
  for (i = 0; i < sizeof(cits); i++)
  {
    entry = find_entry(name, cits[i], 0);
    record[cits[i]] = ([ "points" : entry["points"],
                         "status" : status_for(entry["points"]),
                         "first"  : entry["first"],
                         "last"   : entry["last"],
                         "recent" : ({ }) + entry["recent"] ]);
  }

  return record;
}

// --- Sentences ----------------------------------------------------------------

/**
 * The sentence a living is serving, nil when none. Players' are saved by name,
 * NPCs' are kept in memory.
 */
private mapping find_sentence(object who)
{
  if (!who)
    return nil;

  return who->query_player() ? jailed[who->query_name()] : jailed_npcs[who];
}

private void set_sentence(object who, mapping sentence)
{
  if (who->query_player())
  {
    jailed[who->query_name()] = sentence;
    save_handler();
  }
  else
    jailed_npcs[who] = sentence;
}

private void clear_sentence(object who)
{
  if (!who)
    return;

  if (who->query_player())
  {
    if (jailed[who->query_name()])
    {
      jailed[who->query_name()] = nil;
      save_handler();
    }
  }
  else if (jailed_npcs[who])
    jailed_npcs[who] = nil;
}

// --- Reporting a crime ------------------------------------------------------

/**
 * Whether a living, standing where the crime happened, could make out the
 * culprit.
 */
private int sees(object witness, object place)
{
  if (witness->query_dead() ||
      witness->query_property(SLEEP_PROP) ||
      witness->query_property(PASSED_OUT_PROP) ||
      witness->query_property(PARALYZE_PROP) ||
      witness->query_property(BLIND_PROP))
    return 0;

  // only players judge the darkness through their race
  if (witness->query_player())
    return witness->check_dark(place->query_light()) == 0;

  return place->query_light() >= query_witness_min_light();
}

/**
 * Whether somebody in the place with a citizenship saw who did it. A hidden
 * culprit is seen by nobody.
 */
private int is_witnessed(object criminal, object victim, object place)
{
  object * inv;
  mixed city;
  int i;

  if (criminal->query_hidden())
    return 0;

  inv = all_inventory(place);
  for (i = 0; i < sizeof(inv); i++)
  {
    if (!inv[i] || !living(inv[i]) || inv[i] == criminal || inv[i] == victim)
      continue;

    city = inv[i]->query_city_ob();
    if (!stringp(city) || !strlen(city))
      continue;

    if (sees(inv[i], place))
      return 1;
  }

  return 0;
}

/**
 * Adds an offence to a player's record under a root citizenship.
 */
private void record_offence(string name, string root, string type, int points,
                            object victim, object place)
{
  mapping entry;
  mixed short;
  int limit;

  entry = find_entry(name, root, 1);
  short = place->short();

  entry["points"] += points;
  entry["stamp"] = time();
  if (!entry["first"])
    entry["first"] = time();
  entry["last"] = time();

  entry["recent"] += ({ ({ type, time(), victim ? victim->query_name() : "",
                           stringp(short) ? short : "" }) });

  limit = query_recent_max();
  if (sizeof(entry["recent"]) > limit)
    entry["recent"] = entry["recent"][sizeof(entry["recent"]) - limit..];

  save_handler();
}

// --- Sending guards ---------------------------------------------------------

/**
 * The place's community: the root of its area's parent chain, whose books hold
 * the census.
 */
private object community_of(object place)
{
  object area, owner;

  area = (object)place->query_area();
  if (!area)
    return nil;

  owner = (object)area->query_root_area();
  return owner ? owner : area;
}

/**
 * Whether a guard is standing in the place.
 */
private int guard_in(object place)
{
  object * inv;
  int i;

  inv = all_inventory(place);
  for (i = 0; i < sizeof(inv); i++)
    if (inv[i] && living(inv[i]) && !inv[i]->query_dead() &&
        inv[i]->has_component("guard"))
      return 1;

  return 0;
}

/**
 * The attributes of the guard component in the template of a census source, nil
 * when the type is not a guard. `cache` keeps what was read for each source.
 */
private mapping guard_attrs(object community, string game, string source,
                            mapping cache)
{
  mapping t;
  mixed attrs;

  if (undefinedp(cache[source]))
  {
    t = (mapping)community->query_area_template(game, source);
    attrs = (t && mappingp(t["components"])) ? t["components"]["guard"] : nil;
    cache[source] = mappingp(attrs) ? attrs : 0;
  }

  return mappingp(cache[source]) ? (mapping)cache[source] : nil;
}

/**
 * The guards to send: every free guard of the community, loaded or not, the
 * nearest to the place first (pick_nearest_npcs of the community, which never
 * sends one posted on a door). With a range, only those closer than it.
 */
private object * pick_guards(object community, object place, int wanted,
                             varargs int range)
{
  mapping rows, cache;
  string * uuids, * guards;
  string game;
  int i;

  rows = (mapping)community->query_npc_census();
  game = game_from_path((string)community->query_area_path());
  cache = ([ ]);
  uuids = map_indices(rows);
  guards = ({ });

  for (i = 0; i < sizeof(uuids); i++)
    if (guard_attrs(community, game, rows[uuids[i]]["source"], cache))
      guards += ({ uuids[i] });

  return (object *)community->pick_nearest_npcs(place, guards, wanted, range);
}

/**
 * Sends the guards of the place's community after a culprit. Called out after
 * the report, so loading locations never happens inside the reporting skill.
 * Nothing happens when a guard is already in the place (it answers the event
 * itself) or the place belongs to no community.
 */
void dispatch_guards(object criminal, object place)
{
  object community, comp;
  object * guards;
  int i;

  if (!criminal || !place || query_responders() < 1)
    return;

  if (guard_in(place))
    return;

  community = community_of(place);
  if (!community)
    return;

  guards = pick_guards(community, place, query_responders());
  for (i = 0; i < sizeof(guards); i++)
  {
    guards[i]->add_component("responder", ([ ]));
    comp = (object)guards[i]->query_component_by_type("responder");
    if (comp)
      comp->respond(criminal, place, query_pursuit_give_up());
  }
}

/**
 * Reports an offence.
 *
 * It is a crime only where there is law (the place's area has a citizenship
 * and the place is not lawless) and the offence is not legal there. It is
 * recorded only if the culprit is known: the caller says so with `known` (the
 * victim saw who did it) or a living with a citizenship in the place saw the
 * culprit. A culprit nobody can name is not recorded, but the place hears
 * event_crime with no culprit.
 *
 * Fires event_crime(emitter, criminal, type, victim, points) in the place: this
 * handler is the emitter; criminal is nil and points 0 when nobody can name the
 * culprit. A known crime in a place with no guard also sends the community's
 * guards after the culprit (query_responders() of them), whether it is a player
 * or an NPC.
 *
 * @param criminal who did it
 * @param type one of the CRIME_* offence types
 * @param victim who suffered it, nil if nobody did
 * @param place the location where it happened
 * @param known 1 if the victim knows who did it
 * @return the points added to the culprit's record, 0 if nothing was recorded
 */
int report_crime(object criminal, string type, object victim, object place,
                 varargs int known)
{
  string citizenship, root, name;
  int points;

  if (!criminal || !place || member_array(type, CRIME_TYPES) == -1)
    return 0;

  citizenship = query_jurisdiction(place);
  if (!strlen(citizenship))
    return 0;

  points = query_law_points(citizenship, type);
  if (points <= 0)
    return 0;

  if (!known && !is_witnessed(criminal, victim, place))
  {
    event(place, "crime", nil, type, victim, 0);
    return 0;
  }

  event(place, "crime", criminal, type, victim, points);
  call_out("dispatch_guards", 0, criminal, place);

  if (!criminal->query_player() && !query_records_npcs())
    return 0;

  name = criminal->query_name();
  root = root_of(citizenship);
  record_offence(name, root, type, points, victim, place);

  tell_object(criminal, _LANG_CRIMES_SEEN_CRIMINAL);
  if (victim && victim != criminal && victim->query_player())
    tell_object(victim, replace_string(_LANG_CRIMES_SEEN_VICTIM, "$criminal$",
                                       criminal->query_cap_name()));

  return points;
}

// --- Offences found in combat -------------------------------------------------

/**
 * The name of a living's citizenship in the diplomacy graph, "" if it has none.
 */
private string citizenship_of(object who)
{
  mixed city, name;
  object citizenship;

  city = who->query_city_ob();
  if (!stringp(city) || !strlen(city))
    return "";

  citizenship = load_object(city);
  name = citizenship ? citizenship->query_name() : nil;
  return stringp(name) ? name : "";
}

/**
 * Whether a living enforces the law: a guard, or one sent after a criminal.
 */
private int is_enforcer(object who)
{
  return who->has_component("guard") || who->has_component("responder");
}

/**
 * Whether any of the living's attackers is an enforcer: a criminal being
 * pursued.
 */
private int is_pursued(object victim)
{
  mixed * attackers;
  int i;

  attackers = victim->query_attacker_list();
  for (i = 0; i < sizeof(attackers); i++)
    if (attackers[i] && is_enforcer(attackers[i]))
      return 1;

  return 0;
}

/**
 * Whether the law of a jurisdiction protects a living: any player, and any
 * NPC with a citizenship that is not an enemy of the jurisdiction. Not the
 * wanted and outlaws, nor a criminal the guards are already after.
 */
private int is_protected(object victim, string jurisdiction)
{
  string status, citizenship;

  if (victim->query_player())
  {
    status = query_status(victim, jurisdiction);
    if (status == CRIME_STATUS_WANTED || status == CRIME_STATUS_OUTLAW)
      return 0;
  }

  if (is_pursued(victim))
    return 0;

  if (victim->query_player())
    return 1;

  citizenship = citizenship_of(victim);
  return strlen(citizenship) &&
         !diplomacy()->is_enemy(jurisdiction, citizenship);
}

/**
 * Whether `aggressor` was recorded starting a fight against `victim` recently.
 */
private int started(object aggressor, object victim)
{
  mapping mine;

  mine = fights[victim];
  return mine && intp(mine[aggressor]) &&
         mine[aggressor] + query_fight_memory() > time();
}

/**
 * A living has started attacking another. Called by the combat code the first
 * time it happens between the two.
 *
 * It is assault (guard assault when the victim is a guard) if the law of the
 * place protects the victim. It is not if the victim was already attacking the
 * aggressor, if the aggressor is a guard or a responder, or if the victim is
 * wanted, an outlaw or already being pursued by the guards. The same pair is
 * reported only once per query_fight_window() seconds. Whoever the aggressor
 * is, the event fires and the guards answer; only players are recorded.
 *
 * @param aggressor who attacks
 * @param victim who is attacked
 */
void report_attack(object aggressor, object victim)
{
  object place;
  string jurisdiction, type;
  mapping mine;
  mixed * names;
  int i;

  if (!aggressor || !victim || aggressor == victim || !living(victim))
    return;

  // remember who started, dropping what is too old to matter
  mine = fights[victim];
  if (!mine)
    mine = ([ ]);
  else if (intp(mine[aggressor]) &&
           mine[aggressor] + query_fight_window() > time())
    return;

  names = map_indices(mine);
  for (i = 0; i < sizeof(names); i++)
    if (mine[names[i]] + query_fight_memory() <= time())
      mine[names[i]] = nil;

  mine[aggressor] = time();
  fights[victim] = mine;

  // the victim was already fighting the aggressor: self-defence
  if (member_array(aggressor, victim->query_attacker_list()) != -1)
    return;

  place = environment(victim);
  if (!place || is_enforcer(aggressor))
    return;

  jurisdiction = query_jurisdiction(place);
  if (!strlen(jurisdiction) || !is_protected(victim, jurisdiction))
    return;

  type = victim->has_component("guard") ? CRIME_GUARD_ASSAULT : CRIME_ASSAULT;
  report_crime(aggressor, type, victim, place, sees(victim, place));
}

/**
 * A living has been killed by another. Called by the death code before the
 * victim leaves the place.
 *
 * It is murder if the law of the place protects the victim, with the same
 * exceptions as report_attack, and the victim did not start the fight (a
 * victim who attacked the killer first died in self-defence).
 *
 * A death at the hands of a guard (or of a guard sent after the victim) is
 * never murder: it keeps only query_death_keep_percent() of the victim's points
 * in the guard's country. Any death ends a sentence being served.
 *
 * @param killer who dealt the last blow
 * @param victim who died
 */
void report_death(object killer, object victim)
{
  object place;
  string jurisdiction, citizenship;

  if (!killer || !victim || killer == victim)
    return;

  // a death ends any sentence being served
  clear_sentence(victim);

  // dying at the hands of the law pays part of the debt
  if (is_enforcer(killer))
  {
    citizenship = citizenship_of(killer);
    if (victim->query_player() && strlen(citizenship))
      reduce(victim->query_name(), citizenship, query_death_keep_percent());
    return;
  }

  place = environment(victim);
  if (!place)
    return;

  if (started(victim, killer) && !started(killer, victim))
    return;

  jurisdiction = query_jurisdiction(place);
  if (!strlen(jurisdiction) || !is_protected(victim, jurisdiction))
    return;

  report_crime(killer, CRIME_MURDER, victim, place);
}

// --- Justice: beating, arrest, sentence, fines ----------------------------------

/**
 * The prison as a place: a location file (.o) or a plain room; nil when it
 * cannot be loaded.
 */
private object prison_place(string file)
{
  object place;

  place = nil;
  if (!strlen(file))
    return nil;

  catch(place = load_object(LOCATION_HANDLER)->resolve_to_location(file));
  if (!place)
    catch(place = load_object(file));

  return place;
}

/**
 * The fine a living owes in a citizenship: its points times
 * query_fine_per_point(), grown by query_fine_level_percent() for each level.
 *
 * @param who the culprit
 * @param citizenship a diplomacy-graph name (its root is used)
 * @return the fine in base coins, 0 when nothing is owed
 */
int query_fine_of(object who, string citizenship)
{
  int points;

  if (!who)
    return 0;

  points = query_points(who->query_name(), citizenship);
  if (points <= 0)
    return 0;

  return points * query_fine_per_point() *
         (100 + query_fine_level_percent() * (int)who->query_level()) / 100;
}

/**
 * Takes `amount` base coins from a living that has them.
 */
private void take_money(object who, int amount)
{
  who->pay_money(handler("money", this_object())->create_money_array(amount));
}

/**
 * How long a living still has to serve. A sentence that has run out is ended
 * here, the points reduced and the prisoner told (see release), so nobody has
 * to watch the clock: whoever needs to know asks.
 *
 * @param who the living
 * @return the seconds left, 0 when free
 */
int query_jail_left(object who)
{
  mapping sentence;
  int left;

  sentence = find_sentence(who);
  if (!sentence)
    return 0;

  left = sentence["until"] - time();
  if (left > 0)
    return left;

  release(who);
  return 0;
}

/**
 * The location a prisoner serves in.
 *
 * @param who the living
 * @return the location file, "" when it is not serving a sentence
 */
string query_jail_prison(object who)
{
  mapping sentence;

  sentence = find_sentence(who);
  return sentence ? sentence["prison"] : "";
}

/**
 * Ends a sentence: the prisoner keeps query_release_keep_percent() of the
 * points and is told it is free to go. Does not move it.
 *
 * @param who the prisoner
 */
void release(object who)
{
  mapping sentence;

  sentence = find_sentence(who);
  if (!sentence)
    return;

  if (who->query_player())
    reduce(who->query_name(), sentence["root"], query_release_keep_percent());

  clear_sentence(who);
  tell_object(who, _LANG_CRIMES_RELEASED);
}

/**
 * Pays what a living owes in a citizenship: the record is wiped, and the
 * sentence ends if it was for that country.
 *
 * @param payer who pays (the culprit or somebody else)
 * @param who the culprit
 * @param citizenship a diplomacy-graph name (its root is used)
 * @return the fine paid, 0 if the payer cannot afford it, -1 if nothing is owed
 */
int pay_fine(object payer, object who, string citizenship)
{
  mapping sentence;
  int fine;

  fine = query_fine_of(who, citizenship);
  if (fine <= 0)
    return -1;

  if (!payer || (int)payer->query_value() < fine)
    return 0;

  take_money(payer, fine);
  pardon(who->query_name(), citizenship);

  sentence = find_sentence(who);
  if (sentence && sentence["root"] == root_of(citizenship))
    clear_sentence(who);

  log_file("crimes", "[" + ctime(time(), 4) + "] " + payer->query_name() +
           " paid " + fine + " for " + who->query_name() + " in " +
           root_of(citizenship) + "\n");
  return fine;
}

/**
 * Pays the fine of a prisoner and frees them.
 *
 * @param payer who pays
 * @param prisoner the jailed living
 * @param citizenship the citizenship whose fine is paid
 * @return as pay_fine, or -2 if the living is not serving a sentence for that
 *         country
 */
int bail(object payer, object prisoner, string citizenship)
{
  mapping sentence;
  int fine;

  sentence = find_sentence(prisoner);
  if (!sentence || sentence["root"] != root_of(citizenship))
    return -2;

  fine = pay_fine(payer, prisoner, citizenship);
  if (fine > 0)
    tell_object(prisoner, _LANG_CRIMES_BAILED);
  else if (fine == -1)
    release(prisoner);

  return fine;
}

/**
 * What bribing the jailer costs a prisoner.
 *
 * @param who the prisoner
 * @return base coins: query_bribe_cost_per_point() for each point owed, at least
 *         one point's worth
 */
int query_bribe_cost(object who)
{
  mapping sentence;
  int cost;

  sentence = find_sentence(who);
  cost = sentence ? query_points(who->query_name(), sentence["root"]) *
                    query_bribe_cost_per_point() : 0;

  return (cost < query_bribe_cost_per_point()) ? query_bribe_cost_per_point()
                                               : cost;
}

/**
 * A prisoner bribes the jailer. The price is query_bribe_cost_per_point() for
 * each point owed, paid win or lose; with query_bribe_chance() percent the
 * sentence ends as if served, otherwise it adds query_bribe_fail_points()
 * points and the time they are worth.
 *
 * @param who the prisoner (a player)
 * @return 1 it worked, 0 it failed, -1 not serving a sentence, -2 cannot pay
 */
int bribe(object who)
{
  mapping sentence;
  int cost, extra, ends;

  sentence = find_sentence(who);
  if (!sentence || !who->query_player())
    return -1;

  cost = query_bribe_cost(who);
  if ((int)who->query_value() < cost)
    return -2;

  take_money(who, cost);

  if (random(100) < query_bribe_chance())
  {
    release(who);
    return 1;
  }

  // caught: more points, and the time they are worth, within the longest
  // sentence
  record_offence(who->query_name(), sentence["root"], "bribe",
                 query_bribe_fail_points(), nil, environment(who));
  extra = query_bribe_fail_points() * query_jail_seconds_per_point();
  ends = sentence["until"] + extra;
  if (ends > time() + query_jail_max_seconds())
    ends = time() + query_jail_max_seconds();
  sentence["until"] = ends;
  set_sentence(who, sentence);

  return 0;
}

/**
 * Ends a pending beating that did not end in an arrest.
 */
private void let_go(object victim)
{
  tell_object(victim, _LANG_CRIMES_LET_GO);
}

/**
 * Writes the sentence of a living that is taken to a cell, `points` worth of
 * time within the longest sentence. It does not move the living.
 *
 * @return the seconds of the sentence
 */
private int imprison(object victim, string citizenship, string prison,
                     int points)
{
  int seconds;

  seconds = points * query_jail_seconds_per_point();
  if (seconds > query_jail_max_seconds())
    seconds = query_jail_max_seconds();

  set_sentence(victim, ([ "prison" : prison, "until" : time() + seconds,
                          "root" : root_of(citizenship) ]));
  return seconds;
}

/**
 * The cells of a citizenship that can be loaded, the least occupied first and
 * those with the same number of occupants in random order.
 *
 * @param citizenship a diplomacy-graph name
 * @return the files of the cells, as the citizenship lists them
 */
string * query_cells(string citizenship)
{
  string * prisons, * cells, * ordered;
  object cell;
  object * inv;
  int * keys;
  int i, j, n, best;

  prisons = query_prisons(citizenship);
  cells = ({ });
  keys = ({ });

  for (i = 0; i < sizeof(prisons); i++)
  {
    cell = prison_place(prisons[i]);
    if (!cell)
      continue;

    // each occupant weighs more than any random tie-break
    inv = all_inventory(cell);
    n = 0;
    for (j = 0; j < sizeof(inv); j++)
      if (inv[j] && living(inv[j]))
        n++;

    cells += ({ prisons[i] });
    keys += ({ n * 1000 + random(1000) });
  }

  // the smallest key first, each one used up as it is taken
  ordered = ({ });
  for (i = 0; i < sizeof(cells); i++)
  {
    best = 0;
    for (j = 1; j < sizeof(cells); j++)
      if (keys[j] < keys[best])
        best = j;

    ordered += ({ cells[best] });
    keys[best] = 0x7fffffff;
  }

  return ordered;
}

/**
 * What follows the beating: a sentence in the prison of the citizenship, or,
 * where it has none, the fine on the spot if the culprit can pay. Called out so
 * the blow that caused it has finished.
 */
void arrest(string citizenship, object victim)
{
  object place, cell;
  string root, name, prison;
  string * cells;
  int points, fine;

  if (!victim || victim->query_dead())
    return;

  place = environment(victim);
  root = root_of(citizenship);
  name = victim->query_name();

  // an NPC has no record unless they are kept: it is sentenced all the same
  points = query_points(name, root);
  if (points <= 0 && !victim->query_player())
    points = query_npc_arrest_points();

  // nothing owed: the guards just leave them lying where they fell
  if (points <= 0)
  {
    let_go(victim);
    return;
  }

  // the cell with fewest prisoners; one that does not load counts as none
  cells = query_cells(citizenship);
  prison = sizeof(cells) ? cells[0] : "";
  cell = strlen(prison) ? prison_place(prison) : nil;

  if (cell)
  {
    imprison(victim, citizenship, prison, points);

    tell_object(victim, _LANG_CRIMES_ARRESTED);
    if (place)
      tell_room(place, replace_string(_LANG_CRIMES_ARRESTED_ROOM, "$who$",
                                      victim->query_cap_name()), ({ victim }));
    victim->move_living("X", prison);
    return;
  }

  // no prison: the money, if there is any, else they go free after the beating
  fine = query_fine_of(victim, citizenship);
  if (fine > 0 && victim->query_player() &&
      (int)victim->query_value() >= fine)
  {
    take_money(victim, fine);
    pardon(name, citizenship);
    tell_object(victim, replace_string(_LANG_CRIMES_FINED_ON_SPOT, "$amount$",
                                       "" + fine));
    return;
  }

  let_go(victim);
}

/**
 * The health code asks this when a blow of `guard` would bring `victim` to 0
 * hp or less. If the guard belongs to a citizenship whose guards do not kill
 * (query_lethal), the blow is turned into a beating: the fight stops between the
 * victim and every guard fighting it, the victim is passed out for
 * query_passed_out_seconds() and, a moment later, arrested (see arrest).
 *
 * Only players and NPCs with a citizenship that is not an enemy of the guard's
 * are spared; fauna, monsters and the guards' enemies in war die as always. The
 * caller leaves the victim at 1 hp when this answers 1.
 *
 * @param guard who dealt the blow
 * @param victim who would die
 * @return 1 if the blow does not kill, 0 if combat goes on as usual
 */
int spare_blow(object guard, object victim)
{
  mixed * attackers;
  string citizenship, theirs;
  int i;

  if (!guard || !victim || guard == victim || !is_enforcer(guard))
    return 0;

  citizenship = citizenship_of(guard);
  if (!strlen(citizenship) || query_lethal(citizenship))
    return 0;

  if (!victim->query_player())
  {
    theirs = citizenship_of(victim);
    if (!strlen(theirs) || diplomacy()->is_enemy(citizenship, theirs))
      return 0;
  }

  // the fight is over on both sides, for every guard that was after them
  attackers = ({ guard }) + victim->query_attacker_list();
  for (i = 0; i < sizeof(attackers); i++)
    if (attackers[i] && is_enforcer(attackers[i]))
    {
      attackers[i]->stop_fight(victim, 1);
      victim->stop_fight(attackers[i], 1);
    }

  victim->add_timed_property(PASSED_OUT_PROP, _LANG_CRIMES_PASSED_OUT_PROP,
                             query_passed_out_seconds() / 2);
  tell_object(victim, _LANG_CRIMES_BEATEN);
  if (environment(victim))
    tell_room(environment(victim), replace_string(_LANG_CRIMES_BEATEN_ROOM,
                "$who$", victim->query_cap_name()), ({ victim }));

  call_out("arrest", 1, citizenship, victim);
  return 1;
}

/**
 * A prisoner who logs in is checked a moment later, once it stands somewhere.
 */
void check_prisoner(object who)
{
  mapping sentence;
  object cell;

  if (!who || !environment(who))
    return;

  // frees the ones whose time is up
  if (query_jail_left(who) <= 0)
    return;

  // still serving but somewhere else: back to the cell
  sentence = find_sentence(who);
  cell = prison_place(sentence["prison"]);
  if (cell && environment(who) != cell)
  {
    tell_object(who, _LANG_CRIMES_PUT_BACK);
    who->move_living("X", sentence["prison"]);
  }
}

/**
 * Called when a player enters the game: ends a sentence that ran out while they
 * were away, or puts back in the prison one who is still serving.
 *
 * @param who the player
 */
void watch_login(object who)
{
  if (who && find_sentence(who))
    call_out("check_prisoner", 2, who);
}

// --- Capture: a guard takes a beaten target to prison ---------------------------

/**
 * Tells a player, by name, if they are still playing.
 */
private void tell_winner(string name, string text, object creature)
{
  object winner;

  winner = find_player(name);
  if (winner)
    tell_object(winner, replace_string(text, "$who$",
                                       creature->query_cap_name()));
}

/**
 * A player has beaten an NPC in non-lethal combat (called by the health code
 * a moment after the fight ends). Nothing happens unless the NPC is the target
 * of a capture objective of a quest the player is doing, no arrest is already
 * under way for it, and the law of the place is the quest's capture
 * citizenship or one under it, with cells.
 *
 * Then the nearest free guard of the place's community closer than
 * query_arrest_guard_range() is given the escort component and sent for the
 * NPC (see lib/npc/components/escort.c). A free guard is a person of the
 * community's census whose template carries the guard component, not posted on
 * a door, alive, not fighting and not on another errand. If there is none the
 * player is told, nothing counts and the NPC comes round. Otherwise the NPC
 * is held unconscious until it is jailed or the arrest is given up.
 *
 * @param winner the player who won the fight
 * @param creature the NPC that lost it
 */
void capture(object winner, object creature)
{
  object place, community, comp, guard;
  object * guards;
  string citizenship, wanted_by;
  mapping record;

  if (!winner || !creature || !winner->query_player() ||
      creature->query_player() || creature->query_dead())
    return;

  // only a target of the winner's quests is worth the guards' time
  wanted_by = (string)handler(QUESTS_HANDLER, winner)->
    query_capture_citizenship(winner, creature);
  if (!strlen(wanted_by))
    return;

  record = captures[creature];
  if ((record && record["until"] > time()) || find_sentence(creature))
    return;

  // the law of the place must be the one that wants it, or one under it
  place = environment(creature);
  citizenship = query_jurisdiction(place);
  if (!strlen(citizenship) || root_of(citizenship) != root_of(wanted_by))
  {
    tell_winner(winner->query_name(), _LANG_CRIMES_CAPTURE_NO_LAW, creature);
    return;
  }

  // the cells of the place's law, or else those of the one that wants it
  if (!sizeof(query_cells(citizenship)))
    citizenship = wanted_by;
  if (!sizeof(query_cells(citizenship)))
  {
    tell_winner(winner->query_name(), _LANG_CRIMES_CAPTURE_NO_CELL, creature);
    return;
  }

  community = community_of(place);
  guards = community ? pick_guards(community, place, 1,
                                   query_arrest_guard_range()) : ({ });
  if (!sizeof(guards))
  {
    tell_winner(winner->query_name(), _LANG_CRIMES_CAPTURE_NO_GUARD, creature);
    return;
  }

  guard = guards[0];
  guard->add_component("escort", ([ ]));
  comp = (object)guard->query_component_by_type("escort");
  if (!comp)
  {
    tell_winner(winner->query_name(), _LANG_CRIMES_CAPTURE_NO_GUARD, creature);
    return;
  }

  captures[creature] = ([ "winner" : winner->query_name(),
                          "citizenship" : citizenship,
                          "guard" : guard,
                          "until" : time() + query_arrest_seconds() ]);
  hold_captive(creature);

  // if the guard dies on the way, nobody is left to let the target go
  call_out("capture_expired", query_arrest_seconds() + 10, creature);

  comp->escort(creature, winner->query_name(), citizenship,
               query_arrest_seconds());
  tell_object(winner, replace_string(_LANG_CRIMES_CAPTURE_COMING, "$who$",
                                     creature->query_cap_name()));
}

/**
 * The escort calls it when the guard stands over the target: it is in custody
 * and stays held until it is jailed.
 *
 * @param guard the guard, with the escort component
 * @param creature the NPC being taken
 */
void take_custody(object guard, object creature)
{
  mapping record;

  record = creature ? captures[creature] : nil;
  if (!record || record["guard"] != guard || record["custody"])
    return;

  record["custody"] = 1;
}

/**
 * Keeps a target of an arrest unconscious a few more seconds. The escort asks
 * it on every look, so the target stays down while the arrest lasts and wakes
 * soon after it ends.
 *
 * @param creature the NPC being taken
 */
void hold_captive(object creature)
{
  if (creature && captures[creature])
    creature->add_timed_property(PASSED_OUT_PROP, _LANG_CRIMES_CAPTURE_HELD,
                                 CRIME_CAPTIVE_HOLD);
}

/**
 * Gives an arrest up: the target is let wake and the winner is told.
 *
 * @param creature the NPC that was being taken
 */
void release_captive(object creature)
{
  mapping record;

  record = creature ? captures[creature] : nil;
  if (!record)
    return;

  captures[creature] = nil;
  creature->remove_timed_property(PASSED_OUT_PROP);
  tell_winner(record["winner"], _LANG_CRIMES_CAPTURE_FAILED, creature);
}

/**
 * An arrest whose guard never finished it, such as one killed on the way.
 */
void capture_expired(object creature)
{
  if (creature && captures[creature] && !find_sentence(creature))
    release_captive(creature);
}

/**
 * The escort calls it when the guard stands in a cell with the target. The
 * target is jailed for query_npc_arrest_points() worth of sentence, the
 * capture objective of the winner advances, and the sentence is served out in
 * the cell, after which the NPC is gone for good.
 *
 * @param guard the guard, with the escort component
 * @param creature the NPC being taken
 * @param prison the file of the cell the escort walked to
 * @return 1 if it was jailed, 0 if the guard and the target are not both in
 *         that cell
 */
int deliver_prisoner(object guard, object creature, string prison)
{
  mapping record;
  object cell, winner;
  int seconds;

  record = creature ? captures[creature] : nil;
  if (!record || !guard || record["guard"] != guard)
    return 0;

  cell = prison_place(prison);
  if (!cell || environment(guard) != cell || environment(creature) != cell)
    return 0;

  captures[creature] = nil;
  seconds = imprison(creature, record["citizenship"], prison,
                     query_npc_arrest_points());

  tell_room(cell, replace_string(_LANG_CRIMES_CAPTURE_JAILED_ROOM, "$who$",
                                 creature->query_cap_name()), ({ creature }));

  winner = find_player(record["winner"]);
  if (winner)
  {
    tell_object(winner, replace_string(_LANG_CRIMES_CAPTURE_JAILED, "$who$",
                                       creature->query_cap_name()));
    catch(handler(QUESTS_HANDLER, winner)->captured(winner, creature));
  }

  call_out("end_sentence", seconds, creature);
  return 1;
}

/**
 * The sentence of a jailed NPC is served: it leaves for good. A person of a
 * census is retired from it, so the population brings somebody else; anything
 * else is just removed.
 */
void end_sentence(object creature)
{
  object area;

  if (!creature || !find_sentence(creature))
    return;

  clear_sentence(creature);
  if (environment(creature))
    tell_room(environment(creature),
              replace_string(_LANG_CRIMES_CAPTIVE_GONE, "$who$",
                             creature->query_cap_name()), ({ creature }));

  area = creature->query_persisted() ?
    (object)load_object(AREA_HANDLER)->query_area(creature->query_npc_area_path())
    : nil;
  if (area)
    area->retire_npc(creature->query_uuid());
  else
    creature->dest_me();
}
