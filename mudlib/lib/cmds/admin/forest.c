// The kind of woodland of an area: what trees, shrubs and herbs the forest
// component of its locations grows. The kinds are the game's own (its forests
// table); an area that names none answers with its parent's.

#include <mud/cmd.h>
#include <room/location.h>

inherit CMD_BASE;

void setup()
{
  set_aliases(({ "forest" }));
  set_usage("forest\n" +
            "     forest kind [<kind>|none]\n" +
            "     forest depth [<garden|thicket|forest|jungle>|none]\n" +
            "     forest replant [area]");
  set_help(
    "Report and set the woodland of the area you are standing in.\n" +
    "\n" +
    "  forest                 the kind and depth that apply here (the area's\n" +
    "                         own, or the nearest parent's), the kinds the\n" +
    "                         game knows and the plants standing here\n" +
    "  forest kind <kind>     the area grows that kind of woodland; the trees\n" +
    "                         already planted stay as they are and the kind\n" +
    "                         counts the next time trees are rolled\n" +
    "  forest kind none       the area names no kind and uses its parent's\n" +
    "  forest depth <depth>   how dense the woodland of the area is; a\n" +
    "                         location with a depth of its own keeps it\n" +
    "  forest depth none      the area names no depth and uses its parent's\n" +
    "  forest replant         takes away the trees of this location and rolls\n" +
    "                         new ones by the kind and depth of now\n" +
    "  forest replant area    the same in every loaded forest location of the\n" +
    "                         area (the others keep their saved trees)\n" +
    "\n" +
    "The depth (garden, thicket, forest, jungle) decides how many trees stand " +
    "in a location and how often herbs grow; the kind decides which. The trees " +
    "are rolled once when a location loads with none and then kept.");
}

// The area of the location the player is standing in, or nil.
private object area_of(object me)
{
  object env;

  env = environment(me);
  if (!env || !env->query_location())
    return nil;

  return env->query_area();
}

// The kinds the game of the area has, empty when it has no forests table.
private string * kinds_of(object area)
{
  object forests;

  forests = table("forests", area);
  return forests ? (string *)forests->query_kinds() : ({ });
}

// Plants again the trees of the location of the player, or of every loaded
// location of the area with a forest. How many locations were replanted.
private int replant(object area, object me, int whole_area)
{
  object * locations, forest;
  int i, count;

  locations = whole_area ? area->query_loaded_locations() : ({ environment(me) });
  for (i = 0; i < sizeof(locations); i++)
  {
    forest = locations[i]->query_component_by_type(LOCATION_COMPONENT_FOREST);
    if (forest)
    {
      forest->replant();
      count++;
    }
  }

  return count;
}

private int do_report(object area, object me)
{
  object forest;

  forest = environment(me)->query_component_by_type(LOCATION_COMPONENT_FOREST);

  write("Area " + (string)area->query_area_name() + ": own kind '" +
        (string)area->query_own_forest_kind() + "', applies '" +
        (string)area->query_forest_kind() + "'; own depth '" +
        (string)area->query_own_forest_depth() + "', applies '" +
        (string)area->query_forest_depth() + "'.\n" +
        "Kinds of this game: " + implode(kinds_of(area), ", ") + ".\n" +
        (forest ? "This location is a forest, depth " +
                  (string)forest->query_depth() + ", own depth '" +
                  (string)forest->query_own_depth() + "', plants " +
                  (string)implode(map_indices(forest->query_plants()), ", ") + ".\n"
                : "This location has no forest component.\n"));
  return 1;
}

private int do_kind(object area, object me, string kind)
{
  if (kind == "none")
    kind = "";
  else if (member_array(kind, kinds_of(area)) < 0)
  {
    notify_fail("Unknown kind '" + kind + "'. Kinds: " +
                implode(kinds_of(area), ", ") + ".\n");
    return 0;
  }

  area->set_forest_kind(kind);
  write("Woodland kind of the area: '" + (string)area->query_forest_kind() +
        "'. The trees already planted stay; 'forest replant' rolls them " +
        "again.\n");
  return 1;
}

private int do_depth(object area, object me, string depth)
{
  if (depth == "none")
    depth = "";
  else if (member_array(depth, FOREST_DEPTHS) < 0)
  {
    notify_fail("Unknown depth '" + depth + "'. Depths: " +
                implode(FOREST_DEPTHS, ", ") + ".\n");
    return 0;
  }

  area->set_forest_depth(depth);
  write("Woodland depth of the area: '" + (string)area->query_forest_depth() +
        "'. The trees already planted stay; 'forest replant' rolls them " +
        "again.\n");
  return 1;
}

private int do_replant(object area, object me, int whole_area)
{
  write("Trees planted again in " + replant(area, me, whole_area) +
        " locations.\n");
  return 1;
}

static int cmd(string str, object me, string verb)
{
  string * args;
  object area;

  area = area_of(me);
  if (!area)
  {
    notify_fail("Stand in a location (not a plain room) to see the " +
                "woodland of its area.\n");
    return 0;
  }

  args = explode(str ? str : "", " ") - ({ "" });

  if (!sizeof(args))
    return do_report(area, me);

  if (args[0] == "kind" && sizeof(args) == 2)
    return do_kind(area, me, args[1]);

  if (args[0] == "depth" && sizeof(args) == 2)
    return do_depth(area, me, args[1]);

  if (args[0] == "replant" && sizeof(args) == 1)
    return do_replant(area, me, 0);

  if (args[0] == "replant" && sizeof(args) == 2 && args[1] == "area")
    return do_replant(area, me, 1);

  notify_fail("Usage: forest [ kind <kind>|none | depth <depth>|none | " +
              "replant [area] ]\n");
  return 0;
}
