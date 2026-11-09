// Forest component. Attach to any location dominated by trees, from
// a walled garden with a couple of ornamental oaks to the darkest
// heart of an equatorial jungle. Contributes to the sector-level
// "forest" tally so the world map can render the sector as woodland. A
// location that is also a city counts toward the sector's forest-city tally:
// when half or more of its city locations are forest, the map draws the
// sector as a forest city (bold green woods glyph).
//
// How dense the woodland is (garden < thicket < forest < jungle) grades the
// canopy and how easy it is to travel through. It belongs to the area (set
// with the forest command, resolved up the area's parent chain like the kind);
// a location may override it with its own `depth`.
//
// What grows there depends on the woodland kind of the area and on the depth,
// with the data in the game's forests table (table("forests", location)):
//
//   trees and shrubs  planted when the location loads, as many as the depth
//                     says: they are fixtures (/lib/obj/fixture.c). The paths
//                     planted are kept in this component's saved state and
//                     cloned again, the same ones, on every later load. New
//                     ones are rolled only when none is saved and none stands
//                     here (see replant)
//   herbs             a herbalist walking in may find one growing, at most one
//                     at a time and once every FOREST_HERB_WAIT seconds; the
//                     table rolls the herb and how often by depth. Transient
//
// The table answers query_plants(kind) -> object paths (weighted by repeating
// them), query_seeks_herbs(who) -> whether the walker can see herbs, and
// query_herb_to_grow(who, kind, depth) -> a herb ready to be placed, or nil.
// A location whose area has no kind, or a game with no such table, grows
// nothing.

#include <room/location.h>

inherit component "/lib/location/component.c";

// the seconds between two herbs growing in the same place
#define FOREST_HERB_WAIT 60

// this location's own depth, "" to use the area's
private string depth;
// the trees and shrubs planted here, as path -> amount (saved)
private mapping plants;
// the live trees and shrubs of `plants`
static object * grown;
// the herb standing here, and when the last one was rolled
static object herb;
static int herb_time;

void create()
{
  component::create();
  set_type(LOCATION_COMPONENT_FOREST);
  depth = "";
  plants = ([ ]);
  grown = ({ });
}

void init() {}

// Takes away the trees and shrubs this component holds.
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

/**
 * The depth this location overrides the area's with, "" when it uses the
 * area's.
 */
string query_own_depth() { return depth; }

/**
 * Sets the depth of this location alone ("" to use the area's again). It
 * only counts for the next time trees are rolled.
 *
 * @param s  one of FOREST_DEPTHS, or ""
 */
void set_depth(string s) { depth = s ? s : ""; }

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

/**
 * The depth that applies here: this location's own, else the area's, else
 * "forest".
 */
string query_depth()
{
  object place, area;
  mixed value;

  if (strlen(depth))
    return depth;

  place = query_entity();
  area = place ? (object)place->query_area() : nil;
  value = area ? area->query_forest_depth() : nil;

  return stringp(value) && strlen(value) ? value : "forest";
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
  switch (query_depth())
  {
    case "garden":  return ({ 30, 1, 1 });
    case "thicket": return ({ 60, 1, 2 });
    case "jungle":  return ({ 100, 3, 4 });
    default:        return ({ 90, 2, 3 });
  }
}

// Clones one tree or shrub into the location and keeps it. Whether it exists.
private int put_plant(string path)
{
  object plant_ob;

  plant_ob = nil;
  catch(plant_ob = clone_object(path));
  if (!plant_ob)
    return 0;

  plant_ob->move(query_entity());
  grown += ({ plant_ob });
  return 1;
}

// Whether something that is a tree or a shrub stands in the location.
private int has_plants_standing()
{
  object * here;
  int i;

  here = all_inventory(query_entity());
  for (i = 0; i < sizeof(here); i++)
    if (here[i] && here[i]->query_plant())
      return 1;

  return 0;
}

// Rolls new trees and shrubs from the table, by kind and depth, and notes them.
private void roll_plants()
{
  object table_ob;
  string * paths, path;
  int * counts;
  int i, amount;

  table_ob = forests_table();
  if (!table_ob)
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
    path = paths[random(sizeof(paths))];
    if (put_plant(path))
      plants[path] = (plants[path] ? plants[path] : 0) + 1;
  }
}

// The saved amounts brought up to what really stands here: a tree that is gone
// (cut, destroyed, moved away) is forgotten.
private void reconcile_plants()
{
  object place;
  int i;

  place = query_entity();
  plants = ([ ]);
  grown -= ({ nil });

  for (i = 0; i < sizeof(grown); i++)
    if (environment(grown[i]) == place)
    {
      string path;

      path = base_name(grown[i]) + ".c";
      plants[path] = (plants[path] ? plants[path] : 0) + 1;
    }
}

/**
 * Clones again the trees and shrubs saved for this location. When none is
 * saved and none stands here, rolls new ones by the kind and depth, and saves
 * the location.
 */
void restore_plants()
{
  string * paths;
  int i, j;

  if (!query_entity())
    return;

  paths = map_indices(plants);
  for (i = 0; i < sizeof(paths); i++)
    for (j = 0; j < plants[paths[i]]; j++)
      put_plant(paths[i]);

  if (sizeof(grown) || has_plants_standing())
    return;

  plants = ([ ]);
  roll_plants();
  if (map_sizeof(plants))
    query_entity()->components_changed();
}

/**
 * The trees and shrubs saved for this location, as path -> amount.
 */
mapping query_plants() { return ([ ]) + plants; }

/**
 * Takes away all the trees and shrubs, saved and standing, and rolls new ones
 * by the kind and depth of now.
 */
void replant()
{
  uproot();
  plants = ([ ]);
  roll_plants();
  query_entity()->components_changed();
}

/**
 * Takes away one tree or shrub for good (it is cut down, say): it is destroyed
 * and forgotten, and the location is saved.
 *
 * @param plant_ob  a tree or shrub standing here
 */
void remove_plant(object plant_ob)
{
  if (!plant_ob || member_array(plant_ob, grown) < 0)
    return;

  grown -= ({ plant_ob });
  plant_ob->dest_me();
  query_entity()->components_changed();
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

  herb = (object)table_ob->query_herb_to_grow(who, query_forest_kind(), query_depth());
  if (herb)
    herb->move(query_entity());
}

/**
 * The trees and shrubs stand again as soon as the location has all its
 * components.
 */
void components_ready()
{
  restore_plants();
}

/**
 * A player walking in may find a herb.
 */
void event_enter(object ob, varargs string msg, object from, mixed avoid)
{
  if (ob && living(ob) && ob->query_player())
    grow_herb(ob);
}

mapping query_auto_load_attributes()
{
  reconcile_plants();

  return component::query_auto_load_attributes() +
         ([ "own_depth": depth, "plants": plants, ]);
}

void init_auto_load_attributes(mapping args)
{
  component::init_auto_load_attributes(args);
  if (stringp(args["own_depth"]))
    depth = args["own_depth"];
  if (mappingp(args["plants"]))
    plants = ([ ]) + args["plants"];
}

mixed * stats()
{
  return component::stats() +
         ({ ({ "Depth", query_depth(), }), ({ "Kind", query_forest_kind(), }),
            ({ "Plants", plants, }), });
}
