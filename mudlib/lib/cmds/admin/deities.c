// deities -- admin tool to read the devotion the deities handler keeps for the
// devotees of every deity. Everything here acts on the game you are in.
//
//   deities                  the game's deities and how many devotees each has
//   deities <deity>          a deity's devotees, the most devoted first
//   deities player <name>    one player: devotion in each deity and, if they
//                            are connected, their lifetime totals

#include <mud/cmd.h>
#include <deities.h>
#include <basic/gender.h>

inherit CMD_BASE;

void setup()
{
  set_aliases(({ "deities", "deidades" }));
  set_usage("deities [<deity> | player <name>]");
  set_help(
    "Read the devotion of the followers of the deities of the game you are in.\n" +
    "\n" +
    "  deities                 every deity of the game (its obj/deities/) and\n" +
    "                          how many active devotees each has\n" +
    "  deities <deity>         the deity's sheet and its devotees, the most\n" +
    "                          devoted first. <deity> is the file name under\n" +
    "                          obj/deities/ or a full path\n" +
    "  deities player <name>   the devotion of one player in each deity, and\n" +
    "                          their lifetime totals (worship, prayers made and\n" +
    "                          heard, curses, offerings and their worth) only\n" +
    "                          while they are connected: the totals live in the\n" +
    "                          player and a save file is never read\n" +
    "\n" +
    "Devotion rises with prayers and offerings and falls with time; a player\n" +
    "who stops praying and offering leaves the list. Only the book: the deities\n" +
    "handler decides nothing yet.");
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

// The lifetime totals of a connected player.
private string totals_text(object player)
{
  mapping totals;
  int since;

  totals = player->query_deity_totals();
  since = totals[DEITY_TOT_WORSHIPPED];

  return "  Worshipping    " + (since ? "since " + when(since) : "-") + "\n" +
         "  Prayers        " + (int)totals[DEITY_TOT_PRAYERS] + " made, " +
           (int)totals[DEITY_TOT_ANSWERED] + " heard, " +
           (int)totals[DEITY_TOT_CURSED] + " curses\n" +
         "  Offerings      " + (int)totals[DEITY_TOT_OFFERINGS] + ", worth " +
           (int)totals[DEITY_TOT_OFFERED] + "\n";
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
    out += ({ keys[i] + " = " + to_text(value) });
  }

  return implode(out, ", ");
}

// The name of every race and lineage the game (and the mudlib) has, by its id:
// the deity lists name races by id, not by file.
private mapping race_names(object me)
{
  string * dirs, * files, * subdirs;
  mapping names;
  object race;
  mixed id, name;
  int i, j;

  names = ([ ]);
  dirs = ({ "/lib/obj/races/", game_root(me) + "obj/races/" });

  // the lineages sit in a directory per parent race
  subdirs = get_dir(game_root(me) + "obj/races/*");
  for (i = 0; i < sizeof(subdirs); i++)
    if (file_size(game_root(me) + "obj/races/" + subdirs[i]) == -2)
      dirs += ({ game_root(me) + "obj/races/" + subdirs[i] + "/" });

  for (i = 0; i < sizeof(dirs); i++)
  {
    files = get_dir(dirs[i] + "*.c");
    for (j = 0; j < sizeof(files); j++)
    {
      race = nil;
      catch(race = load_object(dirs[i] + files[j]));
      if (!race || !race->query_is_race_ob())
        continue;
      id = race->query_race_id();
      name = race->query_short();
      if (stringp(id) && stringp(name) && strlen(name))
        names[id] = capitalize(name);
    }
  }

  return names;
}

// One row of a list of names: the label columns, then the names, wrapped to
// the width and indented under the first one.
private string list_row(string label, string sublabel, string * paths, int width,
                        varargs mapping ids)
{
  string * words, * lines;
  string line, indent, item;
  int i;

  indent = sprintf("  %-16s %-10s ", "", "");
  line = sprintf("  %-16s %-10s ", label, sublabel);
  if (!sizeof(paths))
    return line + "-\n";

  lines = ({ });
  words = ({ });
  for (i = 0; i < sizeof(paths); i++)
    words += ({ (ids && ids[paths[i]] ? ids[paths[i]] : deity_title(paths[i])) +
                (i < sizeof(paths) - 1 ? "," : "") });

  for (i = 0; i < sizeof(words); i++)
  {
    item = words[i];
    if (strlen(line) + strlen(item) + 1 > width && strlen(line) > strlen(indent))
    {
      lines += ({ line });
      line = indent;
    }
    line += (line == indent || line[strlen(line) - 1] == ' ' ? "" : " ") + item;
  }

  return implode(lines + ({ line }), "\n") + "\n";
}

