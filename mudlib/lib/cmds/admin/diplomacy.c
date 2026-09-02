// diplomacy -- admin tool to inspect and edit the diplomacy graph.
//
// The diplomacy handler owns the graph as mutable, persisted state; it starts
// empty for every game and is built by hand here. All subcommands act on the
// game of the area you stand in (falling back to your own game).
//
//   diplomacy                       list every citizenship (columns)
//   diplomacy tree                  the parent hierarchy, same columns
//   diplomacy <cit>                 show one citizenship in detail
//   diplomacy add <cit>             create a citizenship
//   diplomacy remove <cit>          delete a citizenship (scrubs it elsewhere)
//   diplomacy <cit> parent <p>      set parent ("none" makes it top-level)
//   diplomacy <cit> security <n>    set guard count its areas field
//   diplomacy <cit> guard <path>    set the NPC source its guards spawn from
//   diplomacy <cit> deity <path>    accept a deity's worship on its ground
//   diplomacy <cit> undeity <path>  stop accepting it
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
  set_usage("diplomacy [tree | <citizenship> [<field> <value>]]");
  set_help("Inspect and edit the diplomacy graph of the game you are in. " +
           "With no argument, lists every citizenship in columns; 'tree' shows " +
           "the parent hierarchy with the same columns. With a citizenship " +
           "name, shows it in detail. Editing subcommands: 'add <cit>', " +
           "'remove <cit>', '<cit> parent <p>', '<cit> security <n>', '<cit> " +
           "guard <path>', '<cit> deity <path>', '<cit> undeity <path>', " +
           "'<cit> ally <other>', '<cit> unally <other>', '<cit> " +
           "enemy <other>', '<cit> unenemy <other>'. The graph is persisted.");
}

// --- columnar rendering ----------------------------------------------------
//
// One shared table serves both the flat listing and the tree. Each row's first
// column is a label (a plain name in the listing, a name hung under box-drawing
// tree lines in the tree); every other column is identical. allies and enemies
// are lists, so a citizenship with several of either spans that many lines, the
// extra names stacked under their column with the leading columns left blank.
// sprintf's width pads by visible width (strlen(str, true)), so the multi-byte
// box-drawing glyphs in a tree label still line up.

// The physical lines for one citizenship: `label` in the first column (padded
// to name_w columns), then security, parent, allies, enemies and the full
// guard path.
private string _render(string label, mapping rec, int name_w)
{
  string * allies, * enemies;
  string parent, guard;
  int sec, rows, i;
  string out;

  sec = intp(rec["security"]) ? rec["security"] : 0;
  parent = (stringp(rec["parent"]) && strlen(rec["parent"])) ? rec["parent"] : "-";
  allies = sizeof(rec["allies"]) ? rec["allies"] : ({ "-" });
  enemies = sizeof(rec["enemies"]) ? rec["enemies"] : ({ "-" });
  guard = (stringp(rec["guard"]) && strlen(rec["guard"])) ? rec["guard"] : "-";

  rows = (sizeof(allies) > sizeof(enemies)) ? sizeof(allies) : sizeof(enemies);
  out = "";
  for (i = 0; i < rows; i++)
  {
    string a, e;
    a = (i < sizeof(allies)) ? allies[i] : "";
    e = (i < sizeof(enemies)) ? enemies[i] : "";
    // the first line carries the name / security / parent / guard; the extra
    // ally / enemy names hang on the lines below with those columns blank
    if (i == 0)
      out += sprintf("  %-*s %4d  %-10s %-12s %-12s %s\n",
                     name_w, label, sec, parent, a, e, guard);
    else
      out += sprintf("  %-*s %4s  %-10s %-12s %-12s\n",
                     name_w, "", "", "", a, e);
  }
  return out;
}

