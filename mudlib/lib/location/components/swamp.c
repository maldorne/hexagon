// Swamp component. Attach to marsh and bog: reeds, stagnant pools, wet ground
// that is not water to swim in (that is the water component). Contributes to
// the sector-level "swamp" tally so the world map draws a green wave glyph.
//
// It adds no behaviour yet: it only marks the terrain.

#include <room/location.h>

inherit component "/lib/location/component.c";

void create()
{
  component::create();
  set_type(LOCATION_COMPONENT_SWAMP);
}

void init() {}
void dest_me() {}

void initialize(object loc)
{
  component::initialize(loc);
}
