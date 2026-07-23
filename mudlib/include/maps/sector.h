
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

// Cartographic exit types — the subset of the room exit_types (see
// lib/room/handlers/room_handler.c) that the world map draws as linear
// connections between sectors: a path is a single line, a road a double
// one. The sector records, per coordinate, which of its exits are of
// these types (sector.c `way_exits`), so the renderer can draw borders
// and a pathfinder can walk the graph. ROAD is listed first so a border
// carrying both collapses to the heavier road glyph.
#define SECTOR_WAY_PATH  "path"
#define SECTOR_WAY_ROAD  "road"
#define SECTOR_WAY_TYPES ({ SECTOR_WAY_ROAD, SECTOR_WAY_PATH })

// Cardinal border keys returned by query_border_ways(). Screen y grows
// downward but the world coord convention (see include/maps/maps.h) is
// +y = north, +x = east.
#define SECTOR_BORDER_N  "n"
#define SECTOR_BORDER_S  "s"
#define SECTOR_BORDER_E  "e"
#define SECTOR_BORDER_W  "w"

#endif // SECTORS_H
