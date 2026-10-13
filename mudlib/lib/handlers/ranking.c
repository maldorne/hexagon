/*
 * Ranking handler: the highest levels reached in every class, guild and job,
 * by players and by the NPCs that are one particular person alike, so anybody
 * can compare themselves with the rest of their group.
 *
 * For each group (the path of its class, guild or job object) it keeps the
 * names at each level (each under a key for who it is), and only the
 * RANKING_LEVELS highest levels: whoever falls below the lowest one kept drops
 * out. Coders are left out.
 *
 * Updated when a level changes and when a player logs in (RANKING_OB->update);
 * shown by the ranking command.
 *
 * From the CcMud ranking (obj/handlers/ranking.c), neverbot 06/2010.
 */

#include <mud/config.h>
#include <user/player.h>

inherit "/lib/core/object.c";

#define RANKING_SAVE "/save/ranking"
// how many different levels of each group are kept
#define RANKING_LEVELS 11

// group path -> ([ level : ([ key : name ]) ]). The key says who it is (see
// _identity_key), so two with the same name do not collide.
mapping ranking;

// Names that were ranked by mistake before entries had a key, dropped when the
// old format is read.
private string * _stale_names()
{
  return ({ "Jabalí", "Lobo gris", "Pájaro", "Panadero", "Guardián",
            "Centinela", "Guardia", "Zorro", "Ciervo", "Viajera perdida",
            "Carcelero", "Guardián del tesoro", "Oso", "Bibliotecaria",
            "Banquero", "Tabernero", "Herrero", "Granjera", "Tendero",
            "Tendera" });
}

// Read the old format (level -> ({ names }) per group): the stale names and
// the groups outside any game (the mudlib's generic ones) go, the rest are kept under a "name:" key
// that the real one replaces when that character is next placed. Whether
// anything changed.
private int _migrate()
{
  string * groups, * names;
  mapping levels, kept;
  int * keys;
  int i, j, k, changed;

  groups = map_indices(ranking);
  for (i = 0; i < sizeof(groups); i++)
  {
    levels = ranking[groups[i]];

    // only the groups of a game are kept
    if (groups[i][0 .. 6] != "/games/")
    {
      map_delete(ranking, groups[i]);
      changed = 1;
      continue;
    }

    if (!mappingp(levels))
    {
      map_delete(ranking, groups[i]);
      changed = 1;
      continue;
    }

    keys = map_indices(levels);
    if (!sizeof(keys) || !pointerp(levels[keys[0]]))
      continue;

    changed = 1;
    kept = ([ ]);
    for (j = 0; j < sizeof(keys); j++)
    {
      names = levels[keys[j]] - _stale_names();
      if (!sizeof(names))
        continue;

      kept[keys[j]] = ([ ]);
      for (k = 0; k < sizeof(names); k++)
        kept[keys[j]]["name:" + names[k]] = names[k];
    }

    if (map_sizeof(kept))
      ranking[groups[i]] = kept;
    else
      map_delete(ranking, groups[i]);
  }

  return changed;
}

void create()
{
  ranking = ([ ]);
  ::create();
  restore_object(RANKING_SAVE, 1);
  if (!mappingp(ranking))
    ranking = ([ ]);
  if (_migrate())
    save_object(RANKING_SAVE, 1);
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
  set_name("ranking handler");
  set_short("Ranking handler");
  set_long("This is the ranking handler!\n");
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
  save_object(RANKING_SAVE, 1);
}

// A group object named with or without its extension is the same group.
private string _group_path(string path)
{
  if (!path || !strlen(path))
    return "";
  if (strlen(path) > 2 && path[strlen(path) - 2 ..] == ".c")
    path = path[0 .. strlen(path) - 3];
  return path;
}

// The class somebody is ranked under: a game's own class with the same id
// stands for the mudlib's generic one, so the NPCs of a game built on the
// generic classes rank beside the players, who take the game's.
private string _class_group(object who)
{
  string path, game, file;
  object class_ob;

  path = _group_path(who->query_class_ob());
  if (!strlen(path) || path[0 .. 16] != "/lib/obj/classes/")
    return path;

  game = game_name(who);
  class_ob = load_object(path);
  if (!game || !strlen(game) || !class_ob)
    return path;

  file = handler("games")->query_class_file(game, class_ob->query_class_id());
  return strlen(file) ? _group_path(file) : path;
}

