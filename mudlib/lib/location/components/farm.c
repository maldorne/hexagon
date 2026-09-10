// Farm component. Attach to any location that is worked farmland: a
// crop field, an orchard, a vineyard, a kitchen garden. Contributes to
// the sector-level "farm" tally so the world map can render the sector
// as cultivated land (a yellow furrow glyph), distinct from wild forest.
//
// A farm location is also the kind of place a settlement's jobs send somebody
// to work (a farmer's workplace) -- but that binding lives on the job, not
// here; this component only marks the terrain.
//
// Classifier `crop` names what grows there. No implicit ordering -- pick
// whichever fits:
//   wheat | barley | vegetables | orchard | vineyard
//
// The stub adds no runtime behaviour yet -- no hooks, no exit-graph
// transforms -- it exists so authors can start tagging fields while the
// downstream consumers (renderer, spawner) are still in flight.

#include <room/location.h>

inherit component "/lib/location/component.c";

// wheat | barley | vegetables | orchard | vineyard
private string crop;

void create()
{
  component::create();
  set_type(LOCATION_COMPONENT_FARM);
  crop = "wheat";
}

void init() {}
void dest_me() {}

void initialize(object loc)
{
  component::initialize(loc);
}

string query_crop() { return crop; }
void set_crop(string s) { crop = s; }

mapping query_auto_load_attributes()
{
  return component::query_auto_load_attributes() +
         ([ "crop": crop, ]);
}

void init_auto_load_attributes(mapping args)
{
  component::init_auto_load_attributes(args);
  if (!undefinedp(args["crop"]))
    crop = args["crop"];
}

mixed * stats()
{
  return component::stats() +
         ({ ({ "Crop", crop, }), });
}
