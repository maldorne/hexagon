#ifndef AREAS_POI_H
#define AREAS_POI_H

// Points of interest. A POI marks a location the world cares about: a town
// gate, the square, an important crossroads, a shrine, a venture like a pub or
// a shop. It is where notable things are expected to happen, and what a system
// addresses when it wants to act on a place rather than on a person -- calling
// a citizenship's guards to the square, staging an event at the gate.
//
// A POI is not a job and does not hold one. A job is a vacancy (see
// <areas/vacancy.h>), it names its own workplace, and that workplace may be a
// POI or any ordinary location of the area. The two are independent: a POI
// with nobody working at it is normal, and so is a vacancy held somewhere the
// world has no particular interest in.
//
// A location carries at most one POI. POIs live on the owning area
// (area.o), keyed by the location's file_name:
//
//   pois: ([ location_file : ([
//              POI_FIELD_KIND:  "pub",
//              POI_FIELD_LABEL: "<label>",   // optional
//            ]) ])

// POI kinds. Venture kinds reuse the exact component name (pub, shop) so a
// location's component and its POI kind line up. Extend this list as more
// kinds are needed.
#define POI_KIND_PUB           "pub"
#define POI_KIND_SHOP          "shop"
#define POI_KIND_TOWN_ENTRANCE "town_entrance"
#define POI_KIND_TOWN_SQUARE   "town_square"
#define POI_KIND_CROSSROADS    "crossroads"
#define POI_KIND_SHRINE        "shrine"

// Every valid kind, for command validation.
#define POI_KINDS ({ \
  POI_KIND_PUB, POI_KIND_SHOP, POI_KIND_TOWN_ENTRANCE, \
  POI_KIND_TOWN_SQUARE, POI_KIND_CROSSROADS, POI_KIND_SHRINE })

// Venture kinds are inferred from a matching location component during
// room2loc conversion; the rest are declared by hand with the builder ring.
// Each entry pairs the POI kind with the component type that implies it.
#define POI_VENTURE_KINDS ({ POI_KIND_PUB, POI_KIND_SHOP })

// POI entry field keys.
#define POI_FIELD_KIND      "kind"
#define POI_FIELD_LABEL     "label"
// On a town_entrance POI: the exit direction that leads into the town. The
// entrance's guards watch this direction to block enemy citizenships.
#define POI_FIELD_GUARD_DIR "guard_dir"

#endif // AREAS_POI_H
