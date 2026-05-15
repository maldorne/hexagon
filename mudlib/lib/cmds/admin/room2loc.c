
#include <mud/cmd.h>
#include <room/location.h>

inherit CMD_BASE;

void setup()
{
  set_aliases(({ "room2loc" }));
  set_usage("room2loc < filename | dirname | here >\n" +
            "room2loc coords <location.o> <x> <y> <z>\n" +
            "room2loc clean [apply] <path>");
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
           "override a real anchor.\n" +
           "\n" +
           "Subverb 'clean' removes every location under <path> (which\n" +
           "may be a .c source directory or the corresponding save dir,\n" +
           "even if the .c side has already been deleted), strips the\n" +
           "map index entries, and trims broken exits on adjacent\n" +
           "locations across the rest of the world. By default it is a\n" +
           "dry-run that only prints the counts; pass 'apply' as the\n" +
           "first word after 'clean' to actually execute.\n");
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

/**
 * Preview or apply a clean of every location under `path` (and the
 * matching map / area index entries, and broken exits in the rest of
 * the world). Pass `apply=1` to execute; `apply=0` is a dry-run.
 */
static int do_clean(string path, int apply)
{
  string * resolved;
  string save_dir;
  string root_locations, root_areas;
  mixed * preview;
  mixed * counts;
  string * orphans;
  mapping adjacent_trim;
  string * adj_keys;
  int trim_count;
  int i;

  if (!path || !strlen(path))
  {
    notify_fail("Usage: room2loc clean [apply] <path>\n");
    return 0;
  }

  resolved = load_object(LOCATION_HANDLER)->resolve_clean_scope(path);
  save_dir = resolved[0];

  if (!strlen(save_dir))
  {
    notify_fail("Cannot resolve '" + path + "' to a save directory.\n" +
                "Expected '/games/<game>/areas/<rest>' or " +
                "'/save/games/<game>/locations/areas/<rest>'.\n");
    return 0;
  }

  root_locations = "/save/games/" + game_from_path(save_dir) + "/locations";
  root_areas = root_locations + "/areas";

  if (save_dir == root_locations || save_dir == root_areas)
  {
    notify_fail("Refusing to clean a root path (" + save_dir + ").\n");
    return 0;
  }

  preview = load_object(LOCATION_HANDLER)->clean_preview(path);
  orphans = preview[0];
  adjacent_trim = preview[1];

  adj_keys = map_indices(adjacent_trim);
  trim_count = 0;
  for (i = 0; i < sizeof(adj_keys); i++)
    trim_count += sizeof(adjacent_trim[adj_keys[i]]);

  if (!sizeof(orphans))
  {
    write("Nothing to clean under " + save_dir + ".\n");
    return 1;
  }

  write("Clean " + (apply ? "applying" : "preview") + " for " + save_dir + "\n");
  write("  Locations to delete: " + sizeof(orphans) + "\n");
  write("  Adjacent locations with exits to trim: " + sizeof(adj_keys) +
        " (" + trim_count + " exits)\n");

  if (!apply)
  {
    write("Run 'room2loc clean apply " + path + "' to execute.\n");
    return 1;
  }

  counts = load_object(LOCATION_HANDLER)->clean_apply(path);
  write("Done. Removed " + counts[0] + " locations, trimmed " +
        counts[2] + " exits on " + counts[1] + " adjacent locations.\n");
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

  if (args[0] == "clean")
  {
    if (sizeof(args) >= 2 && args[1] == "apply")
      return do_clean(implode(args[2..], " "), 1);
    return do_clean(implode(args[1..], " "), 0);
  }

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
