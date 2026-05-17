
#define CARTOGRAPHY_HANDLER "/lib/handlers/cartography"

// Default viewport size for the map view, in cells. Each room occupies one
// cell, with the cell between two rooms holding the exit glyph (so a 15x15
// grid carries about 8x8 rooms in the worst case).
#define CART_DEFAULT_WIDTH   15
#define CART_DEFAULT_HEIGHT  15

// Semantic types stored in `cells[y][x]`. Renderers map these to glyphs
// (ASCII), colours, icons, JSON keys — whatever the consuming UI needs.

#define CART_EMPTY              0   // nothing here

// Room cells (a location occupies the cell). Ordered roughly by priority:
// when a room qualifies for several types, the highest-priority one wins
// (see _classify_room in cartography.c).
#define CART_ROOM               1   // plain room, no markers
#define CART_DOOR_ROOM          2   // exit through a door / gate
#define CART_UP_ROOM            3   // has an `up` exit (stairs / climb)
#define CART_DOWN_ROOM          4   // has a `down` exit
#define CART_COAST_ROOM         5   // borders water / sea
#define CART_QUEST_ROOM         6   // open quest available here
#define CART_FINISH_QUEST_ROOM  7   // quest waiting to be turned in here
#define CART_ADVENTURER_ROOM    8   // a friendly group member is here
#define CART_GUARD_ROOM         9   // friendly guard is here
#define CART_ENEMY_ROOM        10   // hostile to the viewer is here
#define CART_MAZE_ROOM         11   // ghost cell standing in for a maze
                                    // location reached from a normal room;
                                    // renderers paint it as '?' and never
                                    // explore further from here

// Exit-segment cells (the cell between two rooms). The renderer paints
// the corresponding line glyph: '|', '---', '/' or '\'.
#define CART_VERTICAL_EXIT     20
#define CART_HORIZONTAL_EXIT   21
#define CART_SLASH_EXIT        22
#define CART_BACKSLASH_EXIT    23
