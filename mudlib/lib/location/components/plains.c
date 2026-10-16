// Plains component. Attach to open grassland: meadows, steppe, moor.
// Contributes to the sector-level "plains" tally so the world map draws the
// sector with one of two diagonal-dot glyphs, chosen per sector from its
// coordinates.
//
// It adds no behaviour yet: it only marks the terrain.

#include <room/location.h>

inherit component "/lib/location/component.c";

void create()
{
  component::create();
  set_type(LOCATION_COMPONENT_PLAINS);
}

void init() {}
void dest_me() {}

void initialize(object loc)
{
  component::initialize(loc);
}
