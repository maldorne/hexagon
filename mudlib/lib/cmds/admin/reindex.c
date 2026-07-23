
#include <mud/cmd.h>
#include <room/location.h>

inherit CMD_BASE;

void setup()
{
  set_aliases(({ "reindex" }));
  set_usage("reindex [ area <name> | game ]");
  set_help(
    "Reload persisted locations so their sector.o and area.o files\n" +
    "rebuild against the current code (terrain-type tallies and\n" +
    "path/road way exits used by the world map).\n" +
    "\n" +
    "  reindex                current area (from where you stand)\n" +
    "  reindex area <name>    the named area\n" +
    "  reindex game           every area of the current game, one area\n" +
    "                         per tick so the batch stays within the\n" +
    "                         driver's per-execution limits\n" +
    "\n" +
    "Game is inferred from your current environment. Reindexing a\n" +
    "single area runs immediately; a whole game runs in the\n" +
    "background and reports each area as it finishes.\n");
}

static int cmd(string str, object me, string verb)
{
  string * tokens;
  string game, area;
  int count, areas;

  game = game_name(me);
  if (!strlen(game))
  {
    notify_fail("Cannot infer game from your current environment. " +
                "Stand inside a game first.\n");
    return 0;
  }

  tokens = explode(str ? str : "", " ") - ({ "" });

  // ===== whole game, chunked across ticks =====
  if (sizeof(tokens) == 1 && tokens[0] == "game")
  {
    areas = load_object(LOCATION_HANDLER)->reindex_game(game, me);
    if (areas < 0)
    {
      notify_fail("Game '" + game + "' has no areas directory.\n");
      return 0;
    }
    write("Reindexing " + areas + " area" + (areas == 1 ? "" : "s") +
          " of '" + game + "' in the background ...\n");
    return 1;
  }

  // ===== a named area =====
  if (sizeof(tokens) == 2 && tokens[0] == "area")
    area = tokens[1];
  // ===== no args: the area we are standing in =====
  else if (sizeof(tokens) == 0)
  {
    object env, area_ob;

    env = environment(me);
    if (!env || !env->query_location())
    {
      notify_fail("You are not standing in a location. Use " +
                  "'reindex area <name>'.\n");
      return 0;
    }

    area_ob = env->query_area();
    if (!area_ob)
    {
      notify_fail("Your location has no area.\n");
      return 0;
    }
    area = area_ob->query_area_name();
  }
  else
  {
    notify_fail("Usage: reindex [ area <name> | game ]\n");
    return 0;
  }

  count = load_object(LOCATION_HANDLER)->reindex_area(game, area);
  if (count < 0)
  {
    notify_fail("No area '" + area + "' in game '" + game + "'.\n");
    return 0;
  }

  write("Reindexed area '" + area + "' in '" + game + "': " +
        count + " location" + (count == 1 ? "" : "s") + ".\n");
  return 1;
}