// A table of ({ label, citizenship_name }) rows, with a header. `name_w` is
// sized to the widest label so every column lines up.
private string _table(mixed * label_rows, mapping graph)
{
  int name_w, i;
  string out;

  // widest label by visible width, so the box-drawing tree labels line up too
  name_w = strlen("citizenship", true);
  for (i = 0; i < sizeof(label_rows); i++)
  {
    int d;
    d = strlen(label_rows[i][0], true);
    if (d > name_w)
      name_w = d;
  }

  out = sprintf("  %-*s %4s  %-10s %-12s %-12s %s\n",
                name_w, "citizenship", "sec", "parent", "allies",
                "enemies", "guard");
  // underline: dashes the length of each header word, in the same columns.
  // sprintf("%p%*s", '-', n, "") sets '-' as the pad char and fills an n-wide
  // field with it, i.e. n dashes -- the plugin has no single repeat conversion.
  out += sprintf("  %-*s %4s  %-10s %-12s %-12s %s\n",
                 name_w, sprintf("%p%*s", '-', strlen("citizenship"), ""),
                 sprintf("%p%*s", '-', strlen("sec"), ""),
                 sprintf("%p%*s", '-', strlen("parent"), ""),
                 sprintf("%p%*s", '-', strlen("allies"), ""),
                 sprintf("%p%*s", '-', strlen("enemies"), ""),
                 sprintf("%p%*s", '-', strlen("guard"), ""));
  for (i = 0; i < sizeof(label_rows); i++)
    out += _render(label_rows[i][0], graph[label_rows[i][1]], name_w);
  return out;
}

// Detailed multi-line view of one citizenship (single view / edit confirmation).
private string _detail(string name, mapping rec)
{
  string ret;
  string * allies, * enemies;

  ret = sprintf("  %-12s security %d\n", name,
                intp(rec["security"]) ? rec["security"] : 0);
  if (stringp(rec["parent"]) && strlen(rec["parent"]))
    ret += sprintf("      %-9s %s\n", "parent:", rec["parent"]);
  if (stringp(rec["guard"]) && strlen(rec["guard"]))
    ret += sprintf("      %-9s %s\n", "guard:", rec["guard"]);
  if (pointerp(rec[DIPLOMACY_DEITIES]) && sizeof(rec[DIPLOMACY_DEITIES]))
    ret += sprintf("      %-9s %s\n", "deities:",
                   implode(rec[DIPLOMACY_DEITIES], ", "));

  allies = pointerp(rec["allies"]) ? rec["allies"] : ({ });
  enemies = pointerp(rec["enemies"]) ? rec["enemies"] : ({ });
  ret += sprintf("      %-9s %s\n", "allies:",
                 sizeof(allies) ? implode(allies, ", ") : "-");
  ret += sprintf("      %-9s %s\n", "enemies:",
                 sizeof(enemies) ? implode(enemies, ", ") : "-");
  return ret;
}

// --- tree ------------------------------------------------------------------

// parent name -> the citizenships whose parent is that name. Names with no
// parent (or a parent that is not itself a citizenship) are the roots.
private mapping _children_by_parent(mapping graph)
{
  mapping children;
  string * names;
  int i;

  children = ([ ]);
  names = map_indices(graph);
  for (i = 0; i < sizeof(names); i++)
  {
    string parent;
    parent = graph[names[i]]["parent"];
    if (stringp(parent) && strlen(parent) && graph[parent])
      children[parent] = (children[parent] ? children[parent] : ({ })) +
                         ({ names[i] });
  }
  return children;
}

private string * _roots(mapping graph)
{
  string * names, * roots;
  int i;

  names = map_indices(graph);
  roots = ({ });
  for (i = 0; i < sizeof(names); i++)
  {
    string parent;
    parent = graph[names[i]]["parent"];
    if (!stringp(parent) || !strlen(parent) || !graph[parent])
      roots += ({ names[i] });
  }
  return roots;
}

// ({ label, name }) rows for `name`'s descendants, each name hung under
// box-drawing tree lines: the last child gets "└── ", the rest "├── ", with
// "│   " / "    " carried down for deeper levels. `seen` guards against a cycle
// in the parent chain.
private mixed * _tree_rows(mapping graph, mapping children, string name,
                           string prefix, mapping seen)
{
  mixed * rows;
  string * kids;
  int i;

  rows = ({ });
  kids = children[name] ? children[name] : ({ });
  for (i = 0; i < sizeof(kids); i++)
  {
    string child, connector, child_prefix;
    int last;

    child = kids[i];
    if (seen[child])
      continue;                       // already drawn -- broken parent cycle
    seen[child] = 1;

    last = (i == sizeof(kids) - 1);
    connector = last ? "└── " : "├── ";
    rows += ({ ({ prefix + connector + child, child }) });

    child_prefix = prefix + (last ? "    " : "│   ");
    rows += _tree_rows(graph, children, child, child_prefix, seen);
  }
  return rows;
}

