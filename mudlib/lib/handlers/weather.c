/*
 * weather.c, neverbot@CcMud 22/10/03
 * Version 3.1
 *
 * After much tweaking of the old weather system, rewriting it from
 * scratch turned out to be the cleanest option.
 *
 * Moonlight added at night, neverbot 20/02/04
 * Snow added; only rain and hail existed before, neverbot 20/02/04
 * Turned into a base class for per-game weather handlers, three moons
 * collapsed to a single one, identifiers and comments translated to
 * English, neverbot 05/07/26
 */

// Base class instantiated per game through the `handler("weather")` efun:
// the lookup tries `<game_root>/handlers/weather.c` first and falls back
// to this shared instance if a game does not provide its own subclass.
//
// State variables and public API are English by convention. User-facing
// strings remain in the mud's active language pending the i18n split.

inherit "/lib/core/object.c";

#include <mud/config.h>
#include <areas/weather.h>
#include <language.h>

// Duration constants for mud-time.
// A tick is 60 real-time seconds (one minute).
// A day is 60 ticks (one real hour).
// A year is 366 days (a little over two real weeks).
#define TICKS 60
#define DAY 60
#define YEAR 366

int ticks,          // [0..60]
    hour_of_day,    // [0..23]
    day_of_week,    // [0..6]
    day_of_month,   // [0..month_days()-1]
    day_of_year,    // [0..365]
    month,          // [0..11]
    season,         // [0..3]
    year;

// Current lunar phase for the single base-handler moon.
int moon;

// Hour of day when the moon becomes hidden and when it rises again.
int * moon_hidden;

// Per-zone climate state, keyed by zone name (not path).
mapping zones;

// Objects registered to receive weather event notifications.
static private mapping my_obs;

// Prototypes.
void check_zones();
string month_string();
int month_days();
void update_weather(int flag);
int query_day();
int query_dawn_time();
int query_nightfall_time();
void check_season(int flag);
string query_save_file();

void create()
{
  ticks = 0;
  hour_of_day = 0;
  day_of_week = 0;
  day_of_month = 0;
  day_of_year = 0;
  month = 0;
  season = 0;
  year = 0;

  moon = 0;
  moon_hidden = ({ 6, 21 });

  my_obs = ([ ]);
  zones = ([ ]);

  restore_object(query_save_file(), 1);

  // Refresh in case new zones were added to the table.
  check_zones();

  ::create();
}

// Save file location. Subclasses override this to point at
// /save/games/<game>/weather.o instead of the shared default.
string query_save_file()
{
  return SAVE_FILE;
}

void setup()
{
  string name;
  int cnum;

  // Anticloning: only the master instance loaded through handler()
  // should exist. Path is not hardcoded so per-game subclasses pass;
  // any actual clone (file_name suffixed with #N) is destroyed.
  if (sscanf(file_name(this_object()), "%s#%d", name, cnum) == 2)
  {
    write("Este objeto no puede ser clonado.\n");
    dest_me();
    return;
  }

  reset_get();
  set_name("tiempo");
  set_short("Controlador extraordinario del tiempo");
  set_long("¡Éste es el controlador extraordinario del tiempo!\n");
  add_alias("controlador");

  call_out("update_low", TICKS);
  // move() is masked below to always land in HANDLERS_HOME.
  move("bing");
}

/* have this move here to get it to my weather room ;) (Sleaze) */
int move(mixed dest, varargs mixed messin, mixed messout)
{
  ::move(HANDLERS_HOME, messin, messout);
}

void save_weather()
{
  save_object(query_save_file(), 1);
}

void dest_me()
{
  save_object(query_save_file(), 1);
  ::dest_me();
}

// ********************************************************************
// Event notification registry.
// ********************************************************************

mapping query_my_obs() { return my_obs; }

// Register an observer for weather events.
void notify_me(object ob)
{
  if (!my_obs[ob])
    my_obs[ob] = 0;

  my_obs[ob]++;
}

// Unregister an observer.
void unnotify_me(object ob)
{
  if (!my_obs[ob])
    return;
  if (--my_obs[ob] <= 0)
    my_obs = m_delete(my_obs, ob);
}

