
#include <mud/cmd.h>
#include <maps/maps.h>
#include <maps/sector.h>

inherit CMD_BASE;

void setup()
{
  set_aliases(({ "sectors" }));
  set_usage("sectors\n" +
            "sectors settype <type>\n" +
            "sectors settype <x> <y> <z> <type>");
  set_help(
    "List the sectors loaded in the map system, or set a sector's manual\n" +
    "type.\n" +
    "\n" +
    "  sectors                        list loaded sectors\n" +
    "  sectors settype <type>         set the type of the sector you are in\n" +
    "  sectors settype <x> <y> <z> <t>  set the sector containing world\n" +
    "                                 coordinate (x, y, z)\n" +
    "\n" +
    "Type is one of: city, forest, coast, underground, empty, none.\n" +
    "A manual type only shows on the world map when the sector has no\n" +
    "locations of its own; 'empty' and 'none' draw nothing. The coords\n" +
    "form creates the sector.o if it does not exist yet, so a virgin part\n" +
    "of the map can be painted.\n");
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

static int cmd(string str, object me, string verb)
{
  string * tokens;
  mapping sectors;
  string * keys;
  int i, j;

  tokens = explode(str ? str : "", " ") - ({ "" });

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

    // settype <type>  -> the sector we stand in
    if (sizeof(tokens) == 2)
    {
      int * coords;
      env = environment(me);
      if (!env || !env->query_location() || !env->query_coordinates())
      {
        notify_fail("You are not standing in a location with coordinates. " +
                    "Use 'sectors settype <x> <y> <z> <type>'.\n");
        return 0;
      }
      coords = env->query_coordinates();
      x = coords[0]; y = coords[1]; z = coords[2];
      map_name = env->query_map_name();
      type = tokens[1];
    }
    // settype <x> <y> <z> <type>
    else if (sizeof(tokens) == 5)
    {
      if (sscanf(tokens[1], "%d", x) != 1 ||
          sscanf(tokens[2], "%d", y) != 1 ||
          sscanf(tokens[3], "%d", z) != 1)
      {
        notify_fail("Coordinates must be integers.\n");
        return 0;
      }
      type = tokens[4];
      env = environment(me);
      map_name = (env && env->query_location()) ?
                 env->query_map_name() : "default";
    }
    else
    {
      notify_fail("Usage: sectors settype [<x> <y> <z>] <type>\n");
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

    write("Set manual type of sector at (" + x + "," + y + "," + z +
          ") in map '" + map_name + "' to '" + _canon_type(type) + "'.\n");
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
