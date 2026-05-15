
#include <mud/cmd.h>
#include <room/location.h>

inherit CMD_BASE;

void setup()
{
  set_aliases(({ "room2loc" }));
  set_usage("room2loc < filename | dirname | here >\n" +
            "room2loc coords <location.o> <x> <y> <z>");
  set_help("Convert one or more legacy .c rooms into .o locations.\n" +
           "The target may be an absolute file or directory, a path\n" +
           "relative to your coder path, or 'here' for your current\n" +
           "room. Coordinates are guessed from the exit graph after\n" +
           "the batch finishes.\n" +
           "\n" +
           "Subverb 'coords' seeds an already-converted location with\n" +
           "specific (x,y,z) coordinates and re-runs coordinate inference\n" +
           "across its neighbours. Use it to give the inference a starting\n" +
           "point for a cluster that is NOT connected (through exits) to\n" +
           "any location that already has coordinates. If the inference\n" +
           "discovers neighbours with their own coordinates, the map's\n" +
           "topology wins and the seed is discarded — this verb cannot\n" +
           "override a real anchor.\n");
}

static int _filter_dot_c(string file)
{
  return file[strlen(file) - 2..] == ".c";
}

/**
 * Seed a location's (x,y,z) when it sits in a cluster disconnected
 * from the rest of the worldmap.
 *
 * Sets the coordinates and saves the .o, then triggers the standard
 * inference pass starting from this location. If any neighbour
 * reachable through the exit graph already has coordinates, that
 * neighbour's anchor wins and the seed is overwritten — the inference
 * always honours map topology over a manual seed.
 *
 * The verb is only useful when every reachable neighbour lacks
 * coordinates; that is the case the `do_guess_coordinates` walker
 * cannot solve on its own.
 */
static int do_coords(string str)
{
  string file;
  int x, y, z;
  object location;

  if (sscanf(str, "%s %d %d %d", file, x, y, z) != 4)
  {
    notify_fail("Usage: room2loc coords <location.o> <x> <y> <z>\n");
    return 0;
  }

  if (file_size(file) <= 0)
  {
    notify_fail("Location file '" + file + "' does not exist.\n");
    return 0;
  }

  if (file[strlen(file) - 2..] != ".o")
  {
    notify_fail("Expected a .o location file, got '" + file + "'.\n");
    return 0;
  }

  location = load_object(LOCATION_HANDLER)->load_location(file);

  if (!location || !location->query_location())
  {
    notify_fail("'" + file + "' is not a location.\n");
    return 0;
  }

  location->set_coordinates(x, y, z);
  location->save_me();

  write("Seeded " + file + " at (" + x + ", " + y + ", " + z + ").\n");

  // propagate to neighbours that have no coordinates yet — map topology
  // wins over the seed if a connected neighbour is already coordinated
  load_object(LOCATION_HANDLER)->do_guess_coordinates(({ location }));

  // re-load and report what actually stuck
  location = load_object(LOCATION_HANDLER)->load_location(file);

  if (location && location->query_coordinates())
  {
    int * final;
    final = location->query_coordinates();

    if (final[0] != x || final[1] != y || final[2] != z)
      write("Final coordinates: (" + final[0] + ", " + final[1] + ", " +
            final[2] + ") -- the inference found a connected anchor and " +
            "your seed was discarded.\n");
  }

  return 1;
}

static int cmd(string str, object me, string verb)
{
  string * args;
  string * files;

  if (!str || !strlen(str))
  {
    notify_fail("Convert what?\n");
    return 0;
  }

  args = explode(str, " ");

  if (args[0] == "coords")
    return do_coords(implode(args[1..], " "));

  if (str == "here")
  {
    if (!environment(me))
    {
      notify_fail("You are nowhere.\n");
      return 0;
    }
    files = ({ base_name(environment(me)) });
  }
  else
  {
    files = load_object(LOCATION_HANDLER)->resolve_targets(str);
    if (!sizeof(files))
    {
      notify_fail("No such file or directory.\n");
      return 0;
    }
  }

  // only .c files (rooms); skip already-converted locations
  files = filter_array(files, "_filter_dot_c");

  if (!sizeof(files))
  {
    write("No room files to convert.\n");
    return 1;
  }

  load_object(LOCATION_HANDLER)->batch_convert(files);

  return 1;
}