// Dispatch a weather change event to all interested observers.
//
// flag:   FLAG_DAWN, FLAG_NIGHTFALL, FLAG_CLIMATE, FLAG_CHECK.
// zone:   zone name (not path) — only for FLAG_CLIMATE.
// values: array of six ints — {rain_old, wind_old, temp_old,
//         rain_new, wind_new, temp_new}.
void weather_inform(int flag, varargs string zone, int * values)
{
  object * obs;
  int i;

  // Day/night transitions reach every observer.
  if ((flag == FLAG_NIGHTFALL) || (flag == FLAG_DAWN))
  {
    obs = m_indices(my_obs);
    for (i = 0; i < sizeof(obs); i++)
      if (objectp(obs[i]))
        event(obs[i], "weather", flag);
      else
        my_obs = m_delete(my_obs, obs[i]);
    return;
  }

  // FLAG_CHECK reaches only timed npcs.
  if (flag == FLAG_CHECK)
  {
    obs = m_indices(my_obs);
    for (i = 0; i < sizeof(obs); i++)
      if (objectp(obs[i]))
      {
        if (obs[i]->query_npc() && obs[i]->query_timed_npc())
          event(obs[i], "weather", flag);
      }
      else
        my_obs = m_delete(my_obs, obs[i]);
    return;
  }

  obs = m_indices(my_obs);

  // Climate transitions reach rooms in the affected zone (players and
  // npcs living inside those rooms pick the event up via inventory
  // propagation).
  for (i = 0; i < sizeof(obs); i++)
  {
    if (!objectp(obs[i]))
    {
      my_obs = m_delete(my_obs, obs[i]);
      break;
    }

    // A room-level observer has no environment.
    if (!environment(obs[i]))
    {
      if (zone == table(WEATHER_TABLE)->query_zone(file_name(obs[i])))
        event(obs[i], "weather", flag, values);
    }

    // Player observers should not appear in my_obs, but keep the
    // defensive dispatch just in case.
    if (obs[i]->query_player())
    {
      if ((environment(obs[i])->query_outside()) &&
          (zone == table(WEATHER_TABLE)->query_zone(file_name(environment(obs[i])))))
        event(obs[i], "weather", flag, values);
    }
  }
}

// ********************************************************************

// Heartbeat driving the mud-time clock and periodic weather updates.
void update_low()
{
  ticks++;

  // Real-time ticks map to in-game hours through TICKS: advance the
  // in-game hour whenever the current tick's fraction of the day
  // exceeds the current hour.
  if ((ticks * 24) / TICKS > hour_of_day)
  {
    hour_of_day++;
    if (hour_of_day == query_dawn_time())
      weather_inform(FLAG_DAWN);
    else if (hour_of_day == query_nightfall_time())
      weather_inform(FLAG_NIGHTFALL);
    // Hourly ping for timed npcs.
    else
      weather_inform(FLAG_CHECK);

    // End of day: roll everything forward.
    if (hour_of_day >= 24)
    {
      hour_of_day = 0;
      day_of_month++;
      day_of_week++;
      day_of_year++;

      // Advance the moon one phase per day; cycles repeat every 10 days.
      moon = (moon + 1) % 10;

      // Season change on the 22nd of March/June/September/December
      // (indices 21 in day_of_month, months 2/5/8/11).
      if (day_of_month == 21)
      {
        if (month == 11)
        {
          season = 0; // winter starts
          check_season(0);
        }
        if (month == 2)
        {
          season = 1; // spring starts
          check_season(0);
        }
        if (month == 5)
        {
          season = 2; // summer starts
          check_season(0);
        }
        if (month == 8)
        {
          season = 3; // autumn starts
          check_season(0);
        }
      }

      // Weeks wrap every 7 days.
      if (day_of_week >= 7)
        day_of_week = 0;

      // Month roll-over.
      if (day_of_month >= month_days())
      {
        day_of_month = 0;
        month++;
        if (month >= 12)
        {
          month = 0;
          day_of_year = 0;
          year++;
        }
      }
    }

    // End of the mud-time update.

    // Refresh climate every four in-game hours. The interval is
    // arbitrary but avoids updating too aggressively.
    if ((hour_of_day % 4) == 0)
      update_weather(0);
  }

  save_weather();

  ticks = ticks % TICKS;
  call_out("update_low", TICKS);
}

// Used by timed_npcs and similar consumers.
int * query_date_data()
{
  return ({ hour_of_day, day_of_year + 1, month + 1, season + 1, year, day_of_month, });
}

