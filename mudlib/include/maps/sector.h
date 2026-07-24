
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

// The location components the world map cares about. A sector tallies how
// many of its locations carry each one (city, forest, ...); the dominant
// tally is the sector's map classification. These are the same component
// types a location can hold (see include/room/location.h). Road is not
// among them — roads are drawn from the road/path EXITS a sector has (see
// SECTOR_WAY_* below), not from a component tally.
#define SECTOR_TYPE_NONE        ""
#define SECTOR_TYPE_CITY        LOCATION_COMPONENT_CITY
#define SECTOR_TYPE_FOREST      LOCATION_COMPONENT_FOREST
#define SECTOR_TYPE_COAST       LOCATION_COMPONENT_COAST
#define SECTOR_TYPE_UNDERGROUND LOCATION_COMPONENT_UNDERGROUND

// A manual type a programmer can set on a sector (query_manual_type /
// set_manual_type), used when the sector has no locations of its own to
// derive a type from. SECTOR_TYPE_EMPTY is the placeholder left on a
// sector emptied by `room2loc clean`: the sector.o and its folder stay so
// the sector can later be painted a real type, but nothing is drawn for
// it on the world map.
#define SECTOR_TYPE_EMPTY       "empty"

// Iterated in this order by query_sector_type() with strict `>`, so on
// equal counts the earlier entry wins. This is the majority-component
// tie-break only; the map renderer applies a separate display priority
// (city, then road/path exits, then the majority component).
#define SECTOR_MAP_COMPONENTS ({ \
    SECTOR_TYPE_CITY, SECTOR_TYPE_COAST, \
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

// Canonical (English) direction words stored in way_exits. They match
// room_handler::canonical_dir output, so query_border_ways compares
// against these rather than the language-dependent DIR_* macros.
#define SECTOR_DIR_NORTH "north"
#define SECTOR_DIR_SOUTH "south"
#define SECTOR_DIR_EAST  "east"
#define SECTOR_DIR_WEST  "west"

#endif // SECTORS_H
