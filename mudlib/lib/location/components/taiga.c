// Taiga component. Cold conifer woods, drawn with their own outlined
// glyph beside the forest one. Contributes to the sector-level "taiga" tally.
//
// Kind, depth and the plants that grow come from the vegetation base; with no
// kind named by the area, it takes the game's "taiga" kind if there is one.

#include <room/location.h>

inherit vegetation "/lib/location/components/vegetation.c";

void create()
{
  vegetation::create();
  set_type(LOCATION_COMPONENT_TAIGA);
}
