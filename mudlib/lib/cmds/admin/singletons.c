#include <kernel.h>
#include <mud/cmd.h>

inherit CMD_BASE;

void setup()
{
  position = 0;
}

string query_usage()
{
  return "singletons";
}

string query_short_help()
{
  return "Shows a list of the singleton objects handled by the system.";
}

// The name a singleton was asked for. The store keys every entry by the game
// that resolved it ("<game>-<name>"), or by the bare name for objects that
// belong to no game, so the key cannot be split on the dash -- a game name may
// carry one itself. The requested name is always the last segment of the
// resolved file's path, which splits the key unambiguously.
private string entry_name(object ob)
{
  string path;

  path = base_name(ob);

  return path[strsrch(path, "/", -1) + 1 ..];
}

private void show_group(mapping obs, string title)
{
  mapping by_game;
  string * ks, * games;
  int i;

  if (!map_sizeof(obs))
  {
    write("\n * No %^BOLD%^" + title + "%^RESET%^ loaded yet.\n");
    return;
  }

  // Group by the game each entry was resolved for, with the objects that
  // belong to no game under "global". This is what tells a real per-game
  // override apart from a game that simply resolves to the shared object: the
  // second is not a duplicate, it is the cached answer that saves re-checking
  // for an override the game does not have on every call.
  by_game = ([ ]);
  ks = map_indices(obs);

  for (i = 0; i < sizeof(ks); i++)
  {
    string name, game;

    name = entry_name(obs[ks[i]]);
    game = (ks[i] == name) ? "" :
           ks[i][0 .. strlen(ks[i]) - strlen(name) - 2];

    if (!by_game[game])
      by_game[game] = ({ });

    by_game[game] += ({ ({ name, base_name(obs[ks[i]]) }) });
  }

  write("\n * %^BOLD%^" + title + " loaded%^RESET%^:\n");

  games = map_indices(by_game);

  for (i = 0; i < sizeof(games); i++)
  {
    mixed * rows;
    int j;

    write("\n   %^GREEN%^" + (strlen(games[i]) ? games[i] : "global") +
          "%^RESET%^:\n");
    rows = by_game[games[i]];

    for (j = 0; j < sizeof(rows); j++)
    {
      string own;

      // a game entry pointing outside /games/ resolved to the shared object
      own = (strlen(games[i]) && rows[j][1][0 .. 5] != "/games")
              ? "   %^YELLOW%^(shared)%^RESET%^" : "";

      write("     %^BOLD%^" + rows[j][0] + "%^RESET%^ -> " +
            rows[j][1] + own + "\n");
    }
  }
}

static int cmd(string name, object me, string verb)
{
  show_group(SINGLETON_HANDLER->handlers(), "Handlers");
  show_group(SINGLETON_HANDLER->tables(), "Tables");

  write("\n");

  return 1;
}