string date_string()
{
  int * data;
  data = query_date_data();
  return _LANG_WEATHER_DATE_STRING;
}

int month_days()
{
  switch (month)
  {
    case 0:   return 31;  // january
    case 1:   return 29;  // february
    case 2:   return 31;  // march
    case 3:   return 30;  // april
    case 4:   return 31;  // may
    case 5:   return 30;  // june
    case 6:   return 31;  // july
    case 7:   return 31;  // august
    case 8:   return 30;  // september
    case 9:   return 31;  // october
    case 10:  return 30;  // november
    case 11:  return 31;  // december
    default:  return 30;
  }
}

string month_string()
{
  string * names;
  names = _LANG_WEATHER_MONTHS;
  if ((month < 0) || (month >= sizeof(names)))
    return names[sizeof(names) - 1];
  return names[month];
}

// Adjust each zone's average climate values according to the current
// season.
//
// flag == 0: natural season change (logical order); values always shift.
// flag == 1: zone reset — only shift if we are in winter or summer.
void check_season(int flag)
{
  string * zone_names;
  int i;

  zone_names = keys(zones);

  for (i = 0; i < sizeof(zone_names); i++)
  {
    switch (season)
    {
      case 0: // winter
        zones[zone_names[i]][3] += 20;
        zones[zone_names[i]][4] += 20;
        zones[zone_names[i]][5] += 20;
        break;
      case 1: // spring — revert only when arriving from winter
        if (!flag)
        {
          zones[zone_names[i]][3] -= 20;
          zones[zone_names[i]][4] -= 20;
          zones[zone_names[i]][5] -= 20;
        }
        break;
      case 2: // summer
        zones[zone_names[i]][3] -= 20;
        zones[zone_names[i]][4] -= 20;
        zones[zone_names[i]][5] -= 20;
        break;
      case 3: // autumn — revert only when arriving from summer
        if (!flag)
        {
          zones[zone_names[i]][3] += 20;
          zones[zone_names[i]][4] += 20;
          zones[zone_names[i]][5] += 20;
        }
        break;
    }

    // Clamp to [0..100].
    if (zones[zone_names[i]][3] < 0)   zones[zone_names[i]][3] = 0;
    if (zones[zone_names[i]][3] > 100) zones[zone_names[i]][3] = 100;
    if (zones[zone_names[i]][4] < 0)   zones[zone_names[i]][4] = 0;
    if (zones[zone_names[i]][4] > 100) zones[zone_names[i]][4] = 100;
    if (zones[zone_names[i]][5] < 0)   zones[zone_names[i]][5] = 0;
    if (zones[zone_names[i]][5] > 100) zones[zone_names[i]][5] = 100;
  }
}

// Reset every zone to its original table values. Useful when the
// weather table's exit graph or averages change and we need to
// re-seed. Note: map_copy() ensures we don't share the mapping with
// the WEATHER_TABLE object.
void reset_zones()
{
  log_file(LOG_FILE, "Resetting weather zones " + ctime(time(), 4) + "\n");

  zones = map_copy(table(WEATHER_TABLE)->query_zones());
  check_season(1);
  save_weather();
}

// Add any zones defined in the table that aren't in our state yet.
void check_zones()
{
  mapping table_zones;
  string * table_names;
  string * names;
  int i;
  int changed;

  changed = 0;
  table_zones = map_copy(table(WEATHER_TABLE)->query_zones());
  table_names = keys(table_zones);
  names = keys(zones);

  for (i = 0; i < sizeof(table_names); i++)
  {
    if (member_array(table_names[i], names) == -1)
    {
      zones[table_names[i]] = table_zones[table_names[i]];
      log_file(LOG_FILE, "Weather zone added: " + table_names[i] +
        " " + ctime(time(), 4) + "\n");
      changed = 1;
    }
  }

  if (changed)
    save_weather();
}