// The levels of a group, lowest first.
private int * _levels_ascending(mapping levels)
{
  int * keys;
  int i, j, swap;

  keys = map_indices(levels);
  for (i = 1; i < sizeof(keys); i++)
    for (j = i; j > 0 && keys[j - 1] > keys[j]; j--)
    {
      swap = keys[j];
      keys[j] = keys[j - 1];
      keys[j - 1] = swap;
    }

  return keys;
}

// Take every entry with this key (or this displayed name) off a group's
// levels. Whether any was there.
private int _remove_entry(mapping levels, string key, string name)
{
  int * keys;
  string * entries;
  int i, j, found;

  keys = map_indices(levels);
  for (i = 0; i < sizeof(keys); i++)
  {
    entries = map_indices(levels[keys[i]]);
    for (j = 0; j < sizeof(entries); j++)
      if (entries[j] == key || (name && levels[keys[i]][entries[j]] == name))
      {
        map_delete(levels[keys[i]], entries[j]);
        found = 1;
      }

    if (!map_sizeof(levels[keys[i]]))
      map_delete(levels, keys[i]);
  }

  return found;
}

// Put somebody at their level of a group, keeping only the highest levels.
// Whether the group changed.
private int _place(string group, string key, string name, int level)
{
  mapping levels;
  int * keys;
  int changed;

  if (!strlen(group) || level <= 0)
    return 0;

  levels = ranking[group];
  if (!levels)
  {
    levels = ([ ]);
    ranking[group] = levels;
  }

  // already there, at this level
  if (levels[level] && levels[level][key] == name)
    return 0;

  // off the level it held before, and off the entry that only knew its name
  changed = _remove_entry(levels, key, nil);
  changed += _remove_entry(levels, "name:" + name, nil);

  // a new level lower than all the ones kept, when they are all taken, is not
  // kept either
  keys = _levels_ascending(levels);
  if (!levels[level] && sizeof(keys) >= RANKING_LEVELS && level < keys[0])
    return changed;

  if (!levels[level])
    levels[level] = ([ ]);
  levels[level][key] = name;

  // the lowest levels go once there are more than RANKING_LEVELS of them
  keys = _levels_ascending(levels);
  while (sizeof(keys) > RANKING_LEVELS)
  {
    map_delete(levels, keys[0]);
    keys = keys[1 ..];
  }

  return 1;
}

// What tells one ranked character from another: a player by name, an NPC by
// the template or file it comes from.
private string _identity_key(object who)
{
  string source;

  if (who->query_player())
    return "player:" + lower_case((string)who->query_name());

  source = (string)who->query_npc_source();
  if (!source || !strlen(source))
    source = base_name(who);

  return "npc:" + source;
}

// Weigh somebody against the groups they belong to: their class, their guild
// and their job, each with its own level.
void update(object who)
{
  string name, key;
  int changed;

  if (!who || who->query_coder())
    return;

  // players always; NPCs only when they are somebody
  if (who->query_player())
  {
    if (who->query_property(GUEST_PROP))
      return;
  }
  else if (!who->has_proper_name() && !who->query_only_one())
    return;

  name = (string)who->query_cap_name();
  if (!name || !strlen(name))
    return;

  key = _identity_key(who);

  changed = _place(_class_group(who), key, name,
                   (int)who->query_class_level());
  changed += _place(_group_path(who->query_guild_ob()), key, name,
                    (int)who->query_guild_level());
  changed += _place(_group_path(who->query_job_ob()), key, name,
                    (int)who->query_job_level());

  if (changed)
    save_handler();
}

// The ranking of one group, as level -> names, or nil when nobody is in it.
mapping query_group_ranking(string group)
{
  mapping levels, ret;
  int * keys;
  int i;

  levels = ranking[_group_path(group)];
  if (!levels)
    return nil;

  ret = ([ ]);
  keys = map_indices(levels);
  for (i = 0; i < sizeof(keys); i++)
    ret[keys[i]] = sort_array(map_values(levels[keys[i]]));

  return ret;
}

string * query_groups() { return map_indices(ranking); }

// Take a name off every group, for a character that is gone. Whether it was in
// any.
int remove_name(string name)
{
  string * groups;
  int i, found;

  groups = map_indices(ranking);
  for (i = 0; i < sizeof(groups); i++)
    found += _remove_entry(ranking[groups[i]], nil, name);

  if (found)
    save_handler();
  return found;
}

void dest_me()
{
  save_handler();
  ::dest_me();
}

mixed * stats()
{
  return ::stats() + ({ ({ "Groups", map_sizeof(ranking) }) });
}
