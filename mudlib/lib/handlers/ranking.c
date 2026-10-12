/*
 * Ranking handler: the highest levels reached in every class, guild and job,
 * by players and by the NPCs that are one particular person alike, so anybody
 * can compare themselves with the rest of their group.
 *
 * For each group (the path of its class, guild or job object) it keeps the
 * names at each level, and only the RANKING_LEVELS highest levels: whoever
 * falls below the lowest one kept drops out. Coders are left out.
 *
 * Updated when a level changes and when a player logs in (RANKING_OB->update);
 * shown by the ranking command.
 *
 * From the CcMud ranking (obj/handlers/ranking.c), neverbot 06/2010.
 */

#include <mud/config.h>

inherit "/lib/core/object.c";

#define RANKING_SAVE "/save/ranking"
// how many different levels of each group are kept
#define RANKING_LEVELS 11

// group path -> ([ level : ({ names }) ])
mapping ranking;

void create()
{
  ranking = ([ ]);
  ::create();
  restore_object(RANKING_SAVE, 1);
  if (!mappingp(ranking))
    ranking = ([ ]);
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

// Take a name off every level of a group. Whether it was there.
private int _remove_from_group(mapping levels, string name)
{
  int * keys;
  int i, found;

  keys = map_indices(levels);
  for (i = 0; i < sizeof(keys); i++)
    if (member_array(name, levels[keys[i]]) != -1)
    {
      levels[keys[i]] -= ({ name });
      if (!sizeof(levels[keys[i]]))
        map_delete(levels, keys[i]);
      found = 1;
    }

  return found;
}

// Put a name at its level of a group, keeping only the highest levels. Whether
// the group changed.
private int _place(string group, string name, int level)
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
  if (levels[level] && member_array(name, levels[level]) != -1)
    return 0;

  // off the level it held before, if any
  changed = _remove_from_group(levels, name);

  // a new level lower than all the ones kept, when they are all taken, is not
  // kept either
  keys = _levels_ascending(levels);
  if (!levels[level] && sizeof(keys) >= RANKING_LEVELS && level < keys[0])
    return changed;

  levels[level] = (levels[level] ? levels[level] : ({ })) + ({ name });

  // the lowest levels go once there are more than RANKING_LEVELS of them
  keys = _levels_ascending(levels);
  while (sizeof(keys) > RANKING_LEVELS)
  {
    map_delete(levels, keys[0]);
    keys = keys[1 ..];
  }

  return 1;
}

// Weigh somebody against the groups they belong to: their class, their guild
// and their job, each with its own level.
void update(object who)
{
  string name;
  int changed;

  if (!who || who->query_coder())
    return;

  name = (string)who->query_cap_name();
  if (!name || !strlen(name))
    return;

  changed = _place(_class_group(who), name,
                   (int)who->query_class_level());
  changed += _place(_group_path(who->query_guild_ob()), name,
                    (int)who->query_guild_level());
  changed += _place(_group_path(who->query_job_ob()), name,
                    (int)who->query_job_level());

  if (changed)
    save_handler();
}

// The ranking of one group, as level -> names, or nil when nobody is in it.
mapping query_group_ranking(string group)
{
  mapping levels;

  levels = ranking[_group_path(group)];
  return levels ? ([ ]) + levels : nil;
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
    found += _remove_from_group(ranking[groups[i]], name);

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
