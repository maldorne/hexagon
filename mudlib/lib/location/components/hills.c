// Hills component. Attach to rolling high ground that is not a mountain: hills,
// downs, knolls. Contributes to the sector-level "hills" tally so the world
// map draws the sector with a brown half circle.
//
// It adds no behaviour yet: it only marks the terrain.

#include <room/location.h>

inherit component "/lib/location/component.c";

void create()
{
  component::create();
  set_type(LOCATION_COMPONENT_HILLS);
}

void init() {}
void dest_me() {}

void initialize(object loc)
{
  component::initialize(loc);
}
