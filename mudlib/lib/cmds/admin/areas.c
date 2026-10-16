
#include <mud/cmd.h>
#include <areas/area.h>
#include <room/location.h>

inherit LOCATION_CMD;

private int do_reindex(object me, string * tokens);
private int do_state(object area, string * args);
private int do_exploration(object area, string * args);
private int do_noexploration(object area);
private int do_level(object area, string * args);
private int do_stats(object area, string * args);
private int do_diplomacy(object area, string * args);
private int do_relevel(object area);
private string parent_report(object area, mapping plan, string path, int check);
private int do_parent(object area, string * args);
private int do_principal(object area, object loc);
private int do_set(object me, string * args);
private string describe_tree(mapping areas, mapping children, string key,
                             int depth, int width, int show_locations,
                             int show_connections);

void setup()
{
  set_aliases(({ "areas" }));
  set_usage("areas [-c] [locations] [connections]\n" +
            "      areas reindex [ <area> | game ] [save]\n" +
            "      areas < exploration <display name> | noexploration\n" +
            "            | level <n> [<spread>] | level none\n" +
            "            | stats <low> <high> | stats none\n" +
            "            | diplomacy <citizenship|none|inherit>\n" +
            "            | parent <area path|none> [check] | principal\n" +
            "            | state [draft|settled] | relevel >");
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
           "rewrite every .o, so ask for it only when a migration is due.\n" +
           "\n" +
           "The rest set the metadata of the area you are standing in (a\n" +
           "location, not a plain room):\n" +
           "\n" +
           "  areas exploration <name>    walking in it grants exploration, under that name\n" +
           "  areas noexploration         it grants none any more\n" +
           "  areas level <n> [<spread>]  the average level of its NPCs; the spread is\n" +
           "                              left as it was when omitted\n" +
           "  areas level none            drop its own band, take its parent's\n" +
           "  areas stats <low> <high>    the range its people roll each of their stats in\n" +
           "  areas stats none            roll none of its own: they keep their type's\n" +
           "  areas diplomacy <name>      the citizenship the area belongs to\n" +
           "  areas diplomacy none        it belongs to nobody (a road between two towns)\n" +
           "  areas diplomacy inherit     drop its own, back to its parent's\n" +
           "  areas parent <path>         make it part of another area\n" +
           "  areas parent none           detach it, to stand on its own\n" +
           "  areas parent <path> check   only report what that would do\n" +
           "  areas principal             this location is the area's fallback, where\n" +
           "                              orphaned occupants go when a location is destroyed\n" +
           "  areas state                 say how far along the area is\n" +
           "  areas state draft|settled   set it\n" +
           "  areas relevel               bring its NPCs up to the current level band\n" +
           "\n" +
           "An area without a band, stats or citizenship of its own takes its\n" +
           "parent's. An area is a 'draft' while its map can still be wiped and\n" +
           "rebuilt, and 'settled' once it is final; the state gates nothing but\n" +
           "that wipe, and moving back to draft says what a wipe would take with\n" +
           "it. Changing the parent moves the community's books (roster, census,\n" +
           "jobs, houses) between the two roots, so it is refused while anything\n" +
           "would break or be lost; 'check' only reports. It is always set by\n" +
           "hand: a wilderness under a town's folder is not its suburb. An NPC\n" +
           "keeps the level it was born with, so 'relevel' is what carries the\n" +
           "people born under an old band up to a new one.\n");
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

// Width of the level band column; the band is centred in it.
#define BAND_WIDTH 12

// `text` centred in `width` columns.
private string centre(string text, int width)
{
  int left;

  left = (width - strlen(text)) / 2;
  if (left < 0)
    left = 0;
  return pad("", left) + text + pad("", width - left - strlen(text));
}