// After changing a zone's climate, nudge its neighbours toward the
// midpoint so movement between adjacent zones does not feel abrupt.
void balance_weather(string * modified_zones)
{
  string * zones_to_modify;
  int i, j;
  mixed * old_zone;
  mixed * new_zone;
  mapping zones_to_inform;

  zones_to_modify = ({ });
  old_zone = ({ });
  new_zone = ({ });
  zones_to_inform = ([ ]);

  for (i = 0; i < sizeof(modified_zones); i++)
  {
    // Zone whose climate we just modified.
    old_zone = zones[modified_zones[i]];

    // List of neighbours.
    zones_to_modify = old_zone[6];
    // Drop zones already modified this pass — a zone may otherwise be
    // balanced multiple times if several of its neighbours changed.
    zones_to_modify -= modified_zones;

    for (j = 0; j < sizeof(zones_to_modify); j++)
    {
      // Adjacent zone we are about to balance.
      new_zone = zones[zones_to_modify[j]];
      zones_to_inform[zones_to_modify[j]] = ({ new_zone[0], new_zone[1], new_zone[2], });
      new_zone[0] -= (new_zone[0] - old_zone[0]) / 2;
      new_zone[1] -= (new_zone[1] - old_zone[1]) / 2;
      new_zone[2] -= (new_zone[2] - old_zone[2]) / 2;
      // Clamp to [0..100].
      if (new_zone[0] < 0)   new_zone[0] = 0;
      if (new_zone[1] < 0)   new_zone[1] = 0;
      if (new_zone[2] < 0)   new_zone[2] = 0;
      if (new_zone[0] > 100) new_zone[0] = 100;
      if (new_zone[1] > 100) new_zone[1] = 100;
      if (new_zone[2] > 100) new_zone[2] = 100;
      zones_to_inform[zones_to_modify[j]] += ({ new_zone[0], new_zone[1], new_zone[2], });
    }
  }

  zones_to_modify = keys(zones_to_inform);
  for (i = 0; i < sizeof(zones_to_modify); i++)
    weather_inform(FLAG_CLIMATE, zones_to_modify[i], zones_to_inform[zones_to_modify[i]]);
}

// Mutate MODIFY random zones. flag == 1 saves when done.
void update_weather(int flag)
{
  string * names;
  string * names_to_modify;
  int roll, i, delta;
  mixed * zone_data;
  int * inform;

  names_to_modify = ({ });
  zone_data = ({ });
  inform = ({ });

  names = keys(zones);
  for (i = 0; i < MODIFY; i++)
  {
    roll = random(sizeof(names));
    names_to_modify += ({ names[roll] });
    // Drop the picked name so we always select distinct zones.
    names -= ({ names[roll] });
    // delete() did not work here... :(
    // delete(names, roll, 1);
  }

  // names_to_modify holds the list of zones to change.
  // For each of them bump or drop the rain / wind / temperature
  // (cold) values at random:

  for (i = 0; i < sizeof(names_to_modify); i++)
  {
    inform = ({ });
    roll = random(2); // 0 == decrease, 1 == increase

    zone_data = zones[names_to_modify[i]];
    // Store the previous values.
    inform += ({ zone_data[0], zone_data[1], zone_data[2], });

    if (roll)
    {
      // rain
      delta = zone_data[0] - zone_data[3];
      if (delta < 0)  delta *= -1;
      if (delta < 10) delta = DEF_INC;
      zone_data[0] += random(delta);
      // wind
      delta = zone_data[1] - zone_data[4];
      if (delta < 0)  delta *= -1;
      if (delta < 10) delta = DEF_INC;
      zone_data[1] += random(delta);
      // temperature
      delta = zone_data[2] - zone_data[5];
      if (delta < 0)  delta *= -1;
      if (delta < 10) delta = DEF_INC;
      zone_data[2] += random(delta);
    }
    else
    {
      // rain
      delta = zone_data[0] - zone_data[3];
      if (delta < 0)  delta *= -1;
      if (delta < 10) delta = DEF_INC;
      zone_data[0] -= random(delta);
      // wind
      delta = zone_data[1] - zone_data[4];
      if (delta < 0)  delta *= -1;
      if (delta < 10) delta = DEF_INC;
      zone_data[1] -= random(delta);
      // temperature
      delta = zone_data[2] - zone_data[5];
      if (delta < 0)  delta *= -1;
      if (delta < 10) delta = DEF_INC;
      zone_data[2] -= random(delta);
    }

    // Clamp to [0..100].
    if (zone_data[0] < 0)   zone_data[0] = 0;
    if (zone_data[1] < 0)   zone_data[1] = 0;
    if (zone_data[2] < 0)   zone_data[2] = 0;
    if (zone_data[0] > 100) zone_data[0] = 100;
    if (zone_data[1] > 100) zone_data[1] = 100;
    if (zone_data[2] > 100) zone_data[2] = 100;

    // Store the new values as well.
    inform += ({ zone_data[0], zone_data[1], zone_data[2], });

    // Notify the climate change event.
    weather_inform(FLAG_CLIMATE, names_to_modify[i], inform);
  }

  // Now that some zones have been updated it's worth running a
  // rebalance pass on the zone graph so movement between zones does
  // not feel too abrupt. Hopefully this yields a fairly natural
  // result...
  balance_weather(names_to_modify);

  if (flag)
    save_weather();
}

