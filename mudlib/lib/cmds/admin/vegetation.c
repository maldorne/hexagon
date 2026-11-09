// The kind of vegetation of an area: what trees, shrubs and herbs the
// vegetation components (forest, taiga, tundra, swamp) of its locations grow.
// The kinds are the game's own (its vegetation table); an area that names none
// answers with its parent's.

#include <mud/cmd.h>
#include <room/location.h>

inherit CMD_BASE;

// the components that grow vegetation
#define VEGETATION_TYPES ({ LOCATION_COMPONENT_FOREST, LOCATION_COMPONENT_TAIGA, \
                            LOCATION_COMPONENT_TUNDRA, LOCATION_COMPONENT_SWAMP })

void setup()
{
  set_aliases(({ "vegetation" }));
  set_usage("vegetation\n" +
            "     vegetation kind [<kind>|none]\n" +
            "     vegetation depth [<garden|thicket|forest|jungle>|none]\n" +
            "     vegetation replant [area]\n" +
            "     vegetation plants on|off");
  set_help(
    "Report and set the vegetation of the area you are standing in.\n" +
    "\n" +
    "  vegetation                 the kind and depth that apply here (the\n" +
    "                             area's own, or the nearest parent's), the\n" +
    "                             kinds the game knows and the plants standing\n" +
    "                             here\n" +
    "  vegetation kind <kind>     the area grows that kind of vegetation; the\n" +
    "                             plants already planted stay as they are and\n" +
    "                             the kind counts the next time they are rolled\n" +
    "  vegetation kind none       the area names no kind and uses its parent's\n" +
    "  vegetation depth <depth>   how dense the vegetation of the area is; a\n" +
    "                             location with a depth of its own keeps it\n" +
    "  vegetation depth none      the area names no depth and uses its parent's\n" +
    "  vegetation replant         takes away the plants of this location and\n" +
    "                             rolls new ones by the kind and depth of now\n" +
    "  vegetation replant area    the same in every loaded location of the area\n" +
    "                             with a vegetation component (the others keep\n" +
    "                             their saved plants)\n" +
    "  vegetation plants off      nothing grows in this location (no trees,\n" +
    "                             shrubs or herbs), which still counts as its\n" +
    "                             terrain: a walkway in the branches, a bridge,\n" +
    "                             a pier\n" +
    "  vegetation plants on       trees and herbs grow here again\n" +
    "\n" +
    "The vegetation components are forest, taiga, tundra and swamp. A location " +
    "with more than one is acted on in all of them. The depth (garden, " +
    "thicket, forest, jungle) decides how many plants stand in a location and " +
    "how often herbs grow; the kind decides which. A location whose area names " +
    "no kind takes the kind named like its component (swamp, tundra, taiga) if " +
    "the game has one. The plants are rolled once when a location loads with " +
    "none and then kept.");
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

// The kinds the game of the area has, empty when it has no vegetation table.
private string * kinds_of(object area)
{
  object vegetation;

  vegetation = table("vegetation", area);
  return vegetation ? (string *)vegetation->query_kinds() : ({ });
}

// The vegetation components a location has.
private object * vegetation_of(object location)
{
  object * found;
  object component;
  int i;

  found = ({ });
  if (!location)
    return found;

  for (i = 0; i < sizeof(VEGETATION_TYPES); i++)
  {
    component = location->query_component_by_type(VEGETATION_TYPES[i]);
    if (component)
      found += ({ component });
  }

  return found;
}

// the locations replanted per call_out: each one saves its location
#define REPLANT_BATCH 10

// Plants again the plants of one location. Whether it has vegetation.
private int replant_location(object location)
{
  object * found;
  int i;

  found = vegetation_of(location);
  for (i = 0; i < sizeof(found); i++)
    found[i]->replant();

  return sizeof(found) > 0;
}

/**
 * One batch of 'vegetation replant area': a few locations per call_out, since
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
    tell_object(who, "Plants planted again in " + count + " locations.\n");
}

private int do_report(object area, object me)
{
  object * found;
  string text;
  int i;

  found = vegetation_of(environment(me));

  text = "Area " + (string)area->query_area_name() + ": own kind '" +
         (string)area->query_own_vegetation_kind() + "', applies '" +
         (string)area->query_vegetation_kind() + "'; own depth '" +
         (string)area->query_own_vegetation_depth() + "', applies '" +
         (string)area->query_vegetation_depth() + "'.\n" +
         "Kinds of this game: " + implode(kinds_of(area), ", ") + ".\n";

  if (!sizeof(found))
    text += "This location has no vegetation component.\n";

  for (i = 0; i < sizeof(found); i++)
    text += "This location is " + (string)found[i]->query_type() + ", kind '" +
            (string)found[i]->query_vegetation_kind() + "', depth " +
            (string)found[i]->query_depth() + ", own depth '" +
            (string)found[i]->query_own_depth() + "', plants " +
            (string)implode(map_indices(found[i]->query_plants()), ", ") +
            (found[i]->query_grows_plants() ? "" : ", grows no plants") + ".\n";

  write(text);
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

  area->set_vegetation_kind(kind);
  write("Vegetation kind of the area: '" + (string)area->query_vegetation_kind() +
        "'. The plants already planted stay; 'vegetation replant' rolls them " +
        "again.\n");
  return 1;
}

private int do_depth(object area, object me, string depth)
{
  if (depth == "none")
    depth = "";
  else if (member_array(depth, VEGETATION_DEPTHS) < 0)
  {
    notify_fail("Unknown depth '" + depth + "'. Depths: " +
                implode(VEGETATION_DEPTHS, ", ") + ".\n");
    return 0;
  }

  area->set_vegetation_depth(depth);
  write("Vegetation depth of the area: '" + (string)area->query_vegetation_depth() +
        "'. The plants already planted stay; 'vegetation replant' rolls them " +
        "again.\n");
  return 1;
}

private int do_replant(object area, object me, int whole_area)
{
  object * locations;

  if (!whole_area)
  {
    write("Plants planted again in " + replant_location(environment(me)) +
          " locations.\n");
    return 1;
  }

  locations = area->query_loaded_locations();
  write("Planting again the plants of " + sizeof(locations) + " loaded " +
        "locations, a few at a time.\n");
  call_out("replant_step", 0, locations, 0, 0, me);
  return 1;
}

private int do_plants(object me, string flag)
{
  object * found;
  int i;

  found = vegetation_of(environment(me));
  if (!sizeof(found))
  {
    notify_fail("This location has no vegetation component.\n");
    return 0;
  }

  for (i = 0; i < sizeof(found); i++)
  {
    found[i]->set_grows_plants(flag == "on");
    if (flag == "on")
      found[i]->restore_plants();
  }

  write(flag == "on" ? "Trees and herbs grow here again.\n" :
        "Nothing grows here now; the location still counts as its terrain.\n");
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
                "vegetation of its area.\n");
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

  notify_fail("Usage: vegetation [ kind <kind>|none | depth <depth>|none | " +
              "replant [area] | plants on|off ]\n");
  return 0;
}
