/*
 * ranking command: the highest levels reached in the groups you belong to
 * (your class, your guild, your job), players and NPCs alike, from the ranking
 * handler (/lib/handlers/ranking.c). Coders may see every group and take a
 * name off them.
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
  ret = "  " + (group_ob ? capitalize((string)group_ob->query_short()) : group) + "\n";

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
  string ret, name;
  int i;

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

  if (str == _LANG_RANKING_ALL && me->query_coder())
    groups = sort_array(handler->query_groups());
  else
    groups = ({ me->query_class_ob(), me->query_guild_ob(), me->query_job_ob() }) -
             ({ nil, "" });

  ret = "";
  for (i = 0; i < sizeof(groups); i++)
    ret += _group_text(handler, groups[i]);

  if (!strlen(ret))
    ret = _LANG_RANKING_EMPTY;
  else
    ret = _LANG_RANKING_HEADER + ret;

  if (me->query_coder())
    ret += wrap(_LANG_RANKING_CODER_HINTS,
                this_user() ? this_user()->query_cols() : 80);

  write(ret);
  return 1;
}
