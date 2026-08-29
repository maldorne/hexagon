
// Points of interest.
//
// A POI marks a location the world cares about -- a venture, a town entrance or
// square, a crossroads, a shrine. It is what a job can be anchored to, and what
// tells the guard system which gates are watched, but it holds no people of its
// own: the posts held here are vacancies, and the people holding them are
// census individuals.

#include <room/location.h>
#include <areas/poi.h>
#include <living/persisted.h>

//   ([ location_file : ([ kind, label, guard_dir ]) ])
// See include/areas/poi.h for the field keys and the kind vocabulary.
mapping pois;


mapping query_pois() { return pois; }

// The POI attached to a location, or nil. A location holds at most one.
mapping query_poi(string location_file)
{
  return pois[location_file];
}

int is_poi(string location_file)
{
  return !undefinedp(pois[location_file]);
}

// Attach (or replace) a POI on a location. `kind` must be one of POI_KINDS;
// `label` is optional display text.
void add_poi(string location_file, string kind, varargs string label)
{
  mapping entry;

  if (!location_file || !strlen(location_file))
    return;
  if (member_array(kind, POI_KINDS) < 0)
    return;

  entry = pois[location_file];
  if (!entry)
    entry = ([ ]);

  entry[POI_FIELD_KIND] = kind;
  if (label && strlen(label))
    entry[POI_FIELD_LABEL] = label;

  pois[location_file] = entry;
  this_object()->save_me();
}

void remove_poi(string location_file)
{
  mapping census;
  string * ids;
  int i;

  // a POI's guards belong to it: drop them (destructing any live) before the
  // POI itself goes, so removing it never leaves an orphan guard behind
  census = (mapping)this_object()->query_npc_census();
  ids = map_indices(census);
  for (i = 0; i < sizeof(ids); i++)
    if (census[ids[i]]["guard"] &&
        census[ids[i]]["poi"] == location_file)
      this_object()->remove_guard(ids[i]);

  map_delete(pois, location_file);
  this_object()->save_me();
}

void set_poi_label(string location_file, string label)
{
  if (!pois[location_file])
    return;
  pois[location_file][POI_FIELD_LABEL] = label;
  this_object()->save_me();
}

// The exit direction a town_entrance POI's guards watch (the way into the
// town). Only meaningful on a town_entrance; ignored by square guards.
void set_poi_guard_dir(string location_file, string dir)
{
  if (!pois[location_file])
    return;
  pois[location_file][POI_FIELD_GUARD_DIR] = dir;
  this_object()->save_me();
}
string query_poi_guard_dir(string location_file)
{
  return pois[location_file] ? pois[location_file][POI_FIELD_GUARD_DIR] : nil;
}