// The header of the listing: one column for the area, one for its band.
private string header(int width)
{
  string line;

  line = " Area" + pad("", width - 2) + centre("Level band", BAND_WIDTH) + "\n";
  return line + " " + pad("", width + BAND_WIDTH + 1, "-") + "\n";
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
        pad("", gap < 1 ? 1 : gap) + centre(level, BAND_WIDTH) + "\n";

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

// ===== areas state [draft|settled] =====
// How far along the area is. Moving it back to draft is allowed -- sometimes a
// place really has to be rebuilt -- but it is the moment to say out loud what
// a wipe would take with it, because that is the only thing the state gates.
private int do_state(object area, string * args)
{
  mapping census;
  int people, families;

  if (sizeof(args) < 2)
  {
    write("Area '" + area->query_area_name() + "' is " +
          area->query_area_state() + ".\n");
    return 1;
  }

  if (args[1] != AREA_DRAFT && args[1] != AREA_SETTLED)
  {
    notify_fail("An area is '" + AREA_DRAFT + "' or '" + AREA_SETTLED +
                "'.\n");
    return 0;
  }

  census = (mapping)area->query_npc_census();
  people = map_sizeof(census);
  families = sizeof((string *)handler("families", this_player())->families_of_area(
                      (string)area->query_area_path()));

  area->set_area_state(args[1]);

  write("Area '" + area->query_area_name() + "' is now " + args[1] + ".\n");
  if (args[1] == AREA_DRAFT && (people || families))
    write("It holds " + people + " person" + (people == 1 ? "" : "s") +
          " and " + families + " famil" + (families == 1 ? "y" : "ies") +
          ". A draft may be wiped, and a wipe leaves their records naming " +
          "things that no longer exist.\n");
  return 1;
}

// ===== areas exploration <display name> =====
private int do_exploration(object area, string * args)
{
  string name;

  if (sizeof(args) < 2)
  {
    notify_fail("Usage: areas exploration <display name>\n");
    return 0;
  }

  name = rest_of(args, 1);
  area->set_gives_exploration(1);
  area->set_exploration_name(name);
  write("Area '" + area->query_area_name() + "' now grants exploration: \"" +
        name + "\".\n");
  return 1;
}

// ===== areas noexploration =====
private int do_noexploration(object area)
{
  area->set_gives_exploration(0);
  area->set_exploration_name("");
  write("Area '" + area->query_area_name() +
        "' no longer grants exploration.\n");
  return 1;
}

// ===== areas level <n> [<spread>] | none =====
private int do_level(object area, string * args)
{
  int lvl, spread;

  // level is required; spread is optional and left unchanged when omitted
  if (sizeof(args) >= 2 && (args[1] == "none" || args[1] == "inherit"))
    args[1] = "0";

  if (sizeof(args) < 2 || sscanf(args[1], "%d", lvl) != 1)
  {
    notify_fail("Usage: areas level <n> [<spread>] | none\n");
    return 0;
  }

  // 0 clears the area's own band so it takes its parent's
  if (lvl < 1)
  {
    area->set_area_level(0);
    area->set_area_spread(0);
    write("Area '" + area->query_area_name() + "' has no level of its own; " +
          "it takes " + area->query_area_level() + " (spread " +
          area->query_area_spread() + ") from its parents.\n");
    return 1;
  }

  spread = area->query_own_area_spread();
  if (sizeof(args) > 2)
    sscanf(args[2], "%d", spread);

  area->set_area_level(lvl);
  area->set_area_spread(spread);
  write("Area '" + area->query_area_name() + "' average level set to " +
        area->query_own_area_level() + " (spread " +
        area->query_own_area_spread() + ").\n");
  return 1;
}

// ===== areas stats <low> <high> | none =====
// How strong the people of this place are, as the range their eight stats are
// rolled in. Belongs to the place: the same race makes a weak village and a
// hard stronghold.
private int do_stats(object area, string * args)
{
  int low, high;
  int * band;

  if (sizeof(args) > 1 && args[1] == "none")
  {
    area->set_area_stats(0, 0);
    write("Area '" + area->query_area_name() + "' rolls no stats of its " +
          "own; its people keep whatever their type carries.\n");
    return 1;
  }

  if (sizeof(args) < 3 || sscanf(args[1], "%d", low) != 1 ||
      sscanf(args[2], "%d", high) != 1)
  {
    notify_fail("Usage: areas stats <low> <high>, or areas stats none\n");
    return 0;
  }

  if (low < 1 || high < low)
  {
    notify_fail("The range runs from a low of at least 1 up to a high no " +
                "smaller than it.\n");
    return 0;
  }

  area->set_area_stats(low, high);
  band = (int *)area->query_area_stats();
  write("People of '" + area->query_area_name() + "' now roll each stat " +
        "in " + band[0] + "-" + band[1] + ".\n");
  return 1;
}

// ===== areas diplomacy <citizenship|none|inherit> =====
// The citizenship this area belongs to. Without one of its own an area takes
// its parent's; "none" says it belongs to nobody (a road between two towns),
// "inherit" drops its own and goes back to its parent's.
private int do_diplomacy(object area, string * args)
{
  string name;

  if (sizeof(args) < 2)
  {
    notify_fail("Usage: areas diplomacy <citizenship|none|inherit>\n");
    return 0;
  }

  if (args[1] == "none")
  {
    area->set_stateless(1);
    write("Area '" + area->query_area_name() + "' now belongs to no " +
          "citizenship, not even its parent's.\n");
  }
  else if (args[1] == "inherit")
  {
    area->set_citizenship("");
    area->set_stateless(0);
    name = area->query_effective_citizenship();
    write("Area '" + area->query_area_name() + "' has no citizenship of " +
          "its own; it belongs to " +
          ((name && strlen(name)) ? "'" + name + "', its parent's" : "none") +
          ".\n");
  }
  else
  {
    // only a citizenship the diplomacy graph knows: one typed wrong would
    // leave the area answering to nobody without saying so
    if (member_array(args[1],
          (string *)handler("diplomacy", area)->query_citizenships()) == -1)
    {
      notify_fail("No citizenship '" + args[1] + "' in this game's " +
                  "diplomacy ('diplomacy' lists them).\n");
      return 0;
    }
    area->set_citizenship(args[1]);
    write("Area '" + area->query_area_name() + "' now belongs to " +
          "citizenship '" + args[1] + "'.\n");
  }
  return 1;
}

// ===== areas relevel =====
// An NPC keeps the level it was born with, so raising an area's band leaves
// the people born under the old one behind. This brings them up to it.
private int do_relevel(object area)
{
  int touched;

  touched = (int)area->relevel_census();
  write("Re-levelled " + touched + " NPC(s) of '" + area->query_area_name() +
        "' to the current band (" + area->query_area_level() + " +/- " +
        area->query_area_spread() + ").\n");
  return 1;
}

/**
 * The text that tells what a parent change did, or would do: the conflicts that
 * stop it, the warnings it leaves, and the outcome.
 */
private string parent_report(object area, mapping plan, string path, int check)
{
  string out;
  int i;

  out = "";
  for (i = 0; i < sizeof(plan["conflicts"]); i++)
    out += "  - " + plan["conflicts"][i] + "\n";
  if (strlen(out))
    out = "It cannot be done yet:\n" + out;
  for (i = 0; i < sizeof(plan["warnings"]); i++)
    out += (i ? "" : "Warnings:\n") + "  - " + plan["warnings"][i] + "\n";

  if (plan["done"])
    out = (strlen(path)
            ? "Area '" + area->query_area_name() + "' is now part of '" +
              AREA_HANDLER->query_area(path)->query_area_name() + "'.\n"
            : "Area '" + area->query_area_name() + "' stands on its own.\n") +
          out;
  else if (!sizeof(plan["conflicts"]))
    out = (check ? (plan["mode"] == "none" ? "Nothing to change.\n"
                                           : "It can be done.\n")
                 : "Nothing to change.\n") + out;

  return out;
}

// ===== areas parent <area path|none> [check] =====
// What this area is part of. Everything shared down the chain is resolved
// through the link -- today the community (roster, census, jobs, houses),
// whatever comes next tomorrow. "none" detaches it. Always set by hand: a
// wilderness that happens to sit under a town's folder is not its suburb.
// Changing it moves the community's books between the two roots, so it is
// refused while anything would break or be lost; "check" only reports.
private int do_parent(object area, string * args)
{
  mapping plan;
  string path;
  int check;

  if (sizeof(args) < 2 || sizeof(args) > 3 ||
      (sizeof(args) == 3 && args[2] != "check"))
  {
    notify_fail("Usage: areas parent <area path|none> [check]\n");
    return 0;
  }

  path = (args[1] == "none") ? "" : args[1];
  check = sizeof(args) > 2;

  plan = check ? area->plan_parent_change(path) : area->change_parent(path);

  write(parent_report(area, plan, path, check));
  return 1;
}

// ===== areas principal =====
// Mark this location as the area's fallback (where orphaned occupants go when
// a location is destroyed, e.g. deleting an occupied plot).
private int do_principal(object area, object loc)
{
  area->set_principal(loc->query_file_name());
  write("Area '" + area->query_area_name() + "' principal location is now " +
        loc->query_file_name() + ".\n");
  return 1;
}

// The words that set the metadata of the area the admin stands in.
#define SET_VERBS ({ "exploration", "noexploration", "level", "stats", \
                     "diplomacy", "parent", "principal", "state", "relevel" })

