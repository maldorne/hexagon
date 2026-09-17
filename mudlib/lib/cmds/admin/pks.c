// pks - what the player killing records say: every killer at a glance, or
// one character in full, both as killer and as victim.

#include <mud/cmd.h>

#define PK_HANDLER "/lib/handlers/pk"

inherit CMD_BASE;

// the records being sorted, while show_all sorts them
static mapping sorting;

void setup()
{
  set_aliases(({ "pks" }));
  set_usage("pks [name]");
  set_help(
    "Show the player killing records.\n" +
    "\n" +
    "With no name, one line per killer: kills, the average level of their " +
    "victims, the last kill and how many addresses they killed from. With a " +
    "name, that character in full: whom they killed by race, guild and " +
    "citizenship, the game each kill happened in, their last kills, and who " +
    "has killed them.\n" +
    "\n" +
    "Kills involving a coder are not recorded.");
}

private string when(int t)
{
  return t ? ctime(t, 4) : "-";
}

private string breakdown(string title, mapping counts)
{
  string * keys;
  string ret;
  int i;

  keys = map_indices(counts);

  if (!sizeof(keys))
    return "";

  ret = sprintf("  %-40s %6s %9s\n", title, "kills", "avg level");

  for (i = 0; i < sizeof(keys); i++)
    ret += sprintf("  %-40s %6d %9d\n", keys[i], counts[keys[i]][0],
                   counts[keys[i]][1] / counts[keys[i]][0]);

  return ret + "\n";
}

private string show_all()
{
  mapping killers;
  string * names;
  string ret;
  int i;

  killers = PK_HANDLER->query_killers();
  sorting = killers;
  names = sort_array(map_indices(killers), "by_kills", this_object());

  if (!sizeof(names))
    return "No player has killed another player.\n";

  ret = sprintf("%-15s %6s %9s  %-17s %5s\n", "killer", "kills", "avg level",
                "last kill", "sites");

  for (i = 0; i < sizeof(names); i++)
    ret += sprintf("%-15s %6d %9d  %-17s %5d\n", capitalize(names[i]),
                   killers[names[i]]["kills"], PK_HANDLER->avg_pk_level(names[i]),
                   when(killers[names[i]]["last"]), sizeof(killers[names[i]]["sites"]));

  return ret;
}

int by_kills(string a, string b)
{
  return sorting[b]["kills"] - sorting[a]["kills"];
}

private string show_one(string name)
{
  mapping killer, victim;
  string * names;
  string ret;
  int i;

  killer = PK_HANDLER->query_killer(name);
  victim = PK_HANDLER->query_victim(name);

  if (!killer && !victim)
    return capitalize(name) + " has no player killing records.\n";

  ret = "Player killing records of " + capitalize(name) + "\n\n";

  if (killer)
  {
    ret += "As killer: " + killer["kills"] + " kill(s), first " + when(killer["first"]) +
           ", last " + when(killer["last"]) + ", average victim level " +
           PK_HANDLER->avg_pk_level(name) + ".\n";
    ret += "Sites: " + (sizeof(killer["sites"]) ? implode(killer["sites"], ", ") : "-") +
           "\n\n";
    ret += breakdown("game", killer["games"]);
    ret += breakdown("race", killer["races"]);
    ret += breakdown("guild", killer["guilds"]);
    ret += breakdown("citizenship", killer["citizenships"]);

    ret += "  Last kills:\n";
    for (i = sizeof(killer["recent"]) - 1; i >= 0; i--)
      ret += sprintf("    %-17s %-15s level %-3d %s\n", when(killer["recent"][i][1]),
                     capitalize(killer["recent"][i][0]), killer["recent"][i][2],
                     killer["recent"][i][3]);
    ret += "\n";
  }

  if (victim)
  {
    ret += "As victim: killed " + victim["deaths"] + " time(s), last " +
           when(victim["last"]) + ".\n";

    names = map_indices(victim["by"]);
    for (i = 0; i < sizeof(names); i++)
      ret += sprintf("    %-15s %d time(s)\n", capitalize(names[i]), victim["by"][names[i]]);
  }

  return ret;
}

static int cmd(string str, object me, string verb)
{
  if (!strlen(str))
    write(show_all());
  else
    write(show_one(lower_case(str)));

  return 1;
}
