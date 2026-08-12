#ifndef AREAS_POI_H
#define AREAS_POI_H

// Points of interest. A POI marks a location as somewhere the world cares
// about -- a venture (pub, shop), a town entrance or square, an important
// crossroads, a shrine. It is the anchor for vacancies: named NPC slots
// (the pub's barman, a healer, a unique) that the vacancy system fills at
// that location, separately from the statistical population sweep.
//
// A location carries at most one POI. POIs live on the owning area
// (area.o), keyed by the location's file_name:
//
//   pois: ([ location_file : ([
//              POI_FIELD_KIND:      "pub",
//              POI_FIELD_LABEL:     "El Penultimo Hogar",   // optional
//              POI_FIELD_VACANCIES: ({ ([
//                  VACANCY_FIELD_ROLE:   "barman",
//                  VACANCY_FIELD_SOURCE: "/games/.../npcs/corvin.c",
//                  VACANCY_FIELD_UUID:   uuid_or_nil,        // filled slot
//              ]), ... }),
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
#define POI_FIELD_VACANCIES "vacancies"
// On a town_entrance POI: the exit direction that leads into the town. The
// entrance's guards watch this direction to block enemy citizenships.
#define POI_FIELD_GUARD_DIR "guard_dir"

// Vacancy entry field keys.
#define VACANCY_FIELD_ROLE   "role"
#define VACANCY_FIELD_SOURCE "source"
#define VACANCY_FIELD_UUID   "uuid"

// Seconds before a dead vacancy NPC is respawned at its POI.
#define VACANCY_RESPAWN_DELAY 300

#endif // AREAS_POI_H