// Every citizenship as ({ label, name }) in tree order: each root first, then
// its descendants hung beneath it.
private mixed * _tree_label_rows(mapping graph)
{
  mapping children, seen;
  string * roots;
  mixed * rows;
  int i;

  children = _children_by_parent(graph);
  roots = _roots(graph);
  seen = ([ ]);
  rows = ({ });
  for (i = 0; i < sizeof(roots); i++)
  {
    seen[roots[i]] = 1;
    rows += ({ ({ roots[i], roots[i] }) });
    rows += _tree_rows(graph, children, roots[i], "", seen);
  }
  return rows;
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

  // the graph belongs to the game under our feet, so ask for that one
  h = handler("diplomacy", me);
  words = (str && strlen(str)) ? explode(str, " ") - ({ "" }) : ({ });

  // --- listing (columns) -------------------------------------------------
  if (!sizeof(words))
  {
    string * names;
    mixed * label_rows;
    int i;

    g = h->query_relations();
    if (!mappingp(g) || !map_sizeof(g))
    {
      write("Game '" + game + "' has no diplomacy defined yet. " +
            "Build it with 'diplomacy add <citizenship>'.\n");
      return 1;
    }

    names = map_indices(g);
    label_rows = ({ });
    for (i = 0; i < sizeof(names); i++)
      label_rows += ({ ({ names[i], names[i] }) });

    write("Diplomacy graph for game '" + game + "' (" + map_sizeof(g) +
          " citizenship" + (map_sizeof(g) == 1 ? "" : "s") + "):\n\n" +
          _table(label_rows, g));
    return 1;
  }

  // --- tree (same columns, name column hung under ASCII lines) ------------
  if (words[0] == "tree")
  {
    g = h->query_relations();
    if (!mappingp(g) || !map_sizeof(g))
    {
      write("Game '" + game + "' has no diplomacy defined yet.\n");
      return 1;
    }
    write("Diplomacy tree for game '" + game + "':\n\n" +
          _table(_tree_label_rows(g), g));
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
    h->add_citizenship(words[1]);
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
    if (!h->remove_citizenship(words[1]))
    {
      notify_fail("No citizenship '" + words[1] + "' in game '" + game + "'.\n");
      return 0;
    }
    write("Removed citizenship '" + words[1] + "'.\n");
    return 1;
  }

  // --- from here, words[0] is a citizenship name -------------------------
  g = h->query_relations();
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
          _detail(words[0], g[words[0]]));
    return 1;
  }

  // editing: diplomacy <cit> <field> <value>
  if (sizeof(words) < 3)
  {
    notify_fail("Usage: diplomacy <cit> <parent|security|guard|deity|" +
                "undeity|ally|unally|enemy|unenemy> <value>\n");
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
        h->set_parent(cit, (value == "none") ? "" : value);
        break;
      case "security":
        h->set_security(cit, to_int(value));
        break;
      case "guard":
        h->set_guard(cit, (value == "none") ? "" : value);
        break;
      case "deity":
        h->add_deity(cit, value);
        break;
      case "undeity":
        h->remove_deity(cit, value);
        break;
      case "ally":
        h->add_relationship(DIPLOMACY_RELATION_ALLY, cit, value);
        break;
      case "unally":
        h->remove_relationship(DIPLOMACY_RELATION_ALLY, cit, value);
        break;
      case "enemy":
        h->add_relationship(DIPLOMACY_RELATION_ENEMY, cit, value);
        break;
      case "unenemy":
        h->remove_relationship(DIPLOMACY_RELATION_ENEMY, cit, value);
        break;
      default:
        notify_fail("Unknown field '" + field + "'. Use parent, security, " +
                    "guard, deity, undeity, ally, unally, enemy or " +
                    "unenemy.\n");
        return 0;
    }
  }

  // reflect the new state back
  g = h->query_relations();
  write("Updated '" + words[0] + "':\n" + _detail(words[0], g[words[0]]));
  return 1;
}
