
#include <mud/cmd.h>
#include <room/location-cleaner.h>

inherit CMD_BASE;

void setup()
{
  set_aliases(({ "cleaner" }));
  set_usage("cleaner\n" +
            "cleaner games\n" +
            "cleaner list <game>\n" +
            "cleaner cache");
  set_help("Inspect the location-cleaner handler (" + LOCATION_CLEANER + ").\n" +
           "\n" +
           "  cleaner              summary: registered counts, prewarm queue,\n" +
           "                       cache sizes, worker state\n" +
           "  cleaner games        registered map objects per game\n" +
           "  cleaner list <game>  the registered objects in one game bucket\n" +
           "                       (use \"\" for the no-game bucket)\n" +
           "  cleaner cache        L1 region / L2 file cache entries with the\n" +
           "                       seconds of TTL each has left\n");
}

// One "key : value" line, key left-padded to a fixed column.
private string _row(string key, mixed value)
{
  return sprintf("  %-16s %s\n", key, "" + value);
}

private int do_stats()
{
  mapping s;
  mapping counts;
  string * games;
  string out;
  int i;

  s = LOCATION_CLEANER->query_stats();

  out = "Location cleaner\n";
  out += _row("games:",         s["games"]);
  out += _row("registered:",    s["registered"]);
  out += _row("queue pending:", s["queue_pending"]);
  out += _row("in flight:",     s["in_flight"]);
  out += _row("region cached:", s["region_cached"]);
  out += _row("file cached:",   s["file_cached"]);
  out += _row("worker active:", s["worker_active"] ? "yes" : "no");
  out += _row("evicted total:", s["evicted_total"]);

  counts = LOCATION_CLEANER->query_game_counts();
  games = map_indices(counts);
  if (sizeof(games))
  {
    out += "\n  per game:\n";
    for (i = 0; i < sizeof(games); i++)
      out += sprintf("    %-24s %d\n",
                     (strlen(games[i]) ? games[i] : "(no game)"), counts[games[i]]);
  }

  write(out);
  return 1;
}

private int do_games()
{
  mapping counts;
  string * games;
  string out;
  int i;

  counts = LOCATION_CLEANER->query_game_counts();
  games = map_indices(counts);

  if (!sizeof(games))
  {
    write("No registered objects.\n");
    return 1;
  }

  out = "Registered map objects per game:\n";
  for (i = 0; i < sizeof(games); i++)
    out += sprintf("  %-24s %d\n",
                   (strlen(games[i]) ? games[i] : "(no game)"), counts[games[i]]);

  write(out);
  return 1;
}

private int do_list(string game)
{
  object * obs;
  string out;
  int i;

  obs = LOCATION_CLEANER->query_registered(game);

  if (!sizeof(obs))
  {
    write("No registered objects in game '" + game + "'.\n");
    return 1;
  }

  out = sizeof(obs) + " registered in '" +
        (strlen(game) ? game : "(no game)") + "':\n";
  for (i = 0; i < sizeof(obs); i++)
  {
    string name;
    if (!obs[i])
      continue;
    name = obs[i]->query_file_name();
    if (!name || !strlen(name))
      name = object_name(obs[i]);
    out += "  " + name + "\n";
  }

  write(out);
  return 1;
}

// Dump a TTL-stamped cache mapping ([ key : expiry ]) as key + seconds left.
private string _dump_cache(string title, mapping cache)
{
  string * keys;
  string out;
  int i, now;

  keys = map_indices(cache);
  out = title + " (" + sizeof(keys) + "):\n";
  now = time();
  // no fixed field width: DGD sprintf truncates strings to the field size,
  // and these keys (file paths) are long and unbounded
  for (i = 0; i < sizeof(keys); i++)
    out += "  " + keys[i] + "  (" + (cache[keys[i]] - now) + "s)\n";
  return out;
}

private int do_cache()
{
  write(_dump_cache("L1 region cache", LOCATION_CLEANER->query_region_warmed()) +
        "\n" +
        _dump_cache("L2 file cache", LOCATION_CLEANER->query_warmed()));
  return 1;
}

static int cmd(string str, object me, string verb)
{
  string * args;

  if (!str || !strlen(str))
    return do_stats();

  args = explode(str, " ");

  if (args[0] == "games")
    return do_games();

  if (args[0] == "list")
    return do_list(sizeof(args) > 1 ? implode(args[1 ..], " ") : "");

  if (args[0] == "cache")
    return do_cache();

  notify_fail("Usage:\n" + query_usage());
  return 0;
}
