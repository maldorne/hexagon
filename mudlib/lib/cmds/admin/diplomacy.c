// diplomacy -- admin tool to inspect and edit the diplomacy graph.
//
// The diplomacy handler owns the graph as mutable, persisted state; it starts
// empty for every game and is built by hand here. All subcommands act on the
// game of the area you stand in (falling back to your own game).
//
//   diplomacy                       list every citizenship of the game
//   diplomacy <cit>                 show one citizenship
//   diplomacy add <cit>             create a citizenship
//   diplomacy remove <cit>          delete a citizenship (scrubs it elsewhere)
//   diplomacy <cit> parent <p>      set parent kingdom ("none" clears)
//   diplomacy <cit> security <n>    set guard count its areas field
//   diplomacy <cit> guard <path>    set the NPC source its guards spawn from
//   diplomacy <cit> ally <other>    mark <other> an ally
//   diplomacy <cit> unally <other>  drop <other> as ally
//   diplomacy <cit> enemy <other>   mark <other> an enemy (guards block them)
//   diplomacy <cit> unenemy <other> drop <other> as enemy

#include <mud/cmd.h>
#include <areas/diplomacy.h>

inherit CMD_BASE;

void setup()
{
  set_aliases(({ "diplomacy", "diplomacia" }));
  set_usage("diplomacy [<citizenship>] [<field> <value>]");
  set_help("Inspect and edit the diplomacy graph of the game you are in. " +
           "With no argument, lists every citizenship. With a citizenship " +
           "name, shows it. Editing subcommands: 'add <cit>', 'remove <cit>', " +
           "'<cit> parent <p>', '<cit> security <n>', '<cit> guard <path>', " +
           "'<cit> ally <other>', '<cit> unally <other>', '<cit> enemy " +
           "<other>', '<cit> unenemy <other>'. The graph is persisted.");
}

private string _line(string name, mapping rec)
{
  string ret;
  string * allies, * enemies;

  ret = "  " + name + "   security " +
        (intp(rec["security"]) ? rec["security"] : 0);
  if (stringp(rec["parent"]) && strlen(rec["parent"]))
    ret += "   parent " + rec["parent"];
  if (stringp(rec["guard"]) && strlen(rec["guard"]))
    ret += "   guard " + rec["guard"];
  ret += "\n";

  allies = pointerp(rec["allies"]) ? rec["allies"] : ({ });
  enemies = pointerp(rec["enemies"]) ? rec["enemies"] : ({ });
  ret += "      allies:  " + (sizeof(allies) ? implode(allies, ", ") : "-") + "\n";
  ret += "      enemies: " + (sizeof(enemies) ? implode(enemies, ", ") : "-") + "\n";
  return ret;
}

// The game to act on: the current area's, else the admin's own game.
private string _game(object me)
{
  object loc, area;
  string game;

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
  return game;
}

static int cmd(string str, object me, string verb)
{
  object h;
  string game;
  string * words;
  mapping g;

  game = _game(me);
  if (!strlen(game))
  {
    notify_fail("Cannot tell which game to act on -- stand in a game area.\n");
    return 0;
  }

  h = load_object(DIPLOMACY_HANDLER);
  words = (str && strlen(str)) ? explode(str, " ") - ({ "" }) : ({ });

  // --- listing / showing -------------------------------------------------
  if (!sizeof(words))
  {
    string out;
    string * names;
    int i;

    g = h->query_relations(game);
    if (!mappingp(g) || !map_sizeof(g))
    {
      write("Game '" + game + "' has no diplomacy defined yet. " +
            "Build it with 'diplomacy add <citizenship>'.\n");
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

  // --- create / delete ---------------------------------------------------
  if (words[0] == "add")
  {
    if (sizeof(words) < 2)
    {
      notify_fail("Usage: diplomacy add <citizenship>\n");
      return 0;
    }
    h->add_citizenship(game, words[1]);
    write("Added citizenship '" + words[1] + "' to game '" + game + "'.\n");
    return 1;
  }

  if (words[0] == "remove")
  {
    if (sizeof(words) < 2)
    {
      notify_fail("Usage: diplomacy remove <citizenship>\n");
      return 0;
    }
    if (!h->remove_citizenship(game, words[1]))
    {
      notify_fail("No citizenship '" + words[1] + "' in game '" + game + "'.\n");
      return 0;
    }
    write("Removed citizenship '" + words[1] + "'.\n");
    return 1;
  }

  // --- from here, words[0] is a citizenship name -------------------------
  g = h->query_relations(game);
  if (!g[words[0]])
  {
    notify_fail("No citizenship '" + words[0] + "' in game '" + game +
                "'. Create it with 'diplomacy add " + words[0] + "'.\n");
    return 0;
  }

  // show one citizenship
  if (sizeof(words) == 1)
  {
    write("Diplomacy of '" + words[0] + "' (game " + game + "):\n" +
          _line(words[0], g[words[0]]));
    return 1;
  }

  // editing: diplomacy <cit> <field> <value>
  if (sizeof(words) < 3)
  {
    notify_fail("Usage: diplomacy <cit> <parent|security|guard|ally|" +
                "unally|enemy|unenemy> <value>\n");
    return 0;
  }

  {
    string cit, field, value;
    cit = words[0];
    field = words[1];
    value = implode(words[2 ..], " ");

    switch (field)
    {
      case "parent":
        h->set_parent(game, cit, (value == "none") ? "" : value);
        break;
      case "security":
        h->set_security(game, cit, to_int(value));
        break;
      case "guard":
        h->set_guard(game, cit, (value == "none") ? "" : value);
        break;
      case "ally":
        h->add_relationship(game, DIPLOMACY_RELATION_ALLY, cit, value);
        break;
      case "unally":
        h->remove_relationship(game, DIPLOMACY_RELATION_ALLY, cit, value);
        break;
      case "enemy":
        h->add_relationship(game, DIPLOMACY_RELATION_ENEMY, cit, value);
        break;
      case "unenemy":
        h->remove_relationship(game, DIPLOMACY_RELATION_ENEMY, cit, value);
        break;
      default:
        notify_fail("Unknown field '" + field + "'. Use parent, security, " +
                    "guard, ally, unally, enemy or unenemy.\n");
        return 0;
    }
  }

  // reflect the new state back
  g = h->query_relations(game);
  write("Updated '" + words[0] + "':\n" + _line(words[0], g[words[0]]));
  return 1;
}
