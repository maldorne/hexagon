// Tundra component. Attach to cold treeless ground: frozen steppe, moss and
// lichen flats, the barren lands beyond the mountains. Contributes to the
// sector-level "tundra" tally so the world map draws the sector with the plains
// glyphs in bold.
//
// It adds no behaviour yet: it only marks the terrain.

#include <room/location.h>

inherit component "/lib/location/component.c";

void create()
{
  component::create();
  set_type(LOCATION_COMPONENT_TUNDRA);
}

void init() {}
void dest_me() {}

void initialize(object loc)
{
  component::initialize(loc);
}
