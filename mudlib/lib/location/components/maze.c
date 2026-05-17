// Maze component. Marker attached during conversion when the source
// room has the MAZE_PROP property set. Carries no behaviour of its own
// yet — its presence is the signal: the coord-guesser treats the
// location as opaque (no coords assigned, no propagation through its
// exits) and the cartography renderer paints the cell as '?' without
// recursing into its neighbours.
//
// Future maze-only behaviour (randomised exit directions on entry,
// rotating descriptions, fog effects, a hint that only appears when
// the viewer is lost long enough) lives here when it lands.

#include <room/location.h>

inherit component "/lib/location/component.c";

void create()
{
  component::create();
  set_type(LOCATION_COMPONENT_MAZE);
}

void init() {}
void dest_me() {}

void initialize(object location)
{
  component::initialize(location);
}
