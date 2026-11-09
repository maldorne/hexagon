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
            "     forest replant [area]\n" +
            "     forest plants on|off");
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
    "  forest plants off      nothing grows in this location (no trees, shrubs\n" +
    "                         or herbs), which still counts as woodland: a\n" +
    "                         walkway in the branches, a bridge, a pier\n" +
    "  forest plants on       trees and herbs grow here again\n" +
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

// the locations replanted per call_out: each one saves its location
#define REPLANT_BATCH 10

// Plants again the trees of one location. Whether it has a forest.
private int replant_location(object location)
{
  object forest;

  forest = location ? location->query_component_by_type(LOCATION_COMPONENT_FOREST) : nil;
  if (!forest)
    return 0;

  forest->replant();
  return 1;
}

/**
 * One batch of 'forest replant area': a few locations per call_out, since
 * every replant saves its location and a whole area does not fit in one
 * execution. Tells who asked when the list is done. Public because the
 * call_out dispatcher reaches it through call_other.
 *
 * @param locations  the loaded locations of the area
 * @param idx        the first one of this batch
 * @param count      the locations replanted so far
 * @param who        who gets the summary
 */
void replant_step(object * locations, int idx, int count, object who)
{
  int i;

  for (i = idx; i < sizeof(locations) && i < idx + REPLANT_BATCH; i++)
    catch(count += replant_location(locations[i]));

  if (i < sizeof(locations))
  {
    call_out("replant_step", 0, locations, i, count, who);
    return;
  }

  if (who)
    tell_object(who, "Trees planted again in " + count + " locations.\n");
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
                  (string)implode(map_indices(forest->query_plants()), ", ") +
                  (forest->query_grows_plants() ? "" : ", grows no plants") + ".\n"
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
  object * locations;

  if (!whole_area)
  {
    write("Trees planted again in " + replant_location(environment(me)) +
          " locations.\n");
    return 1;
  }

  locations = area->query_loaded_locations();
  write("Planting again the trees of " + sizeof(locations) + " loaded " +
        "locations, a few at a time.\n");
  call_out("replant_step", 0, locations, 0, 0, me);
  return 1;
}

private int do_plants(object me, string flag)
{
  object forest;

  forest = environment(me)->query_component_by_type(LOCATION_COMPONENT_FOREST);
  if (!forest)
  {
    notify_fail("This location has no forest component.\n");
    return 0;
  }

  forest->set_grows_plants(flag == "on");
  if (flag == "on")
    forest->restore_plants();
  write(flag == "on" ? "Trees and herbs grow here again.\n" :
        "Nothing grows here now; the location still counts as woodland.\n");
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

  if (args[0] == "plants" && sizeof(args) == 2 &&
      (args[1] == "on" || args[1] == "off"))
    return do_plants(me, args[1]);

  notify_fail("Usage: forest [ kind <kind>|none | depth <depth>|none | " +
              "replant [area] | plants on|off ]\n");
  return 0;
}
