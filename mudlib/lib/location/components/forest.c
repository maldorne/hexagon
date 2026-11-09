// Forest component. Attach to any location dominated by trees. Contributes to
// the sector-level "forest" tally so the world map can render the sector as
// woodland. A location that is also a city counts toward the sector's
// forest-city tally: when half or more of its city locations are forest, the
// map draws the sector as a forest city (bold green woods glyph).
//
// Kind, depth and the plants that grow come from the vegetation base.

#include <room/location.h>

inherit vegetation "/lib/location/components/vegetation.c";

void create()
{
  vegetation::create();
  set_type(LOCATION_COMPONENT_FOREST);
}
