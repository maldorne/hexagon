
#ifndef SECTORS_H
#define SECTORS_H

#include <room/location.h>

// A sector's "type" is the dominant terrain across its locations.
// Sector types share the exact taxonomy of the cartography-contributing
// location components in include/room/location.h — a location with a
// COMPONENT_CITY component counts toward SECTOR_TYPE_CITY, etc.
//
// The counts live in sector.c's `type_counts` mapping, populated by
// add_location() and decremented by remove_location(). Consumers read
// them via sector->query_sector_type() (dominant type) or
// sector->query_type_counts() (full distribution).

#define SECTOR_TYPE_NONE        ""
#define SECTOR_TYPE_CITY        LOCATION_COMPONENT_CITY
#define SECTOR_TYPE_ROAD        LOCATION_COMPONENT_ROAD
#define SECTOR_TYPE_FOREST      LOCATION_COMPONENT_FOREST
#define SECTOR_TYPE_COAST       LOCATION_COMPONENT_COAST
#define SECTOR_TYPE_UNDERGROUND LOCATION_COMPONENT_UNDERGROUND

// Iterated in this order by query_sector_type() with strict `>`. That
// makes the earlier entries win ties: on equal counts, city beats road,
// road beats coast, and so on. Reflects visual salience on a world map.
#define SECTOR_CONTRIB_COMPONENTS ({ \
    SECTOR_TYPE_CITY, SECTOR_TYPE_ROAD, SECTOR_TYPE_COAST, \
    SECTOR_TYPE_FOREST, SECTOR_TYPE_UNDERGROUND \
  })

#endif // SECTORS_H
