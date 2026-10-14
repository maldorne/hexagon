
#include <mud/cmd.h>
#include <areas/area.h>
#include <room/location.h>

inherit CMD_BASE;

private int do_reindex(object me, string * tokens);
private string describe_tree(mapping areas, mapping children, string key,
                             int depth, int width, int show_locations,
                             int show_connections);

void setup()
{
  set_aliases(({ "areas" }));
  set_usage("areas [-c] [locations] [connections]\n" +
            "      areas reindex [ <area> | game ] [save]");
  set_help("List areas loaded in the location system.\n" +
           "Areas are listed as a tree: every area that stands on its own, with\n" +
           "the areas that are part of it indented below. The last column is the\n" +
           "NPC level band (level+/-spread); one in parentheses is inherited from\n" +
           "a parent area instead of set on the area itself.\n" +
           "If '-c' is specified, it will only show your current area.\n" +
           "If 'locations' is specified, it will also list the locations of every area.\n" +
           "If 'connections' is specified, it will also list their connections to other areas.\n" +
           "\n" +
           "'areas reindex' reloads an area's persisted locations so the\n" +
           "indices built from them -- the sector map, the terrain tallies,\n" +
           "the ways the world map draws -- are rebuilt against the current\n" +
           "code. Run it after changing how any of that is calculated.\n" +
           "\n" +
           "  areas reindex            the area you are standing in\n" +
           "  areas reindex <area>     the one you name\n" +
           "  areas reindex game       every area of this game, one per tick\n" +
           "\n" +
           "Reading only: it loads locations, it does not write them. Adding\n" +
           "'save' as the last word re-saves every location as well, which\n" +
           "migrates the files on disk to the current format. That one does\n" +
           "rewrite every .o, so ask for it only when a migration is due.\n");
}

