// diplomacy -- coder tool to inspect the diplomacy graph (read-only).
//
// Shows each citizenship's parent, allies, enemies and security level for a
// game, straight from the diplomacy handler (which reads the game's
// tables/diplomacy.c). It only reads; it never changes anything.
//
//   diplomacy               all citizenships of your current area's game
//   diplomacy <citizenship> just that one
//
// The game is taken from the area you stand in; falling back to your own game.

#include <mud/cmd.h>
#include <areas/diplomacy.h>

inherit CMD_BASE;

void setup()
{
  set_aliases(({ "diplomacy", "diplomacia" }));
  set_usage("diplomacy [<citizenship>]");
  set_help("Show the diplomacy graph -- each citizenship's parent, allies, " +
           "enemies and security level -- for the game of the area you are in. " +
           "Give a citizenship name to show only that one. Read-only.");
}

private string _line(string name, mapping rec)
{
  string ret;
  string * allies, * enemies;

  ret = "  " + name + "   security " +
        (intp(rec["security"]) ? rec["security"] : 0);
  if (stringp(rec["parent"]) && strlen(rec["parent"]))
    ret += "   parent " + rec["parent"];
  ret += "\n";

  allies = pointerp(rec["allies"]) ? rec["allies"] : ({ });
  enemies = pointerp(rec["enemies"]) ? rec["enemies"] : ({ });
  ret += "      allies:  " + (sizeof(allies) ? implode(allies, ", ") : "-") + "\n";
  ret += "      enemies: " + (sizeof(enemies) ? implode(enemies, ", ") : "-") + "\n";
  return ret;
}

static int cmd(string str, object me, string verb)
{
  object loc, area;
  string game, only, out;
  mapping g;
  string * names;
  int i;

  only = str && strlen(str) ? str : "";

  // the game to inspect: the current area's, else the coder's own game
  game = "";
  loc = environment(me);
  if (loc && loc->query_location())
  {
    area = loc->query_area();
    if (area)
      game = game_from_path(area->query_area_path());
  }
  if (!strlen(game))
    game = game_name(me);

  if (!strlen(game))
  {
    notify_fail("Cannot tell which game to inspect -- stand in a game area.\n");
    return 0;
  }

  g = DIPLOMACY_HANDLER->query_relations(game);
  if (!mappingp(g) || !map_sizeof(g))
  {
    write("Game '" + game + "' has no diplomacy defined " +
          "(no tables/diplomacy.c or it is empty).\n");
    return 1;
  }

  if (strlen(only))
  {
    if (!g[only])
    {
      notify_fail("No citizenship '" + only + "' in game '" + game + "'.\n");
      return 0;
    }
    write("Diplomacy of '" + only + "' (game " + game + "):\n" +
          _line(only, g[only]));
    return 1;
  }

  out = "Diplomacy graph for game '" + game + "' (" + map_sizeof(g) +
        " citizenship" + (map_sizeof(g) == 1 ? "" : "s") + "):\n";
  names = map_indices(g);
  for (i = 0; i < sizeof(names); i++)
    out += _line(names[i], g[names[i]]);

  write(out);
  return 1;
}
