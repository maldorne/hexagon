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
 */

#include <mud/config.h>
#include <common/properties.h>
#include <areas/diplomacy.h>
#include <crimes.h>
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

string query_save_file();

void create()
{
  records = ([ ]);
  ::create();
  restore_object(query_save_file(), 1);
}

// Where the records are kept. A game's own handler overrides it to point at
// /save/games/<game>/crimes.o.
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

// --- The law ----------------------------------------------------------------

// The diplomacy graph of this handler's game.
private object diplomacy()
{
  return handler("diplomacy", this_object());
}

// The country at the top of a citizenship's parent chain.
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

/** The location a citizenship takes its prisoners to, "" when none. */
string query_prison(string citizenship)
{
  return diplomacy()->query_prison(citizenship);
}

// --- Records ----------------------------------------------------------------

// Brings an entry's points up to date with the time elapsed since its stamp.
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

// The record of a player under a root citizenship, decayed up to now; nil when
// there is none and `create` is 0.
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

// The status a number of points amounts to.
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

// --- Reporting a crime ------------------------------------------------------

// Whether a living, standing where the crime happened, could make out the
// culprit.
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

// Whether somebody in the place with a citizenship saw who did it. A hidden
// culprit is seen by nobody.
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

// Adds an offence to a player's record under a root citizenship.
private void record_offence(string name, string root, string type, int points,
                            object victim, object place)
{
  mapping entry;
  mixed short;
  int limit;

  entry = find_entry(name, root, 1);
  short = place->query_short();

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
 * culprit.
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
