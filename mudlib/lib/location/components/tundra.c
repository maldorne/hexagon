// Tundra component. Attach to cold treeless ground: frozen steppe, moss and
// lichen flats, the barren lands beyond the mountains. Contributes to the
// sector-level "tundra" tally so the world map draws the sector with the plains
// glyphs in bold.
//
// Kind, depth and the plants that grow come from the vegetation base; with no
// kind named by the area, it takes the game's "tundra" kind if there is one.

#include <room/location.h>

inherit vegetation "/lib/location/components/vegetation.c";

void create()
{
  vegetation::create();
  set_type(LOCATION_COMPONENT_TUNDRA);
}
