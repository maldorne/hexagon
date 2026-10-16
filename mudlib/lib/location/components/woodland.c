// Woodland component. A second kind of woods, drawn with its own outlined
// glyph beside the forest one. What sets it apart from the forest component is
// still to be decided; until then it only names the terrain. Contributes to
// the sector-level "woodland" tally.
//
// It adds no behaviour yet: it only marks the terrain.

#include <room/location.h>

inherit component "/lib/location/component.c";

void create()
{
  component::create();
  set_type(LOCATION_COMPONENT_WOODLAND);
}

void init() {}
void dest_me() {}

void initialize(object loc)
{
  component::initialize(loc);
}
