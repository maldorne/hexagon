
#ifndef LOCATION_H
#define LOCATION_H

#include <ecs/ecs.h>

#define BASE_LOCATION_OBJ "/lib/location.c"
// why uncarve_location refuses a location with something in it and nowhere to
// move it, so a caller can say where that somewhere should come from
#define LOCATION_NOT_EMPTY "That location is not empty."
#define LOCATION_HANDLER "/lib/room/handlers/location_handler.c"

// location component types

#define LOCATION_COMPONENT_SHOP        "shop"
#define LOCATION_COMPONENT_PUB         "pub"
#define LOCATION_COMPONENT_OUTSIDE     "outside"
#define LOCATION_COMPONENT_MAZE        "maze"
// Leads astray whoever leaves without keeping their bearings. See
// /lib/location/components/crossing.c.
#define LOCATION_COMPONENT_CROSSING    "crossing"
// A rope tied to something opens an exit that is sealed without it. See
// /lib/location/components/rope.c.
#define LOCATION_COMPONENT_ROPE        "rope"
#define LOCATION_COMPONENT_PROPS       "props"
#define LOCATION_COMPONENT_SIGN        "sign"
#define LOCATION_COMPONENT_POST_OFFICE "post-office"
#define LOCATION_COMPONENT_BOARD       "board"
#define LOCATION_COMPONENT_QUEST_GIVER "quest-giver"
#define LOCATION_COMPONENT_VAULT       "vault"
// Shelves of books to read, copy or add to. See
// /lib/location/components/library.c.
#define LOCATION_COMPONENT_LIBRARY     "library"
// The entrance of a guild's house, and its main hall. See
// /lib/location/components/guild-entrance.c and guild-room.c.
#define LOCATION_COMPONENT_GUILD_ENTRANCE "guild-entrance"
#define LOCATION_COMPONENT_GUILD_ROOM     "guild-room"
// Where a job is taught: one takes it up or gives it up there. See
// /lib/location/components/job-entrance.c.
#define LOCATION_COMPONENT_JOB_ENTRANCE   "job-entrance"
// The main hall of the house where a job is taught. See
// /lib/location/components/job-room.c.
#define LOCATION_COMPONENT_JOB_ROOM       "job-room"
// Something that happens to whoever enters carrying a given mark. See
// /lib/location/components/trigger.c.
#define LOCATION_COMPONENT_TRIGGER     "trigger"

// Cartography-contributing components. A location that carries any of
// these tells the sector storage what kind of terrain it occupies, so
// the sector can classify itself (query_sector_type) and the world map
// renderer can pick a glyph. See include/maps/sector.h.
#define LOCATION_COMPONENT_CITY        "city"
#define LOCATION_COMPONENT_ROAD        "road"
#define LOCATION_COMPONENT_FOREST      "forest"
#define LOCATION_COMPONENT_COAST       "coast"
// A place where one swims instead of walking. See
// /lib/location/components/water.c.
#define LOCATION_COMPONENT_WATER       "water"
// A cell that is a bridge and water at once. See /lib/location/components/bridge.c.
#define LOCATION_COMPONENT_BRIDGE      "bridge"
#define LOCATION_COMPONENT_UNDERGROUND "underground"
#define LOCATION_COMPONENT_FARM        "farm"
// Terrain kinds that only name the ground, for the world map. See
// /lib/location/components/plains.c, swamp.c, mountain.c, taiga.c and ruins.c.
#define LOCATION_COMPONENT_PLAINS      "plains"
#define LOCATION_COMPONENT_SWAMP       "swamp"
#define LOCATION_COMPONENT_MOUNTAIN    "mountain"
#define LOCATION_COMPONENT_TAIGA       "taiga"
#define LOCATION_COMPONENT_RUINS       "ruins"
// A buildable empty lot, and a dwelling raised on one. See
// /lib/location/components/plot.c and home.c.
#define LOCATION_COMPONENT_PLOT        "plot"
#define LOCATION_COMPONENT_HOME        "home"
#define LOCATION_COMPONENT_TEMPLE      "temple"
// A smithy: a forge where metal is worked and the repair skill is used. See
// /lib/location/components/blacksmith.c.
#define LOCATION_COMPONENT_BLACKSMITH  "blacksmith"
// A cell: whoever serves a sentence cannot leave it before the time is up, and
// may try a bribe. The office where fines are paid and bail posted. See
// /lib/location/components/jail.c and fines.c.
#define LOCATION_COMPONENT_JAIL        "jail"
#define LOCATION_COMPONENT_FINES       "fines"

#endif // LOCATION_H