// What the deity object says about itself: who may worship it, what it takes,
// what it can be prayed for, in aligned columns.
private string deity_sheet(object deity, int width, mapping ids)
{
  string ret;
  mapping * offerings;
  string * prayers;
  mapping table;
  int * range;
  int i, luck;

  range = deity->query_alignment_range();
  ret = deity_title(deity->query_deity_path()) + "  (" +
        deity->query_deity_path() + ")\n\n" +
        sprintf("  %-16s %s\n", "Gender",
          deity->query_gender() == GENDER_FEMALE ? "female" : "male") +
        sprintf("  %-16s %d to %d\n\n", "Real alignment", range[0], range[1]);

  // who may come, and who may not, each list on its own rows
  ret += list_row("Races", "allowed", deity->query_races(), width, ids) +
         list_row("", "forbidden", deity->query_forbidden_races(), width, ids) +
         list_row("Lineages", "allowed", deity->query_lineages(), width, ids) +
         list_row("", "forbidden", deity->query_forbidden_lineages(), width, ids) +
         list_row("Guilds", "allowed", deity->query_guilds(), width) +
         list_row("", "forbidden", deity->query_forbidden_guilds(), width) + "\n";

  ret += "  Offerings\n";
  offerings = deity->query_offerings();
  if (!sizeof(offerings))
    ret += "    none\n";
  for (i = 0; i < sizeof(offerings); i++)
    ret += "    " + rule_text(offerings[i]) + "\n";

  // each prayer by its id, the first word that asks for it and its luck (the
  // default luck when the deity sets none)
  ret += "\n" + sprintf("  %-16s %-16s %s\n", "Prayers", "Word", "Luck");
  table = deity->query_prayers();
  prayers = map_indices(table);
  if (!sizeof(prayers))
    ret += "    none\n";
  for (i = 0; i < sizeof(prayers); i++)
  {
    luck = deity->query_prayer_luck(prayers[i]);
    ret += sprintf("    %-14s %-16s %4d\n", prayers[i],
             sizeof(table[prayers[i]][DEITY_PRAYER_WORDS]) ?
               table[prayers[i]][DEITY_PRAYER_WORDS][0] : "-",
             luck ? luck : DEITY_PRAY_LUCK);
  }

  return ret + "\n";
}

static int cmd(string str, object me, string verb)
{
  object deities, god, player;
  mixed * devotees;
  string * paths, * names;
  string ret, path;
  int i, points;

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
      ret += sprintf("  %-20s %4d devotees\n", deity_title(paths[i]),
                     sizeof(deities->query_devotees(paths[i])));
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
      points = deities->query_devotion(path, names[i]);
      if (points)
        ret += sprintf("  %-20s %5d\n", deity_title(names[i]), points);
    }

    if (!strlen(ret))
      ret = "  on no deity's book\n";

    ret = "Devotion of " + capitalize(path) + "\n" + ret + "\n";

    // the totals live in the player: only readable while connected
    player = find_player(path);
    if (player)
      ret += "Lifetime totals\n" + totals_text(player);
    else
      ret += "Not connected: their lifetime totals cannot be read.\n";

    write(ret);
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

  ret = deity_sheet(god, me->user() ? me->user()->query_cols() : 80,
                    race_names(me));
  devotees = deities->query_devotees(path);
  if (!sizeof(devotees))
  {
    me->user()->more_string(ret + "No active devotees.\n");
    return 1;
  }

  ret += "Devotees\n";
  for (i = 0; i < sizeof(devotees); i++)
    ret += sprintf("  %-20s %5d\n", capitalize(devotees[i][0]), devotees[i][1]);

  me->user()->more_string(ret);
  return 1;
}
