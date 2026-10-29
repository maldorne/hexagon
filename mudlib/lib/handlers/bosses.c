/*
 * Bosses handler: the record of the NPCs that carry the boss component
 * (/lib/npc/components/boss.c), kept across reboots so players can see who
 * has beaten them and how.
 *
 * A boss is known by the template (or file) it comes from, so the record
 * survives it being respawned. It is registered the first time it is placed
 * in the world, so the list is complete before anybody has fought it. For each
 * one it keeps how many times it has died, who struck the killing blow and
 * who fought it the first and the last time, the groups of the last
 * BOSSES_RECENT deaths, how many players it has killed and the shortest fight
 * in which it was beaten.
 *
 * Coders of every kind, test characters (BOSSES_TEST_NAMES) and guests
 * are never recorded: a fight with nobody else in it leaves no trace.
 *
 * Shown by the bosses command; prune() drops the bosses whose template is gone.
 */

#include <mud/config.h>
#include <user/player.h>

inherit "/lib/core/object.c";

#define BOSSES_SAVE "/save/bosses"
// how many deaths keep their group of fighters
#define BOSSES_RECENT 20
// test characters, never recorded (testplay, a plain player, is not one)
#define BOSSES_TEST_NAMES ({ "test", "testaux" })

// boss key (its template path, or its file) -> ([
//   "name"     : displayed name,
//   "game"     : game it belongs to,
//   "deaths"   : times it has died,
//   "killed"   : players it has killed,
//   "first"    : ([ "killer", "group", "time", "seconds" ]) of its first death,
//   "last"     : same, of the latest one,
//   "recent"   : the latest deaths, oldest first, each as "first",
//   "fastest"  : ([ "seconds", "group", "killer" ]) of the shortest fight won ])
mapping bosses;

void create()
{
  bosses = ([ ]);
  ::create();
  restore_object(BOSSES_SAVE, 1);
  if (!mappingp(bosses))
    bosses = ([ ]);
}

void setup()
{
  string name;
  int cnum;

  // Anticloning: only the master instance should exist.
  if (sscanf(file_name(this_object()), "%s#%d", name, cnum) == 2)
  {
    write("This object cannot be cloned.\n");
    dest_me();
    return;
  }

  reset_get();
  set_name("bosses handler");
  set_short("Bosses handler");
  set_long("This is the bosses handler!\n");
  add_alias("handler");
  add_plural("handlers");

  move("bing");
}

int move(mixed dest, varargs mixed messin, mixed messout)
{
  ::move(HANDLERS_HOME, messin, messout);
}

void save_handler()
{
  save_object(BOSSES_SAVE, 1);
}

/** Whether somebody is left out of the records: any kind of coder, a guest or a test character. */
private int _is_left_out(object who)
{
  return who->query_coder() || who->query_property(GUEST_PROP) ||
         member_array(lower_case((string)who->query_name()), BOSSES_TEST_NAMES) != -1;
}

/**
 * The key a boss is recorded under: its game and its template path (or its
 * file), so two games cannot clash.
 */
string query_boss_key(object npc)
{
  string source, game;

  source = npc->query_npc_source();
  if (!stringp(source) || !strlen(source))
    source = base_name(npc);

  game = npc->query_npc_game();
  if (!stringp(game) || !strlen(game))
    game = game_name(npc);
  return (stringp(game) && strlen(game)) ? game + ":" + source : source;
}

private mapping _new_entry(string name, string game)
{
  return ([ "name": name, "game": game, "deaths": 0, "killed": 0,
            "first": nil, "last": nil, "recent": ({ }), "fastest": nil ]);
}

/**
 * Note a boss as known: a first record, or its displayed name brought up to
 * date. Called when the boss is placed in the world.
 */
void register_boss(object npc)
{
  string key, name, game;
  int changed;

  if (!npc)
    return;

  key = query_boss_key(npc);
  name = (string)npc->query_cap_name();
  if (!name || !strlen(name))
    return;

  if (!bosses[key])
  {
    bosses[key] = _new_entry(name, sscanf(key, "%s:%*s", game) == 2 ? game : "");
    changed = 1;
  }
  else if (bosses[key]["name"] != name)
  {
    bosses[key]["name"] = name;
    changed = 1;
  }

  if (changed)
    save_handler();
}

/**
 * Record the death of a boss.
 *
 * @param npc the boss
 * @param killer who struck the last blow
 * @param fighters the living things that were fighting it
 * @param seconds how long the fight lasted
 */
