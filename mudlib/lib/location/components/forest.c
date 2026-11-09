// Forest component. Attach to any location dominated by trees, from
// a walled garden with a couple of ornamental oaks to the darkest
// heart of an equatorial jungle. Contributes to the sector-level
// "forest" tally so the world map can render the sector as woodland. A
// location that is also a city counts toward the sector's forest-city tally:
// when half or more of its city locations are forest, the map draws the
// sector as a forest city (bold green woods glyph).
//
// Classifier `depth` grades canopy density and how easy it is to
// travel through, smallest -> densest:
//   garden < thicket < forest < jungle
//
// What grows there depends on the woodland kind of the area (set with the
// forest command, resolved up the area's parent chain) and on the depth, with
// the data in the game's forests table (table("forests", location)):
//
//   trees and shrubs  planted the first time somebody walks in, as many as the
//                     depth says (FOREST_PLANTS_*): they are fixtures
//                     (/lib/obj/fixture.c), cloned again on every load
//   herbs             a herbalist walking in may find one growing, at most one
//                     at a time and once every FOREST_HERB_WAIT seconds; the
//                     table rolls the herb and how often by depth
//
// The table answers query_plants(kind) -> object paths (weighted by repeating
// them), query_seeks_herbs(who) -> whether the walker can see herbs, and
// query_herb_to_grow(who, kind, depth) -> a herb ready to be placed, or nil.
// A location whose area has no kind, or a game with no such table, grows
// nothing.

#include <room/location.h>

inherit component "/lib/location/component.c";

void plant();

// the seconds between two herbs growing in the same place
#define FOREST_HERB_WAIT 60

// garden | thicket | forest | jungle
private string depth;
// the trees and shrubs this component planted, and whether it has planted yet
static object * grown;
static int planted;
// the herb standing here, and when the last one was rolled
static object herb;
static int herb_time;

void create()
{
  component::create();
  set_type(LOCATION_COMPONENT_FOREST);
  depth = "forest";
  grown = ({ });
}

void init() {}

// Takes away the trees and shrubs this component planted.
private void uproot()
{
  int i;

  for (i = 0; i < sizeof(grown); i++)
    if (grown[i])
      grown[i]->dest_me();
  grown = ({ });
}

/**
 * The trees and shrubs go with the location.
 */
void dest_me() { uproot(); }

void initialize(object loc)
{
  component::initialize(loc);
}

string query_depth() { return depth; }

void set_depth(string s)
{
  depth = s;
  // the new depth decides how many trees stand here
  if (planted)
    plant();
}

/**
 * The woodland kind of the area this location is in ("" when no area up the
 * chain names one).
 */
string query_forest_kind()
{
  object place, area;
  mixed kind;

  place = query_entity();
  area = place ? (object)place->query_area() : nil;
  kind = area ? area->query_forest_kind() : nil;

  return stringp(kind) ? kind : "";
}

// The game's forests table, nil when it has none.
private object forests_table()
{
  return query_entity() ? table("forests", query_entity()) : nil;
}

// The chance, in a hundred, that trees stand in a place, and the fewest and
// most that do: ({ chance, least, most }).
private int * plant_counts()
{
  switch (depth)
  {
    case "garden":  return ({ 30, 1, 1 });
    case "thicket": return ({ 60, 1, 2 });
    case "jungle":  return ({ 100, 3, 4 });
    default:        return ({ 90, 2, 3 });
  }
}

/**
 * Plants the trees and shrubs of the kind of woodland, as many as the depth
 * says, in place of any it planted before. Called the first time a living
 * walks in, and again when the kind of the area changes.
 */
void plant()
{
  object table_ob, plant_ob;
  string * paths;
  int * counts;
  int i, amount;

  planted = 1;
  uproot();

  table_ob = forests_table();
  if (!table_ob || !query_entity())
    return;

  paths = (string *)table_ob->query_plants(query_forest_kind());
  if (!sizeof(paths))
    return;

  counts = plant_counts();
  if (random(100) >= counts[0])
    return;

  amount = counts[1] + random(counts[2] - counts[1] + 1);
  for (i = 0; i < amount; i++)
  {
    plant_ob = nil;
    catch(plant_ob = clone_object(paths[random(sizeof(paths))]));
    if (!plant_ob)
      continue;

    plant_ob->move(query_entity());
    grown += ({ plant_ob });
  }
}

// A herbalist walking in may find a herb growing: not while one stands here
// already, and not twice within FOREST_HERB_WAIT seconds.
private void grow_herb(object who)
{
  object table_ob;

  table_ob = forests_table();
  if (!table_ob || !table_ob->query_seeks_herbs(who))
    return;

  if (herb && environment(herb) == query_entity())
    return;
  if (time() - herb_time < FOREST_HERB_WAIT)
    return;
  herb_time = time();

  herb = (object)table_ob->query_herb_to_grow(who, query_forest_kind(), depth);
  if (herb)
    herb->move(query_entity());
}

/**
 * The first living in plants the trees; a player walking in may find a herb.
 */
void event_enter(object ob, varargs string msg, object from, mixed avoid)
{
  if (!ob || !living(ob))
    return;

  if (!planted)
    plant();
  if (ob->query_player())
    grow_herb(ob);
}

mapping query_auto_load_attributes()
{
  return component::query_auto_load_attributes() +
         ([ "depth": depth, ]);
}

void init_auto_load_attributes(mapping args)
{
  component::init_auto_load_attributes(args);
  if (!undefinedp(args["depth"]))
    depth = args["depth"];
}

mixed * stats()
{
  return component::stats() +
         ({ ({ "Depth", depth, }), ({ "Kind", query_forest_kind(), }), });
}