// Dispatches a metadata verb, once the area under the admin's feet is known.
private int do_set(object me, string * args)
{
  object loc, area;
  string verb;

  loc = location_of(me);
  if (!loc)
  {
    notify_fail("Stand in a location (not a plain room) to set area " +
                "metadata.\n");
    return 0;
  }

  area = loc->query_area();
  if (!area)
  {
    notify_fail("This location has no area.\n");
    return 0;
  }

  verb = args[0];
  if (verb == "state")
    return do_state(area, args);
  if (verb == "exploration")
    return do_exploration(area, args);
  if (verb == "noexploration")
    return do_noexploration(area);
  if (verb == "level")
    return do_level(area, args);
  if (verb == "stats")
    return do_stats(area, args);
  if (verb == "diplomacy")
    return do_diplomacy(area, args);
  if (verb == "parent")
    return do_parent(area, args);
  if (verb == "principal")
    return do_principal(area, loc);
  return do_relevel(area);
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

  pieces = words(str);
  if (sizeof(pieces) && pieces[0] == "reindex")
    return do_reindex(me, pieces[1 ..]);
  if (sizeof(pieces) && member_array(pieces[0], SET_VERBS) >= 0)
    return do_set(me, pieces);

  // the listing takes only its own flags; anything else was meant as a command
  if (sizeof(pieces - ({ "-c", "locations", "connections" })))
  {
    notify_fail("Usage: " + query_usage() + "\n");
    return 0;
  }

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

  ret = header(width);

  for (i = 0; i < sizeof(roots); i++)
  {
    ret += describe_tree(areas, children, roots[i], 0, width, show_locations,
                         show_connections);
  }

  if (only_current)
  {
    // the registry is keyed by area path, so compare the objects: the current
    // area's display name never matches a key
    ret = header(width);
    keys = keys(areas);
    for (i = 0; i < sizeof(keys); i++)
      if (areas[keys[i]] == current_area)
        ret += describe_tree(areas, ([ ]), keys[i], 0, width, show_locations,
                             show_connections);
  }

  write(ret + " (a band in parentheses is inherited from a parent area)\n");
  return 1;
}