void record_death(object npc, object killer, object * fighters, int seconds)
{
  mapping entry, death;
  string key, * group;
  int i;

  if (!npc)
    return;

  register_boss(npc);
  key = query_boss_key(npc);
  entry = bosses[key];
  if (!entry)
    return;

  // only players take part in the record, and the ones left out do not count
  group = ({ });
  fighters = (fighters ? fighters : ({ })) - ({ nil });
  if (killer && member_array(killer, fighters) == -1)
    fighters += ({ killer });
  for (i = 0; i < sizeof(fighters); i++)
    if (fighters[i]->query_player() && !_is_left_out(fighters[i]))
      group += ({ (string)fighters[i]->query_cap_name() });

  if (!sizeof(group))
    return;

  death = ([ "killer": (killer && !_is_left_out(killer)) ? (string)killer->query_cap_name() : "",
             "group": sort_array(group), "time": time(),
             "seconds": seconds ]);

  entry["deaths"] += 1;
  if (!entry["first"])
    entry["first"] = death;
  entry["last"] = death;

  entry["recent"] += ({ death });
  while (sizeof(entry["recent"]) > BOSSES_RECENT)
    entry["recent"] = entry["recent"][1 ..];

  if (!entry["fastest"] || seconds < entry["fastest"]["seconds"])
    entry["fastest"] = death;

  save_handler();
}

/** Record that a boss has killed a player. */
void record_kill(object npc, object victim)
{
  string key;

  if (!npc || !victim || !victim->query_player() || _is_left_out(victim))
    return;

  register_boss(npc);
  key = query_boss_key(npc);
  if (!bosses[key])
    return;

  bosses[key]["killed"] += 1;
  save_handler();
}

/** The keys of the bosses of a game, sorted by displayed name. */
string * query_game_bosses(string game)
{
  string * keys, * found;
  string * names;
  int i;

  keys = map_indices(bosses);
  found = ({ });
  for (i = 0; i < sizeof(keys); i++)
    if (bosses[keys[i]]["game"] == game)
      found += ({ keys[i] });

  names = allocate(sizeof(found));
  for (i = 0; i < sizeof(found); i++)
    names[i] = lower_case(bosses[found[i]]["name"]) + "|" + found[i];
  names = sort_array(names);

  for (i = 0; i < sizeof(names); i++)
    found[i] = names[i][strsrch(names[i], "|") + 1 ..];
  return found;
}

/** The record of a boss, nil when it is not known. */
mapping query_boss(string key)
{
  return bosses[key];
}

/** Lowercase, without accents, to compare what a player typed with a name. */
private string _fold(string str)
{
  string from, to;
  int i;

  from = "áéíóúüñ";
  str = lower_case(str);
  // the accents are two bytes each, so they are replaced one letter at a time
  for (i = 0; i < 7; i++)
  {
    to = ({ "a", "e", "i", "o", "u", "u", "n" })[i];
    str = implode(explode(str, from[i * 2 .. i * 2 + 1]), to);
  }
  return str;
}

/**
 * The boss of a game whose name is, or starts with, or contains what was
 * typed (an exact name wins).
 *
 * @return the boss key, "" when none or when several fit
 */
string find_boss(string game, string typed)
{
  string * keys, * partial;
  string wanted, name;
  int i;

  wanted = _fold(typed);
  keys = query_game_bosses(game);
  partial = ({ });
  for (i = 0; i < sizeof(keys); i++)
  {
    name = _fold(bosses[keys[i]]["name"]);
    if (name == wanted)
      return keys[i];
    if (strsrch(name, wanted) != -1)
      partial += ({ keys[i] });
  }

  return sizeof(partial) == 1 ? partial[0] : "";
}

/** Take a boss off the records (a coder's call, to clean up after a test). */
int forget_boss(string key)
{
  if (!bosses[key])
    return 0;
  map_delete(bosses, key);
  save_handler();
  return 1;
}

/**
 * Drop the bosses whose template is gone, or no longer gives the boss
 * component. Bosses outside any game are kept.
 *
 * @return how many were dropped
 */
int prune()
{
  string * keys;
  string game, source;
  mapping t;
  int i, removed;

  keys = map_indices(bosses);
  for (i = 0; i < sizeof(keys); i++)
  {
    game = bosses[keys[i]]["game"];
    if (!strlen(game))
      continue;

    sscanf(keys[i], "%*s:%s", source);
    t = ("/lib/handlers/bestiary")->query_template(game, source);
    if (t ? !(mappingp(t["components"]) && t["components"]["boss"] != nil) :
            file_size("/games/" + game + "/" + source) < 0 &&
            file_size("/games/" + game + "/" + source + ".c") < 0)
    {
      map_delete(bosses, keys[i]);
      removed++;
    }
  }

  if (removed)
    save_handler();
  return removed;
}

void dest_me()
{
  save_handler();
  ::dest_me();
}

mixed * stats()
{
  return ::stats() + ({ ({ "Bosses", map_sizeof(bosses) }) });
}
