// deities -- admin tool to read the accounts the deities handler keeps for the
// followers of every deity. Everything here acts on the game you are in.
//
//   deities                  the game's deities and how many follow each
//   deities <deity>          a deity's followers and their counts
//   deities player <name>    the accounts of one player

#include <mud/cmd.h>
#include <deities.h>
#include <basic/gender.h>

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

// The names of a list of social objects (races, guilds...), or "-".
private string titles_of(string * paths)
{
  string * out;
  int i;

  if (!sizeof(paths))
    return "-";

  out = ({ });
  for (i = 0; i < sizeof(paths); i++)
    out += ({ deity_title(paths[i]) });

  return implode(out, ", ");
}

// A value as text, whatever it is.
string to_text(mixed value)
{
  if (stringp(value))
    return value;
  if (intp(value) || floatp(value))
    return "" + value;
  return "?";
}

// One offering rule, every key it sets, as key=value.
private string rule_text(mapping rule)
{
  string * keys, * out;
  mixed value;
  int i;

  keys = sort_array(map_indices(rule));
  out = ({ });
  for (i = 0; i < sizeof(keys); i++)
  {
    value = rule[keys[i]];
    if (pointerp(value))
      value = implode(map_array(value, "to_text", this_object()), "|");
    out += ({ keys[i] + "=" + to_text(value) });
  }

  return implode(out, " ");
}

// What the deity object says about itself: who may worship it, what it takes,
// what it can be prayed for.
private string deity_sheet(object deity)
{
  string ret;
  mapping * offerings;
  string * prayers;
  mapping table;
  int * range;
  int i, luck;

  range = deity->query_alignment_range();
  ret = deity_title(deity->query_deity_path()) + "  (" +
        deity->query_deity_path() + ")\n" +
        "  gender: " + (deity->query_gender() == GENDER_FEMALE ? "female" : "male") + "\n" +
        "  races: " + titles_of(deity->query_races()) +
        "   forbidden: " + titles_of(deity->query_forbidden_races()) + "\n" +
        "  lineages: " + titles_of(deity->query_lineages()) +
        "   forbidden: " + titles_of(deity->query_forbidden_lineages()) + "\n" +
        "  guilds: " + titles_of(deity->query_guilds()) +
        "   forbidden: " + titles_of(deity->query_forbidden_guilds()) + "\n" +
        "  real alignment: " + range[0] + " to " + range[1] + "\n";

  ret += "  offerings:\n";
  offerings = deity->query_offerings();
  if (!sizeof(offerings))
    ret += "    none\n";
  for (i = 0; i < sizeof(offerings); i++)
    ret += "    " + rule_text(offerings[i]) + "\n";

  // each prayer by its id, the first word that asks for it and its luck (the
  // default luck when the deity sets none)
  ret += "  prayers:";
  table = deity->query_prayers();
  prayers = map_indices(table);
  if (!sizeof(prayers))
    ret += " none";
  for (i = 0; i < sizeof(prayers); i++)
  {
    luck = deity->query_prayer_luck(prayers[i]);
    ret += " " + prayers[i] +
           (sizeof(table[prayers[i]][DEITY_PRAYER_WORDS]) ?
              " '" + table[prayers[i]][DEITY_PRAYER_WORDS][0] + "'" : "") +
           " (luck " + (luck ? luck : DEITY_PRAY_LUCK) + ")";
  }

  return ret + "\n\n";
}

static int cmd(string str, object me, string verb)
{
  object deities, god;
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
  god = nil;
  catch(god = load_object(path));
  if (!god)
  {
    write("No deity loads at " + path + ".\n");
    return 1;
  }

  ret = deity_sheet(god);
  followers = deities->query_followers(path);
  if (!map_sizeof(followers))
  {
    me->user()->more_string(ret + "No followers on the books.\n");
    return 1;
  }

  names = sort_array(map_indices(followers));
  ret += "Followers\n" + header();
  for (i = 0; i < sizeof(names); i++)
    ret += account_line(names[i], followers[names[i]]);

  me->user()->more_string(ret);
  return 1;
}
