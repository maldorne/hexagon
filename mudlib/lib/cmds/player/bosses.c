/*
 * bosses command: the bosses of your game (the NPCs with the boss component),
 * how many times each has been defeated and how many players it has killed,
 * from the bosses handler (/lib/handlers/bosses.c). With a name it shows that
 * boss's record: who defeated it the first and the last time, the shortest
 * fight and the latest groups to take it on. Coders can force a prune with
 * 'bosses prune'.
 */

#include <mud/cmd.h>
#include <language.h>

inherit CMD_BASE;

// how many of the latest deaths a record shows
#define SHOWN_DEATHS 5

void setup()
{
  set_aliases(_LANG_CMD_BOSSES_ALIAS);
  set_usage(_LANG_CMD_BOSSES_SYNTAX);
  set_help(_LANG_CMD_BOSSES_HELP);
}

// A fight's length as minutes and seconds.
private string _duration(int seconds)
{
  return sprintf("%d:%02d", seconds / 60, seconds % 60);
}

// The line of a death: when, who struck the blow, who fought, how long.
private string _death_line(mapping death)
{
  return sprintf(_LANG_BOSSES_DEATH_LINE, ctime(death["time"], 4),
                 strlen(death["killer"]) ? death["killer"] : _LANG_BOSSES_NOBODY,
                 query_multiple_short(death["group"]),
                 _duration(death["seconds"]));
}

// The list of the bosses of a game.
private string _list_text(object bosses_ob, string game)
{
  string * keys;
  mapping boss;
  string ret;
  int i;

  keys = bosses_ob->query_game_bosses(game);
  if (!sizeof(keys))
    return "";

  ret = "";
  for (i = 0; i < sizeof(keys); i++)
  {
    boss = bosses_ob->query_boss(keys[i]);
    ret += sprintf(_LANG_BOSSES_LIST_LINE, boss["name"],
                   _LANG_BOSSES_TIMES(boss["deaths"]),
                   _LANG_BOSSES_PLAYERS(boss["killed"]));
  }

  return ret + "\n" + _LANG_BOSSES_LIST_HINT;
}

// The record of one boss.
private string _record_text(mapping boss)
{
  string ret;
  mapping * recent;
  int i, first;

  ret = sprintf(_LANG_BOSSES_RECORD_TOTALS, _LANG_BOSSES_TIMES(boss["deaths"]),
                _LANG_BOSSES_PLAYERS(boss["killed"]));

  if (!boss["deaths"])
    return ret + _LANG_BOSSES_UNDEFEATED;

  ret += _LANG_BOSSES_FIRST + _death_line(boss["first"]) + "\n";
  ret += _LANG_BOSSES_LAST + _death_line(boss["last"]) + "\n";
  ret += _LANG_BOSSES_FASTEST + _death_line(boss["fastest"]) + "\n";

  recent = boss["recent"];
  first = sizeof(recent) - SHOWN_DEATHS;
  if (first < 0)
    first = 0;

  ret += "\n" + _LANG_BOSSES_RECENT;
  for (i = sizeof(recent) - 1; i >= first; i--)
    ret += "  " + _death_line(recent[i]) + "\n";

  return ret;
}

static int cmd(string str, object me, string verb)
{
  object bosses_ob;
  mapping boss;
  string game, key, text;

  bosses_ob = handler("bosses", me);
  if (!bosses_ob)
  {
    notify_fail(_LANG_BOSSES_BROKEN);
    return 0;
  }

  if (str == _LANG_BOSSES_PRUNE && me->query_coder())
  {
    write(sprintf(_LANG_BOSSES_PRUNED, bosses_ob->prune()));
    return 1;
  }

  game = game_name(me);

  if (!strlen(str))
  {
    text = _list_text(bosses_ob, game);
    if (!strlen(text))
    {
      write(_LANG_BOSSES_NONE);
      return 1;
    }

    tell_object(me, handler("frames")->frame(text, _LANG_BOSSES_TITLE,
                                             this_user()->query_cols()));
    return 1;
  }

  key = bosses_ob->find_boss(game, str);
  if (!strlen(key))
  {
    write(_LANG_BOSSES_UNKNOWN);
    return 1;
  }

  boss = bosses_ob->query_boss(key);
  tell_object(me, handler("frames")->frame(_record_text(boss), boss["name"],
                                           this_user()->query_cols()));
  return 1;
}
