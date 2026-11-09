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
            "     forest kind [<kind>|none]");
  set_help(
    "Report and set the woodland kind of the area you are standing in.\n" +
    "\n" +
    "  forest               the kind that applies here (the area's own, or\n" +
    "                       the nearest parent's), the kinds the game knows\n" +
    "                       and the depth of this location\n" +
    "  forest kind <kind>   the area grows that kind of woodland; the trees\n" +
    "                       of the loaded forest locations of the area are\n" +
    "                       planted again\n" +
    "  forest kind none     the area names no kind and uses its parent's\n" +
    "\n" +
    "The depth of each location (garden, thicket, forest, jungle) decides how " +
    "many trees stand there and how often herbs grow; the kind decides which.");
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

// Plants again the trees of every loaded location of the area with a forest.
private int replant(object area)
{
  object * locations, forest;
  int i, count;

  locations = area->query_loaded_locations();
  for (i = 0; i < sizeof(locations); i++)
  {
    forest = locations[i]->query_component_by_type(LOCATION_COMPONENT_FOREST);
    if (forest)
    {
      forest->plant();
      count++;
    }
  }

  return count;
}

private int do_report(object area, object me)
{
  object forest;
  string own;

  own = (string)area->query_own_forest_kind();
  forest = environment(me)->query_component_by_type(LOCATION_COMPONENT_FOREST);

  write("Area " + (string)area->query_area_name() + ": own kind '" + own +
        "', applies '" + (string)area->query_forest_kind() + "'.\n" +
        "Kinds of this game: " + implode(kinds_of(area), ", ") + ".\n" +
        (forest ? "This location is a forest, depth " +
                  (string)forest->query_depth() + ".\n"
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
        "'. Trees planted again in " + replant(area) + " loaded locations.\n");
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

  notify_fail("Usage: forest [ kind <kind>|none ]\n");
  return 0;
}