// ***********************************************************************
// Queries used by outside rooms/locations, look, shadows, etc.
// ***********************************************************************

// Percentage light level in a room. 100 == full daylight, 0 == pitch dark.
int query_darkness(object room)
{
  int amount;
  string zone;

  zone = table(WEATHER_TABLE)->query_zone(file_name(room));

  if (query_day())
  {
    amount = 100;
    // Rain and wind cut visibility.
    amount -= zones[zone][0] / 4;
    amount -= zones[zone][1] / 5;
    // First and last two hours of daylight are dimmer.
    if ((hour_of_day > query_dawn_time()) &&
        (hour_of_day < query_dawn_time() + 2))
      amount -= 10;
    if ((hour_of_day < query_nightfall_time()) &&
        (hour_of_day > query_nightfall_time() - 2))
      amount -= 10;
  }
  else
  {
    amount = 25;
    // The single moon adds a little light near full phase.
    // Phase 0 == full; 4/5/6 are hidden (new moon window).
    if (moon == 0) // full
      amount += 10;
    else if (((moon >= 1) && (moon <= 3)) ||
             ((moon >= 7) && (moon <= 9)))
      amount += 5;

    /* Moon phase reference:
     * ({ "full",
     *    "waning three quarters",
     *    "waning half",
     *    "waning crescent",
     *    "waning crescent",
     *    "new",
     *    "waxing crescent",
     *    "waxing crescent",
     *    "waxing half",
     *    "waxing three quarters" })
     */
  }

  if (amount < 0)   return 0;
  if (amount > 100) return 100;
  return amount;
}

// Is it currently raining in this room's zone?
int query_raining(object room)
{
  string zone;
  zone = table(WEATHER_TABLE)->query_zone(file_name(room));
  return (zones[zone][0] >= 50);
}

// Raw rain/wind/temperature values for a room's zone.
int * query_actual_data(object room)
{
  string zone;
  zone = table(WEATHER_TABLE)->query_zone(file_name(room));
  return ({ zones[zone][0], zones[zone][1], zones[zone][2], });
}

// Season-adjusted dawn hour.
int query_dawn_time()
{
  switch (season)
  {
    case 0: return 9;  // winter
    case 1: return 8;  // spring
    case 2: return 7;  // summer
    default:
    case 3: return 8;  // autumn
  }
}

// Season-adjusted nightfall hour.
int query_nightfall_time()
{
  switch (season)
  {
    case 0: return 20; // winter
    case 1: return 21; // spring
    case 2: return 22; // summer
    default:
    case 3: return 21; // autumn
  }
}

// 1 if it is currently daytime, 0 otherwise.
int query_day()
{
  if ((hour_of_day >= query_dawn_time()) && (hour_of_day < query_nightfall_time()))
    return 1;
  else
    return 0;
}

// Temperature bucket text. Values map to the zone's cold value.
// [0..9]    scorching heat
// [10..19]  much heat
// [20..29]  hot
// [30..39]  warm
// [40..59]  pleasant
// [60..69]  cool
// [70..79]  cold
// [80..89]  very cold
// [>=90]    polar cold
string temperature_string(string zone)
{
  int amount;
  string ret;

  amount = zones[zone][2];
  ret = "";

  switch (amount)
  {
    case 0..9:    ret = _LANG_WEATHER_TEMP_SCORCHING; break;
    case 10..19:  ret = _LANG_WEATHER_TEMP_VERY_HOT;  break;
    case 20..29:  ret = _LANG_WEATHER_TEMP_HOT;       break;
    case 30..39:  ret = _LANG_WEATHER_TEMP_WARM;      break;
    case 40..59:  ret = _LANG_WEATHER_TEMP_PLEASANT;  break;
    case 60..69:  ret = _LANG_WEATHER_TEMP_COOL;      break;
    case 70..79:  ret = _LANG_WEATHER_TEMP_COLD;      break;
    case 80..89:  ret = _LANG_WEATHER_TEMP_VERY_COLD; break;
    case 90..100: ret = _LANG_WEATHER_TEMP_POLAR;     break;
  }
  return ret;
}

