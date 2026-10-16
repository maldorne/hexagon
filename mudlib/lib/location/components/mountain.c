// Mountain component. Attach to steep rock: peaks, slopes, passes. Contributes
// to the sector-level "mountain" tally so the world map draws a brown
// triangle.
//
// It adds no behaviour yet: it only marks the terrain.

#include <room/location.h>

inherit component "/lib/location/component.c";

void create()
{
  component::create();
  set_type(LOCATION_COMPONENT_MOUNTAIN);
}

void init() {}
void dest_me() {}

void initialize(object loc)
{
  component::initialize(loc);
}
