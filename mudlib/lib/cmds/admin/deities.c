// deities -- admin tool to read the accounts the deities handler keeps for the
// followers of every deity. Everything here acts on the game you are in.
//
//   deities                  the game's deities and how many follow each
//   deities <deity>          a deity's followers and their counts
//   deities player <name>    the accounts of one player

#include <mud/cmd.h>
#include <deities.h>

inherit CMD_BASE;

void setup()
{
  set_aliases(({ "deities", "deidades" }));
  set_usage("deities [<deity> | player <name>]");
  set_help(
    "Read the accounts of the followers of the deities of the game you are in.\n" +
    "\n" +
    "  deities                 every deity of the game (its obj/deities/) and\n" +
    "                          how many followers each has on the books\n" +
    "  deities <deity>         a deity's followers: when they took it, their\n" +
    "                          offerings (how many and their worth), prayers\n" +
    "                          made and heard, times cursed. <deity> is the\n" +
    "                          file name under obj/deities/ or a full path\n" +
    "  deities player <name>   the accounts of one player\n" +
    "\n" +
    "Only the accounts: the deities handler decides nothing yet.");
}

private string when(int t)
{
  return t ? ctime(t, 4) : "-";
}

// The deities the game ships, full paths with ".c".
private string * game_deities(object me)
{
  string dir;
  mixed * names;
  string * ret;
  int i;

  dir = game_root(me) + "obj/deities/";
  names = get_dir(dir + "*.c");
  ret = ({ });

  for (i = 0; i < sizeof(names); i++)
    if (stringp(names[i]))
      ret += ({ dir + names[i] });

  return ret;
}

// A deity named by its file name under the game's obj/deities/, or by path.
private string resolve_deity(object me, string str)
{
  if (str[0] == '/')
    return str;
  return game_root(me) + "obj/deities/" + str + ".c";
}

private string account_line(string name, mapping rec)
{
  return sprintf("  %-14s %-18s %4d (%7d)  %4d/%-4d  %3d\n", capitalize(name),
                 when(rec[DEITY_REC_WORSHIPPED]), rec[DEITY_REC_OFFERINGS],
                 rec[DEITY_REC_OFFERED], rec[DEITY_REC_ANSWERED],
                 rec[DEITY_REC_PRAYERS], rec[DEITY_REC_CURSED]);
}

private string header()
{
  return sprintf("  %-14s %-18s %15s  %9s  %3s\n", "follower", "worshipped",
                 "offerings(worth)", "heard/pr", "cur");
}

// How a deity is called, from its object; its path when it does not load.
private string deity_title(string path)
{
  object deity;
  mixed name;

  deity = nil;
  catch(deity = load_object(path));
  name = deity ? deity->query_short() : nil;

  return (stringp(name) && strlen(name)) ? capitalize(name) : path;
}

// The name a game shows, from its master object; its directory otherwise.
private string game_title(object me)
{
  object master;
  mixed name;

  master = nil;
  catch(master = load_object(game_root(me) + "master.c"));
  name = master ? master->query_game_name() : nil;

  return (stringp(name) && strlen(name)) ? name : game_root(me);
}

static int cmd(string str, object me, string verb)
{
  object deities;
  mapping followers, rec;
  string * paths, * names;
  string ret, path;
  int i;

  deities = handler("deities", me);
  if (!deities)
  {
    notify_fail("No deities handler here.\n");
    return 0;
  }

  // --- the list -------------------------------------------------------------
  if (!str || !strlen(str))
  {
    paths = game_deities(me);
    names = deities->query_deities();
    for (i = 0; i < sizeof(names); i++)
      if (member_array(names[i], paths) == -1)
        paths += ({ names[i] });

    if (!sizeof(paths))
    {
      write("This game has no deities.\n");
      return 1;
    }

    ret = "Deities of " + game_title(me) + "\n";
    for (i = 0; i < sizeof(paths); i++)
      ret += sprintf("  %-20s %4d followers\n", deity_title(paths[i]),
                     map_sizeof(deities->query_followers(paths[i])));
    write(ret);
    return 1;
  }

  // --- one player -----------------------------------------------------------
  if (sscanf(str, "player %s", path) == 1)
  {
    path = lower_case(trim(path));
    names = deities->query_deities();
    ret = "";
    for (i = 0; i < sizeof(names); i++)
    {
      rec = deities->query_follower(path, names[i]);
      if (rec)
        ret += "  " + deity_title(names[i]) + "\n" + account_line(path, rec);
    }

    if (!strlen(ret))
    {
      write(capitalize(path) + " is on no deity's books.\n");
      return 1;
    }

    write("Accounts of " + capitalize(path) + "\n" + header() + ret);
    return 1;
  }

  // --- one deity ------------------------------------------------------------
  path = resolve_deity(me, trim(str));
  followers = deities->query_followers(path);
  if (!map_sizeof(followers))
  {
    write((load_object(path) ? deity_title(path) : "No deity loads at " + path) +
          ": no followers on the books.\n");
    return 1;
  }

  names = sort_array(map_indices(followers));
  ret = "Followers of " + deity_title(path) + "\n" + header();
  for (i = 0; i < sizeof(names); i++)
    ret += account_line(names[i], followers[names[i]]);

  me->user()->more_string(ret);
  return 1;
}
