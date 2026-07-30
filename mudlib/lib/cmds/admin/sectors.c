
#include <mud/cmd.h>
#include <maps/maps.h>
#include <maps/sector.h>

inherit CMD_BASE;

#define R   "%^BOLD%^RED%^"
#define Y   "%^BOLD%^YELLOW%^"
#define G   "%^BOLD%^GREEN%^"
#define RE  "%^RESET%^"

void setup()
{
  set_aliases(({ "sectors" }));
  set_usage("sectors\n" +
            "sectors settype <type>\n" +
            "sectors settype <sx> <sy> <sz> <type> [<map>]\n" +
            "sectors verify <sx> <sy> <sz> [<map>]\n" +
            "sectors purge [<map>]");
  set_help(
    "List the sectors loaded in the map system, set a sector's manual\n" +
    "type, audit a sector, or purge stale index entries.\n" +
    "\n" +
    "  sectors                             list loaded sectors\n" +
    "  sectors settype <type>              set the type of the sector you are in\n" +
    "  sectors settype <sx> <sy> <sz> <t>  set the sector at index (sx, sy, sz)\n" +
    "  sectors verify <sx> <sy> <sz>       report drift in one sector (read-only)\n" +
    "  sectors purge                       drop the ghost positions it finds\n" +
    "\n" +
    "Coordinates are SECTOR indices (the ones shown by look / glance), not\n" +
    "location coordinates. The map defaults to the one you are standing in;\n" +
    "pass an explicit <map> as the last argument to target another one.\n" +
    "Type is one of: city, forest, coast, underground, empty, none. A manual\n" +
    "type only shows on the world map when the sector has no locations of its\n" +
    "own; 'empty' and 'none' draw nothing. The coords form creates the\n" +
    "sector.o if it does not exist yet, so a virgin part of the map can be\n" +
    "painted.\n");
}

private int _valid_type(string type)
{
  return member_array(type, SECTOR_MAP_COMPONENTS) != -1 ||
         type == SECTOR_TYPE_EMPTY || type == SECTOR_TYPE_NONE ||
         type == "none";
}

private string _canon_type(string type)
{
  return type == "none" ? SECTOR_TYPE_NONE : type;
}

// The map to operate on: an explicit name when the caller gave one, else the
// map of the location the player is standing in (falling back to "default"
// when they are not in a location).
private string _resolve_map(object me, string explicit)
{
  object env;

  if (explicit && strlen(explicit))
    return explicit;

  env = environment(me);
  return (env && env->query_location()) ? env->query_map_name() : "default";
}

// ===== verify: read-only drift audit of one sector =====
private int do_verify(object me, string * tokens)
{
  string game, map_name;
  int x, y, z;
  mapping report;
  mixed * stale;
  string * orphans;
  int i;

  if (sizeof(tokens) != 4 && sizeof(tokens) != 5)
  {
    notify_fail("Usage: sectors verify <sx> <sy> <sz> [<map>]\n");
    return 0;
  }

  if (sscanf(tokens[1], "%d", x) != 1 ||
      sscanf(tokens[2], "%d", y) != 1 ||
      sscanf(tokens[3], "%d", z) != 1)
  {
    notify_fail("Coordinates must be integers.\n");
    return 0;
  }

  game = game_name(me);
  if (!strlen(game))
  {
    notify_fail("Cannot infer game from your current environment. " +
                "Stand inside a game first.\n");
    return 0;
  }
  map_name = _resolve_map(me, sizeof(tokens) == 5 ? tokens[4] : nil);

  report = load_object(MAPS_HANDLER)->verify_sector(game, map_name, x, y, z);
  if (!report)
  {
    write("No sector.o at (" + x + "," + y + "," + z + ") in map '" +
          map_name + "'.\n");
    return 1;
  }

  stale = report["stale"];
  orphans = report["orphans"];

  write("Sector (" + x + "," + y + "," + z + ") in " + game + "/" + map_name +
        ": " + report["total"] + " indexed position" +
        (report["total"] == 1 ? "" : "s") + ".\n");

  if (!sizeof(stale) && !sizeof(orphans))
  {
    write(" " + G + "Clean" + RE + ": no drift.\n");
    return 1;
  }

  for (i = 0; i < sizeof(stale); i++)
    write(" " + R + "!" + RE + " " + stale[i][0] + " -> " + stale[i][1] +
          "  (" + stale[i][2] + ")\n");

  for (i = 0; i < sizeof(orphans); i++)
    write(" " + Y + "?" + RE + " orphan pointer file " + orphans[i] +
          ".o (no index entry)\n");

  write(" " + sizeof(stale) + " stale, " + sizeof(orphans) +
        " orphan. Run 'sectors purge" +
        (sizeof(tokens) == 5 ? " " + map_name : "") + "' to clean.\n");
  return 1;
}