// ===== areas reindex =====
private int do_reindex(object me, string * tokens)
{
  string game, area;
  int count, areas, do_save;

  game = game_name(me);
  if (!strlen(game))
  {
    notify_fail("Cannot infer game from your current environment. " +
                "Stand inside a game first.\n");
    return 0;
  }

  // the save flag may sit anywhere after the verb
  do_save = 0;
  if (member_array("save", tokens) != -1)
  {
    do_save = 1;
    tokens -= ({ "save" });
  }

  // every area of the game, chunked across ticks so the batch stays inside
  // the driver's per-execution limits
  if (sizeof(tokens) == 1 && tokens[0] == "game")
  {
    areas = load_object(LOCATION_HANDLER)->reindex_game(game, me, do_save);
    if (areas < 0)
    {
      notify_fail("Game '" + game + "' has no areas directory.\n");
      return 0;
    }
    write("Reindexing " + areas + " area" + (areas == 1 ? "" : "s") +
          " of '" + game + "'" + (do_save ? " (with save)" : "") +
          " in the background ...\n");
    return 1;
  }

  if (sizeof(tokens) == 1)
    area = tokens[0];
  else if (!sizeof(tokens))
  {
    object env, area_ob;

    env = environment(me);
    if (!env || !env->query_location())
    {
      notify_fail("You are not standing in a location. Name the area: " +
                  "'areas reindex <area>'.\n");
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
    notify_fail("Usage: areas reindex [ <area> | game ] [save]\n");
    return 0;
  }

  count = load_object(LOCATION_HANDLER)->reindex_area(game, area, do_save);
  if (count < 0)
  {
    notify_fail("No area '" + area + "' in game '" + game + "'.\n");
    return 0;
  }

  write("Reindexed area '" + area + "' in '" + game + "': " +
        count + " location" + (count == 1 ? "" : "s") +
        (do_save ? " (saved)" : "") + ".\n");
  return 1;
}

// One area's entry and, indented below it, its child areas.
private string describe_tree(mapping areas, mapping children, string key,
                             int depth, int width, int show_locations,
                             int show_connections)
{
  object area, source;
  mapping connections;
  object * locations;
  string * connections_keys, * kids;
  string ret, indent, level;
  int i, gap;

  area = areas[key];
  indent = depth ? pad("", depth * 3) : "";

  // the band, in parentheses when it comes from a parent
  source = (object)area->query_area_level_source();
  level = area->query_area_level() + "+/-" + area->query_area_spread();
  if (source != area)
    level = "(" + level + ")";

  gap = width - strlen(key) - strlen(indent);
  ret = indent + " - %^GREEN%^" + key + "%^RESET%^" +
        pad("", gap < 1 ? 1 : gap) + level + "\n";

  if (show_locations)
  {
    ret += "\n" + indent + "  Locations:\n\n";
    locations = area->query_loaded_locations();

    for (i = 0; i < sizeof(locations); i++)
      ret += indent + "   - " + locations[i]->query_file_name() + "\n";

    ret += "\n";
  }

  if (show_connections)
  {
    ret += "\n" + indent + "  Connections:\n\n";

    connections = area->query_connections();
    connections_keys = keys(connections);

    for (i = 0; i < sizeof(connections_keys); i++)
      ret += indent + "   - " + connections_keys[i] + "\n" + indent +
             "     %^CYAN%^" + connections[connections_keys[i]][0] +
             "%^RESET%^ : " + connections[connections_keys[i]][1] + "\n";

    ret += "\n";
  }

  kids = children[key];
  for (i = 0; kids && i < sizeof(kids); i++)
    ret += describe_tree(areas, children, kids[i], depth + 1, width,
                         show_locations, show_connections);

  return ret;
}

static int cmd(string str, object me, string verb) 
{
  mapping areas, children;
  string * keys, * roots;
  string parent_key;
  object parent;
  int i, width;
  string * pieces;
  int show_locations, show_connections, only_current;
  object current_area;
  string ret;

  pieces = (str && strlen(str)) ? explode(str, " ") - ({ "" }) : ({ });
  if (sizeof(pieces) && pieces[0] == "reindex")
    return do_reindex(me, pieces[1 ..]);

  areas = load_object(AREA_HANDLER)->query_loaded_areas();

  if (!areas || !map_sizeof(areas))
  {
    write("No areas loaded.\n");
    return 1;
  }

  show_locations = FALSE;
  show_connections = FALSE;
  only_current = FALSE;
  current_area = nil;

  if (member_array("locations", pieces) != -1)
    show_locations = TRUE;
  if (member_array("connections", pieces) != -1)
    show_connections = TRUE;
  if (member_array("-c", pieces) != -1)
  {
    current_area = environment(this_player())->query_area();
    only_current = TRUE;
  }

  keys = sort_array(keys(areas));
  children = ([ ]);
  roots = ({ });
  width = 0;

  // an area whose parent is not loaded is listed as a root
  for (i = 0; i < sizeof(keys); i++)
  {
    parent = (object)areas[keys[i]]->query_parent_area();
    parent_key = parent ? (string)parent->query_area_path() : nil;

    if (parent_key && parent_key != keys[i] && areas[parent_key])
    {
      if (!children[parent_key])
        children[parent_key] = ({ });
      children[parent_key] += ({ keys[i] });
    }
    else
      roots += ({ keys[i] });

    if (strlen(keys[i]) + 12 > width)
      width = strlen(keys[i]) + 12;
  }

  ret = "Areas loaded (level band; in parentheses: inherited):\n";

  for (i = 0; i < sizeof(roots); i++)
  {
    ret += describe_tree(areas, children, roots[i], 0, width, show_locations,
                         show_connections);
  }

  if (only_current)
  {
    // the registry is keyed by area path, so compare the objects: the current
    // area's display name never matches a key
    ret = "Areas loaded (level band; in parentheses: inherited):\n";
    keys = keys(areas);
    for (i = 0; i < sizeof(keys); i++)
      if (areas[keys[i]] == current_area)
        ret += describe_tree(areas, ([ ]), keys[i], 0, width, show_locations,
                             show_connections);
  }

  write(ret);
  return 1;
}
