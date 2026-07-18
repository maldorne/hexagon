// Forest component. Attach to any location dominated by trees, from
// a walled garden with a couple of ornamental oaks to the darkest
// heart of an equatorial jungle. Contributes to the sector-level
// "forest" tally so the world map can render the sector as woodland.
//
// Classifier `depth` grades canopy density and how easy it is to
// travel through, smallest → densest:
//   garden < thicket < forest < jungle

#include <room/location.h>

inherit component "/lib/location/component.c";

// garden | thicket | forest | jungle
private string depth;

void create()
{
  component::create();
  set_type(LOCATION_COMPONENT_FOREST);
  depth = "forest";
}

void init() {}
void dest_me() {}

void initialize(object loc)
{
  component::initialize(loc);
}

string query_depth() { return depth; }
void set_depth(string s) { depth = s; }

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
         ({ ({ "Depth", depth, }), });
}