// Wind bucket text.
// [0..59]   nothing
// [60..75]  strong wind
// [76..90]  very strong wind
// [91..98]  blizzard
// [>=99]    hurricane
string wind_string(string zone)
{
  int amount;
  string ret;

  amount = zones[zone][1];
  ret = "";

  switch (amount)
  {
    case 0..59:   ret = "";                             break;
    case 60..75:  ret = _LANG_WEATHER_WIND_STRONG;      break;
    case 76..90:  ret = _LANG_WEATHER_WIND_VERY_STRONG; break;
    case 91..98:  ret = _LANG_WEATHER_WIND_BLIZZARD;    break;
    case 99..100: ret = _LANG_WEATHER_WIND_HURRICANE;   break;
  }
  return ret;
}

// Rain bucket text. If it's cold enough rain turns into hail (>80) or
// snow (>85).
// [0..49]   nothing
// [50..59]  light rain
// [60..69]  rain
// [70..79]  storm
// [80..89]  strong storm
// [>=90]    heavy storm
string rain_string(string zone)
{
  int amount;
  string ret;
  int mod;

  amount = zones[zone][0];
  ret = "";
  mod = 0;

  if (zones[zone][2] > 80) mod = 1;
  if (zones[zone][2] > 85) mod = 2;

  switch (amount)
  {
    case 0..49:   ret = "";                                    break;
    case 50..59:  ret = _LANG_WEATHER_RAIN_LIGHT;              break;
    case 60..69:  ret = _LANG_WEATHER_RAIN_NORMAL;             break;
    case 70..79:  ret = _LANG_WEATHER_RAIN_STORM[mod];         break;
    case 80..89:  ret = _LANG_WEATHER_RAIN_STRONG_STORM[mod];  break;
    case 90..100: ret = _LANG_WEATHER_RAIN_HEAVY_STORM[mod];   break;
    default:      ret = "";                                    break;
  }
  return ret;
}

string season_string()
{
  string * names;
  names = _LANG_WEATHER_SEASONS;
  return names[season];
}

string daynight_string()
{
  string * names;
  names = _LANG_WEATHER_DAYNIGHT;
  return names[query_day() ? 0 : 1];
}

string weather_string(object room)
{
  string ret;
  string zone;
  string temp, wind, rain;

  ret = capitalize(daynight_string()) + _LANG_WEATHER_STRING_OF + season_string();
  zone = table(WEATHER_TABLE)->query_zone(file_name(room));

  temp = temperature_string(zone);
  wind = wind_string(zone);
  rain = rain_string(zone);

  ret += _LANG_WEATHER_STRING_WITH + temp;
  if (wind != "") ret += _LANG_WEATHER_STRING_SEP + wind;
  if (rain != "") ret += _LANG_WEATHER_STRING_AND + rain;
  ret += _LANG_WEATHER_STRING_END;

  return ret;
}

private string query_aux_moon_string(int value)
{
  string * names;
  names = _LANG_WEATHER_MOON_PHASES;
  return names[value];
}

// Text description of the moon's current phase. Games that want
// multiple moons override this in their subclass.
string moon_string()
{
  string phase;
  phase = query_aux_moon_string(moon);
  return _LANG_WEATHER_MOON_STRING;
}


// ***********************************************************************
// Debugging.
void query_all_data()
{
  tell_object(this_player(), "Hour of day: " + hour_of_day + "\n" +
                             "Day of week: " + day_of_week + "\n" +
                             "Day of month: " + day_of_month + "\n" +
                             "Day of year: " + day_of_year + "\n" +
                             "Month: " + month + "\n" +
                             "Season: " + season + "\n" +
                             "Year: " + year + "\n" +
                             "Moon: " + moon + "\n");
}

mapping query_zones() { return zones; }
