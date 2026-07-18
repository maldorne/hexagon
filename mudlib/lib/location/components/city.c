// City component. Attach to any location that belongs to inhabited
// built-up terrain: streets, squares, courtyards, indoor rooms of a
// settlement building. Contributes to the sector-level "city" tally
// so the world map can render the sector as urban.
//
// Classifier `size` grades the settlement scale:
//   village < town < city < metropolis
//
// The stub adds no runtime behaviour yet — no hooks, no exit-graph
// transforms — it exists so authors can start tagging content while
// the downstream consumers (renderer, spawner) are still in flight.

#include <room/location.h>

inherit component "/lib/location/component.c";

// village | town | city | metropolis
private string size;

void create()
{
  component::create();
  set_type(LOCATION_COMPONENT_CITY);
  size = "village";
}

void init() {}
void dest_me() {}

void initialize(object loc)
{
  component::initialize(loc);
}

string query_size() { return size; }
void set_size(string s) { size = s; }

mapping query_auto_load_attributes()
{
  return component::query_auto_load_attributes() +
         ([ "size": size, ]);
}

void init_auto_load_attributes(mapping args)
{
  component::init_auto_load_attributes(args);
  if (!undefinedp(args["size"]))
    size = args["size"];
}

mixed * stats()
{
  return component::stats() +
         ({ ({ "Size", size, }), });
}
