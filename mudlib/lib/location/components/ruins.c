// Ruins component. Attach to what is left of a built place: fallen walls, a
// sacked fort, a buried temple. Contributes to the sector-level "ruins" tally
// so the world map draws the therefore sign.
//
// It adds no behaviour yet: it only marks the terrain.

#include <room/location.h>

inherit component "/lib/location/component.c";

void create()
{
  component::create();
  set_type(LOCATION_COMPONENT_RUINS);
}

void init() {}
void dest_me() {}

void initialize(object loc)
{
  component::initialize(loc);
}
