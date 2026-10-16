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
 * prune() drops the entries of characters that no longer exist: a player whose
 * save is gone, an NPC whose template is gone, is no longer a named one or no
 * longer has that name, a source .c file that is gone, and the old "name:"
 * entries nobody (player or named template) answers to. The ranking command
 * runs it at most once every RANKING_PRUNE_EVERY seconds; 'ranking prune'
 * forces it.
 *
 * From the CcMud ranking (obj/handlers/ranking.c), neverbot 06/2010.
 */

#include <mud/config.h>
#include <user/player.h>

inherit "/lib/core/object.c";

#define RANKING_SAVE "/save/ranking"
// how many different levels of each group are kept
#define RANKING_LEVELS 11
// seconds between two prunes asked for by the ranking command
#define RANKING_PRUNE_EVERY 300

// group path -> ([ level : ([ key : name ]) ]). The key says who it is (see
// _identity_key), so two with the same name do not collide.
mapping ranking;

// when the last prune ran
private int last_prune;

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

// The lowercase names a template gives its NPC (one, or one per gender), only
// when it is a named one; an empty list otherwise.
private string * _template_names(mapping t)
{
  mixed name;

  if (!mappingp(t) || (!t["proper_name"] && !t["unique"]))
    return ({ });

  name = t["name"];
  if (stringp(name))
    return ({ lower_case(name) });
  if (mappingp(name))
  {
    string * names;
    mixed * values;
    int i;

    names = ({ });
    values = map_values(name);
    for (i = 0; i < sizeof(values); i++)
      if (stringp(values[i]))
        names += ({ lower_case(values[i]) });
    return names;
  }

  return ({ });
}

// Every lowercase name given by the named templates of a game, found by
// walking its tree. Costly, so it is only asked for when a "name:" entry
// needs it.
private mapping _game_template_names(string game)
{
  mapping found;
  string * queue, * names, suffix, dir, source;
  mixed * entries;
  int i, j, len;

  found = ([ ]);
  suffix = "." + mud_language() + ".json";
  len = strlen(suffix);
  queue = ({ "/games/" + game + "/" });

  while (sizeof(queue))
  {
    dir = queue[0];
    queue = queue[1 ..];

    entries = get_dir(dir + "*", -1);
    for (i = 0; i < sizeof(entries); i++)
    {
      if (entries[i][1] == -2)
      {
        queue += ({ dir + entries[i][0] + "/" });
        continue;
      }

      if (strlen(entries[i][0]) <= len ||
          entries[i][0][strlen(entries[i][0]) - len ..] != suffix)
        continue;

      source = dir + entries[i][0][0 .. strlen(entries[i][0]) - len - 1];
      names = _template_names(("/lib/handlers/bestiary")->query_template(game, source));
      for (j = 0; j < sizeof(names); j++)
        found[names[j]] = 1;
    }
  }

  return found;
}

// Whether the NPC ranked under this source and displayed name still exists:
// by its template when it has one (still a named one with that name), else by
// its source file.
private int _npc_exists(string game, string source, string name)
{
  mapping t;

  t = ("/lib/handlers/bestiary")->query_template(game, source);
  if (t)
    return member_array(lower_case(name), _template_names(t)) != -1;

  // no template: a legacy source file is enough while it is there
  return strlen(source) > 2 && source[strlen(source) - 2 ..] == ".c" &&
         file_size(source) >= 0;
}

// Take out of every group the entries of characters that no longer exist (see
// the header). Unless forced, it does nothing when it ran less than
// RANKING_PRUNE_EVERY seconds ago. Saves if anything went; returns how many
// entries were removed.
int prune(varargs int force)
{
  mapping levels, game_names, cache;
  string * groups, * entries;
  string game, key, name, source, who;
  int * keys;
  int i, j, k, removed, keep;

  if (!force && last_prune && time() - last_prune < RANKING_PRUNE_EVERY)
    return 0;
  last_prune = time();

  game_names = ([ ]);
  cache = ([ ]);
  groups = map_indices(ranking);
  for (i = 0; i < sizeof(groups); i++)
  {
    if (sscanf(groups[i], "/games/%s/%*s", game) != 2)
      continue;

    levels = ranking[groups[i]];
    keys = map_indices(levels);
    for (j = 0; j < sizeof(keys); j++)
    {
      entries = map_indices(levels[keys[j]]);
      for (k = 0; k < sizeof(entries); k++)
      {
        key = entries[k];
        name = levels[keys[j]][key];

        // one verdict per game, key and name
        who = game + "|" + key + "|" + name;
        if (cache[who] == nil)
        {
          if (sscanf(key, "player:%s", source))
            keep = player_exists(source);
          else if (sscanf(key, "npc:%s", source))
            keep = _npc_exists(game, source, name);
          else if (sscanf(key, "name:%s", source))
          {
            if (!game_names[game])
              game_names[game] = _game_template_names(game);
            keep = player_exists(lower_case(source)) ||
                   game_names[game][lower_case(source)];
          }
          else
            keep = 1;
          cache[who] = keep ? 1 : 0;
        }

        if (!cache[who])
        {
          map_delete(levels[keys[j]], key);
          removed++;
        }
      }

      if (!map_sizeof(levels[keys[j]]))
        map_delete(levels, keys[j]);
    }

    if (!map_sizeof(levels))
      map_delete(ranking, groups[i]);
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
  return ::stats() + ({ ({ "Groups", map_sizeof(ranking) }) });
}
