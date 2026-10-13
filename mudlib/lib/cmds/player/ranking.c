/*
 * ranking command: the highest levels reached in the groups you belong to
 * (your class, your guild, your job), players and NPCs alike, from the ranking
 * handler (/lib/handlers/ranking.c). 'ranking all' shows every group of your
 * game (coders: of every game) and coders may take a name off them.
 *
 * From the CcMud ranking command, neverbot 06/2010.
 */

#include <mud/cmd.h>
#include <living/living.h>
#include <language.h>

inherit CMD_BASE;

void setup()
{
  set_aliases(_LANG_CMD_RANKING_ALIAS);
  set_usage(_LANG_CMD_RANKING_SYNTAX);
  set_help(_LANG_CMD_RANKING_HELP);
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

// What a group is, as a title prefix: a class, a guild or a job.
private string _group_kind(string group)
{
  if (strsrch(group, "/obj/classes/") != -1)
    return _LANG_RANKING_KIND_CLASS + ": ";
  if (strsrch(group, "/obj/guilds/") != -1)
    return _LANG_RANKING_KIND_GUILD + ": ";
  if (strsrch(group, "/obj/jobs/") != -1)
    return _LANG_RANKING_KIND_JOB + ": ";
  return "";
}

// One group's ranking, highest level first.
private string _group_text(object handler, string group)
{
  mapping levels;
  object group_ob;
  int * keys;
  string ret;
  int i;

  levels = handler->query_group_ranking(group);
  if (!levels || !map_sizeof(levels))
    return "";

  group_ob = nil;
  catch(group_ob = load_object(group));
  ret = "  " + _group_kind(group) +
        (group_ob ? capitalize((string)group_ob->query_short()) : group) + "\n";

  keys = _levels_ascending(levels);
  for (i = sizeof(keys) - 1; i >= 0; i--)
    ret += sprintf("    " + _LANG_RANKING_LEVEL + " %2d: %s\n", keys[i],
                   query_multiple_short(levels[keys[i]]));

  return ret + "\n";
}

static int cmd(string str, object me, string verb)
{
  object handler;
  string * groups;
  string ret, name, prefix, game, last_game, text;
  int i, all;

  handler = load_object(RANKING_OB);
  if (!handler)
  {
    notify_fail(_LANG_RANKING_BROKEN);
    return 0;
  }

  if (str && me->query_coder() && sscanf(str, _LANG_RANKING_REMOVE + " %s", name) == 1)
  {
    write(handler->remove_name(capitalize(name)) ? _LANG_RANKING_REMOVED :
                                                   _LANG_RANKING_NOT_FOUND);
    return 1;
  }

  all = (str == _LANG_RANKING_ALL);
  prefix = "/games/" + game_name(me) + "/";
  if (all)
  {
    // coders see every game, the rest their own
    groups = sort_array(handler->query_groups());
    if (!me->query_coder())
      for (i = sizeof(groups) - 1; i >= 0; i--)
        if (strsrch(groups[i], prefix) != 0)
          groups -= ({ groups[i] });
  }
  else
  {
    // never the groups of another game
    groups = ({ me->query_class_ob(), me->query_guild_ob(), me->query_job_ob() }) -
             ({ nil, "" });
    for (i = sizeof(groups) - 1; i >= 0; i--)
      if (strsrch(groups[i], "/games/") == 0 && strsrch(groups[i], prefix) != 0)
        groups -= ({ groups[i] });
  }

  ret = "";
  for (i = 0; i < sizeof(groups); i++)
  {
    text = _group_text(handler, groups[i]);

    // a header for each game's groups
    if (all && me->query_coder() && strlen(text) && sscanf(groups[i], "/games/%s/%*s", game) == 2 &&
        game != last_game)
    {
      ret += game + "\n";
      last_game = game;
    }
    ret += text;
  }

  if (!strlen(ret))
    ret = all ? _LANG_RANKING_ALL_EMPTY : _LANG_RANKING_EMPTY;
  else
    ret = (all ? _LANG_RANKING_ALL_HEADER : _LANG_RANKING_HEADER) + ret;

  if (me->query_coder())
    ret += wrap(_LANG_RANKING_CODER_HINTS,
                this_user() ? this_user()->query_cols() : 80);

  write(ret);
  return 1;
}