static int cmd(string str, object me, string verb)
{
  string * tokens;
  mapping sectors;
  string * keys;
  int i, j;

  tokens = explode(str ? str : "", " ") - ({ "" });

  // ===== verify =====
  if (sizeof(tokens) && tokens[0] == "verify")
    return do_verify(me, tokens);

  // ===== settype =====
  if (sizeof(tokens) && tokens[0] == "settype")
  {
    string game, map_name, type;
    int x, y, z;
    object env, sector;

    game = game_name(me);
    if (!strlen(game))
    {
      notify_fail("Cannot infer game from your current environment. " +
                  "Stand inside a game first.\n");
      return 0;
    }

    // settype <type>  -> the sector we stand in. Convert the location's
    // world coordinates into the sector index the map system keys on.
    if (sizeof(tokens) == 2)
    {
      int * coords;
      env = environment(me);
      if (!env || !env->query_location() || !env->query_coordinates())
      {
        notify_fail("You are not standing in a location with coordinates. " +
                    "Use 'sectors settype <sx> <sy> <sz> <type>'.\n");
        return 0;
      }
      coords = env->query_coordinates();
      x = coords[0] / 10 - (coords[0] < 0);
      y = coords[1] / 10 - (coords[1] < 0);
      z = coords[2] / 10 - (coords[2] < 0);
      map_name = env->query_map_name();
      type = tokens[1];
    }
    // settype <sx> <sy> <sz> <type> [<map>]  -> sector indices, used directly
    else if (sizeof(tokens) == 5 || sizeof(tokens) == 6)
    {
      if (sscanf(tokens[1], "%d", x) != 1 ||
          sscanf(tokens[2], "%d", y) != 1 ||
          sscanf(tokens[3], "%d", z) != 1)
      {
        notify_fail("Coordinates must be integers.\n");
        return 0;
      }
      type = tokens[4];
      map_name = _resolve_map(me, sizeof(tokens) == 6 ? tokens[5] : nil);
    }
    else
    {
      notify_fail("Usage: sectors settype [<sx> <sy> <sz>] <type> [<map>]\n");
      return 0;
    }

    if (!_valid_type(type))
    {
      notify_fail("Unknown type '" + type + "'. One of: city, forest, " +
                  "coast, underground, empty, none.\n");
      return 0;
    }

    sector = load_object(MAPS_HANDLER)->set_sector_manual_type(
               game, map_name, x, y, z, _canon_type(type));

    if (!sector)
    {
      notify_fail("Could not resolve a sector for that coordinate.\n");
      return 0;
    }

    write("Set manual type of sector (" + x + "," + y + "," + z +
          ") in map '" + map_name + "' to '" +
          (strlen(_canon_type(type)) ? _canon_type(type) : "none") + "'.\n");
    return 1;
  }

  // ===== purge (ghost / drift cleanup) =====
  if (sizeof(tokens) && tokens[0] == "purge")
  {
    string game, map_name;
    int removed;

    game = game_name(me);
    if (!strlen(game))
    {
      notify_fail("Cannot infer game from your current environment. " +
                  "Stand inside a game first.\n");
      return 0;
    }

    map_name = _resolve_map(me, sizeof(tokens) >= 2 ? tokens[1] : nil);

    removed = load_object(MAPS_HANDLER)->purge_drift(game, map_name);
    write("Purged " + removed + " stale position" +
          (removed == 1 ? "" : "s") + " from map '" + map_name + "'.\n");
    return 1;
  }

  // ===== list =====
  sectors = load_object(MAPS_HANDLER)->query_loaded_sectors();

  if (!sectors || !map_sizeof(sectors))
  {
    write("No sectors loaded.\n");
    return 1;
  }

  write("Sectors loaded:\n");
  keys = keys(sectors);

  for (i = 0; i < sizeof(keys); i++)
  {
    object * locations;
    string manual;

    manual = sectors[keys[i]]->query_manual_type();

    write(" - %^GREEN%^" + keys[i] + "%^RESET%^" +
          (strlen(manual) ? "  (manual: " + manual + ")" : "") + "\n");

    locations = sectors[keys[i]]->query_loaded_locations();

    for (j = 0; j < sizeof(locations); j++)
      write("   - " + locations[j]->query_file_name() + "\n");
  }

  return 1;
}
