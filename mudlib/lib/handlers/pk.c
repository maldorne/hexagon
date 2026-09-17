// Radix 1996
// Site and player data on PKs
// Basically break down killers by who they're killing (guilds, groups...)
// Version 1.0
//
// Rewritten for Hexagon, neverbot 09/2026: both sides of every player kill
// are kept (who killed whom, and who died to whom), victims are broken down
// by race, guild and citizenship with their average level, and the last
// kills are listed one by one. The records are mud-wide, since a name is
// unique across every game, and each kill notes the game it happened in.

#include <mud/secure.h>

#define PK_SAVE "/save/pk"
#define DEATH_HANDLER "/lib/handlers/death"
// how many of the last kills and deaths are kept for each character
#define PK_RECENT 20

// killer name -> ([
//   "first" : time of the first kill,
//   "last"  : time of the last kill,
//   "kills" : number of players killed,
//   "sites" : ({ addresses the killer played from }),
//   "games", "races", "guilds", "citizenships" :
//             ([ key : ({ kills, sum of levels }) ]),
//   "recent" : ({ ({ victim, time, victim level, game }) }), newest last
// ])
mapping killers;

// victim name -> ([
//   "deaths" : times killed by a player,
//   "last"   : time of the last one,
//   "by"     : ([ killer name : times ]),
//   "recent" : ({ ({ killer, time, killer level, game }) }), newest last
// ])
mapping victims;

void create()
{
  seteuid(ROOT);
  killers = ([ ]);
  victims = ([ ]);
  restore_object(PK_SAVE, 1);
}

private void save_me()
{
  save_object(PK_SAVE, 1);
}

// Add one kill to a ({ kills, sum of levels }) breakdown.
private mapping count_in(mapping breakdown, string key, int level)
{
  if (!strlen(key))
    key = "none";

  if (!breakdown[key])
    breakdown[key] = ({ 0, 0 });

  breakdown[key][0]++;
  breakdown[key][1] += level;
  return breakdown;
}

private mixed * keep_recent(mixed * list, mixed * entry)
{
  list += ({ entry });

  if (sizeof(list) > PK_RECENT)
    list = list[sizeof(list) - PK_RECENT..];

  return list;
}

// Called from the death handler when a player kills another player.
void update_player_killed(object victim, object killer)
{
  string killer_name, victim_name, site, game;
  mapping entry;

  if (base_name(previous_object()) != DEATH_HANDLER)
    return;

  if (!victim || !killer || victim == killer)
    return;

  // fights with or between coders are not player killing
  if (victim->query_coder() || killer->query_coder())
    return;

  killer_name = killer->query_name();
  victim_name = victim->query_name();
  game = environment(victim) ? game_name(environment(victim)) : nil;
  if (!strlen(game))
    game = "none";

  entry = killers[killer_name];

  if (!entry)
    entry = ([ "first" : time(), "kills" : 0, "sites" : ({ }), "games" : ([ ]), "races" : ([ ]),
               "guilds" : ([ ]), "citizenships" : ([ ]), "recent" : ({ }) ]);

  entry["last"] = time();
  entry["kills"]++;

  if (killer->user() && (site = query_ip_number(killer->user())) &&
      member_array(site, entry["sites"]) == -1)
    entry["sites"] += ({ site });

  entry["games"] = count_in(entry["games"], game, victim->query_level());
  entry["races"] = count_in(entry["races"], victim->query_race_ob(), victim->query_level());
  entry["guilds"] = count_in(entry["guilds"], victim->query_guild_ob(), victim->query_level());
  entry["citizenships"] = count_in(entry["citizenships"], victim->query_citizenship(),
                                   victim->query_level());
  entry["recent"] = keep_recent(entry["recent"], ({ victim_name, time(), victim->query_level(), game }));

  killers[killer_name] = entry;

  entry = victims[victim_name];

  if (!entry)
    entry = ([ "deaths" : 0, "by" : ([ ]), "recent" : ({ }) ]);

  entry["deaths"]++;
  entry["last"] = time();
  entry["by"][killer_name] = (entry["by"][killer_name] ? entry["by"][killer_name] : 0) + 1;
  entry["recent"] = keep_recent(entry["recent"], ({ killer_name, time(), killer->query_level(), game }));

  victims[victim_name] = entry;

  save_me();
}

// A character that stops existing takes its records with it.
void retire_user(string name)
{
  if (!this_user() || (this_user()->player() &&
      this_user()->player()->query_name() != name && !this_user()->query_admin()))
    return;

  map_delete(killers, name);
  map_delete(victims, name);
  save_me();
}

mapping query_killers() { return ([ ]) + killers; }
mapping query_victims() { return ([ ]) + victims; }

mapping query_killer(string name) { return killers[name] ? ([ ]) + killers[name] : nil; }
mapping query_victim(string name) { return victims[name] ? ([ ]) + victims[name] : nil; }

int total_pk_no(string name)
{
  return killers[name] ? killers[name]["kills"] : 0;
}

// Average level of the players this one has killed.
int avg_pk_level(string name)
{
  mapping races;
  mixed * counts;
  int i, kills, levels;

  if (!killers[name])
    return 0;

  races = killers[name]["races"];
  counts = map_values(races);

  for (i = 0; i < sizeof(counts); i++)
  {
    kills += counts[i][0];
    levels += counts[i][1];
  }

  return kills ? levels / kills : 0;
}

int last_pk(string name)
{
  return killers[name] ? killers[name]["last"] : 0;
}
