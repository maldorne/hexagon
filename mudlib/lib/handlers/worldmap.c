// World-map renderer. Draws a sector-scale bird's-eye map — one glyph
// per 10x10 sector — around a given world coordinate. Called from
// player and admin commands, and (later) from anything that wants to
// show a snapshot of the surrounding territory: quest handers, travel
// commands, gazetteer entries, etc.
//
// Layers:
//   1. Compute the sector index for the target coord and clip a
//      viewport of `width x height` sectors centred on it.
//   2. For each sector in the viewport, take the glyph code its map's index
//      holds (/lib/location/map-index.c), written whenever the sector changes
//      (see sector_glyph here and save_me() of /lib/location/sector.c):
//      drawing a map loads no sector and works no glyph out.
//   3. A sector's glyph comes from its dominant type (query_sector_type) and
//      its border crossings: city sectors are a solid block (with an optional
//      wall overlay), or the bold green forest glyph when half or more of
//      their city locations are forest; road/path sectors are box-drawing
//      joins, one light line for both (see _arms_code).
//      Rivers (the swum exits of flowing water, see sector.c query_border_rivers)
//      are drawn the same way in blue, over roads, farmland and open land but
//      under cities, forests, taiga, mountains and swamps; where a road crosses
//      a straight river it is a bridge (see bridge_code).
//   4. Where a city wall meets a road, the wall glyph swaps to a wall/road
//      hybrid so the road visibly enters the settlement.
//   5. Overwrite the viewer's own sector with '@'.
//
// Sector storage is looked up through the maps handler, which already
// caches loaded sector objects. To avoid repeated file_size probes for
// non-existent sectors within one render, we memoise the sector lookup
// inside this handler for the duration of a single render() call.

#include <mud/config.h>
#include <sector/sector.h>
#include <maps/glyphs.h>
#include <room/location.h>
#include <areas/area.h>

inherit "/lib/core/object.c";

// ---------------------------------------------------------------
//  Glyph table
// ---------------------------------------------------------------

#define GLYPH_EMPTY       " "
#define GLYPH_PLAYER      "@"

// Toggle for the decorative city-wall overlay (see _overlay_city_walls).
// Walls are a post-processing pass drawn AROUND contiguous city regions
// to highlight where a settlement is; they do not correspond one-to-one
// with the underlying sectors. Set to 0 to render cities as plain solid
// blocks with no surrounding wall.
//
// Temporarily disabled (0): cities render as solid blocks for now. Flip
// back to 1 to re-enable the wall overlay; the overlay code is left intact.
#define WORLDMAP_CITY_WALLS 0

// Toggle for the river line: 1 draws rivers with double box lines, 0 with
// single ones. Either way they are blue.
#define WORLDMAP_RIVER_DOUBLE 1

// Colour of the river line.
#define COLOUR_RIVER  "%^BLUE%^"

// A river that never reaches a border of its sector (it starts and ends in it).
#define GLYPH_RIVER_SPOT (chr(194) + chr(176))     // U+00B0 degree sign

// per-render sector cache. Reset at the top of every render(); safe
// because DGD executes each mudlib call chain atomically — there is no
// interleaving of two renders on the same handler.
private mapping sector_cache;
// Same lifetime: the border summaries and the river flag of each sector, which
// cost a walk over the whole exit graph and are asked for several times.
private mapping border_cache;
// The sectors handler, looked up once per render (load_object is costly).
private object sectors;
// Same for the areas handler.
private object areas;
// The level of the map being drawn, from its world map index:
// ([ sy : ([ sx : ({ glyph code, city directory }) ]) ]).
private mapping index_level;

// Every glyph a sector can show, as it is drawn, indexed by its code (see
// MAP_GLYPH_* in include/maps/glyphs.h). Built once, so the chr() sequences
// of the glyph header are evaluated a single time.
private string * glyphs;

void create()
{
  string * way_table, * river_table;
  int i, n, s, e, w;

  ::create();
  sector_cache = ([ ]);
  border_cache = ([ ]);

  // the box tables are indexed n*27 + s*9 + e*3 + w (an arm weighs 0, 1 or
  // 2) and n*8 + s*4 + e*2 + w; the map draws every way with a light line
  way_table = BOX_GLYPHS_INDEXED;
  river_table = BOX_GLYPHS_DOUBLE;

  glyphs = MAP_GLYPHS + allocate(MAP_GLYPH_COUNT - sizeof(MAP_GLYPHS));

  // terrain kinds; ORANGE is the dark yellow of the ANSI palette, the closest
  // thing to brown (see /lib/handlers/terminal.c)
  glyphs[MAP_GLYPH_PLAINS_A] = GLYPH_MAP_PLAINS_A;
  glyphs[MAP_GLYPH_PLAINS_B] = GLYPH_MAP_PLAINS_B;
  glyphs[MAP_GLYPH_SWAMP]    = "%^GREEN%^" + GLYPH_MAP_SWAMP + "%^RESET%^";
  glyphs[MAP_GLYPH_MOUNTAIN] = "%^ORANGE%^" + GLYPH_MAP_MOUNTAIN + "%^RESET%^";
  glyphs[MAP_GLYPH_TAIGA] = "%^GREEN%^" + GLYPH_MAP_TAIGA + "%^RESET%^";
  glyphs[MAP_GLYPH_RUINS]    = GLYPH_MAP_RUINS;
  // the shore in the colour of the water, a beach in the colour of sand
  glyphs[MAP_GLYPH_COAST]    = COLOUR_RIVER + GLYPH_MAP_COAST + "%^RESET%^";
  glyphs[MAP_GLYPH_BEACH]    = "%^YELLOW%^" + GLYPH_MAP_COAST + "%^RESET%^";

  // a bridge is part of the road, so it is drawn in the road's plain colour
  glyphs[MAP_GLYPH_BRIDGE_V] =
    (WORLDMAP_RIVER_DOUBLE ? GLYPH_CROSS_DBL_V : GLYPH_BOX_1111);
  glyphs[MAP_GLYPH_BRIDGE_H] =
    (WORLDMAP_RIVER_DOUBLE ? GLYPH_CROSS_DBL_H : GLYPH_BOX_1111);

  for (i = 0; i < 16; i++)
  {
    n = (i >> 3) & 1;
    s = (i >> 2) & 1;
    e = (i >> 1) & 1;
    w = i & 1;
    glyphs[MAP_GLYPH_WAYS + i] = way_table[n * 27 + s * 9 + e * 3 + w];
    // a river with no arm starts and ends in its sector; one arm alone is the
    // full line of its axis (there is no half-line in the double set)
    glyphs[MAP_GLYPH_RIVERS + i] = COLOUR_RIVER +
      (!i ? GLYPH_RIVER_SPOT :
       (WORLDMAP_RIVER_DOUBLE ? river_table[i] :
        way_table[n * 27 + s * 9 + e * 3 + w])) + "%^RESET%^";
  }
}

// Memoised sect->query_border_ways() / query_border_rivers() / has_river() for
// the sector at (sx, sy, sz), keyed by `kind` and the coordinates.
private mixed _memo(string kind, object sect, int sx, int sy, int sz)
{
  string key;
  mixed value;

  key = kind + sx + "_" + sy + "_" + sz;
  if (!undefinedp(border_cache[key]))
    return border_cache[key];

  switch (kind)
  {
    case "w": value = sect->query_border_ways(); break;
    case "r": value = sect->query_border_rivers(); break;
    default:  value = sect->has_river(); break;
  }

  // a nil result is stored as an empty mapping (callers treat both alike)
  if (value == nil)
    value = ([ ]);
  border_cache[key] = value;
  return value;
}

// Load the sector at (sx, sy, sz), or nil if no sector.o exists.
// Memoised for the current render pass — the same neighbour sector is
// consulted by the four wall/road decisions around it.
private object cached_sector(string game, string map_name,
                             int sx, int sy, int sz)
{
  string key;
  object sect;

  key = sx + "_" + sy + "_" + sz;
  if (!undefinedp(sector_cache[key]))
    return sector_cache[key];
  if (sector_cache["none" + key])
    return nil;

  // nil when the sector.o does not exist: a coordinate nobody ever wrote is
  // drawn as empty space, not materialised on disk. A sector already in memory
  // is returned without asking the file system, the costly part of a render.
  sect = sectors->query_sector_for_coord(
           game, map_name, sx * SECTOR_SIZE, sy * SECTOR_SIZE, sz * SECTOR_SIZE);

  // storing nil in a mapping deletes the key, so a missing sector is
  // remembered under its own key
  if (sect)
    sector_cache[key] = sect;
  else
    sector_cache["none" + key] = 1;
  return sect;
}

private string neighbour_type(string game, string map_name,
                              int sx, int sy, int sz, int dx, int dy)
{
  object sect;
  sect = cached_sector(game, map_name, sx + dx, sy + dy, sz);
  if (!sect) return SECTOR_TYPE_NONE;
  return sect->query_sector_type();
}

// Weight of the way crossing border `b`: 1 if a path or a road crosses it
// (both are drawn with one light line), else 0.
private int _border_weight(mapping borders, string b)
{
  return (mappingp(borders) && arrayp(borders[b]) && sizeof(borders[b])) ? 1 : 0;
}

// Whether any border of this sector carries a way the map draws.
private int _has_drawn_way(mapping borders)
{
  return _border_weight(borders, SECTOR_BORDER_N) ||
         _border_weight(borders, SECTOR_BORDER_S) ||
         _border_weight(borders, SECTOR_BORDER_E) ||
         _border_weight(borders, SECTOR_BORDER_W);
}

// Weight of one arm, reconciled with the neighbour across that border. The
// arm is drawn if EITHER this sector or the neighbour records a way on the
// shared border, so asymmetric border data still joins up instead of leaving
// a broken line.
private int _arm_weight(mapping borders, string own_border,
                        string game, string map_name,
                        int nsx, int nsy, int nsz, string opp_border)
{
  int own, nb;
  object neigh;

  own = _border_weight(borders, own_border);
  neigh = cached_sector(game, map_name, nsx, nsy, nsz);
  nb = neigh ? _border_weight(_memo("w", neigh, nsx, nsy, nsz), opp_border) : 0;
  return (nb > own) ? nb : own;
}

// The arm weights of a road/path sector as ({ n, s, e, w }), each 0 (none) or
// 1 (a way), taken from this sector's border crossings and reconciled with
// its neighbours.
private int * way_arms(object sect, string game, string map_name,
                       int sx, int sy, int sz)
{
  mapping borders;

  borders = _memo("w", sect, sx, sy, sz);
  if (!mappingp(borders)) borders = ([ ]);

  return ({
    _arm_weight(borders, SECTOR_BORDER_N, game, map_name, sx, sy + 1, sz, SECTOR_BORDER_S),
    _arm_weight(borders, SECTOR_BORDER_S, game, map_name, sx, sy - 1, sz, SECTOR_BORDER_N),
    _arm_weight(borders, SECTOR_BORDER_E, game, map_name, sx + 1, sy, sz, SECTOR_BORDER_W),
    _arm_weight(borders, SECTOR_BORDER_W, game, map_name, sx - 1, sy, sz, SECTOR_BORDER_E)
  });
}

// The part of a way or river code that says which arms ({ n, s, e, w }) it
// draws. A way with a single arm is a half-line stub (a dead-end).
//
// Neighbour convention (from guess_coordinates): north = larger y, south =
// smaller y, east = larger x, west = smaller x.
private int _arms_code(int * arms)
{
  return (arms[0] ? 8 : 0) + (arms[1] ? 4 : 0) + (arms[2] ? 2 : 0) +
         (arms[3] ? 1 : 0);
}

// Whether the sector at (nsx, nsy, nsz) records a river on its `opp_border`.
private int _neighbour_river(string game, string map_name,
                             int nsx, int nsy, int nsz, string opp_border)
{
  object neigh;
  mapping nb;

  neigh = cached_sector(game, map_name, nsx, nsy, nsz);
  if (!neigh)
    return 0;
  nb = _memo("r", neigh, nsx, nsy, nsz);
  return mappingp(nb) && arrayp(nb[opp_border]) && sizeof(nb[opp_border]);
}

// The arms of a river sector as ({ n, s, e, w }), each 0 or 1. An arm is
// drawn if either this sector or the neighbour across it records the river on
// the shared border, so a river never breaks at a sector seam.
private int * river_arms(object sect, string game, string map_name,
                         int sx, int sy, int sz)
{
  mapping borders;
  int n, s, e, w;

  borders = _memo("r", sect, sx, sy, sz);
  if (!mappingp(borders)) borders = ([ ]);

  n = (arrayp(borders[SECTOR_BORDER_N]) && sizeof(borders[SECTOR_BORDER_N])) ||
      _neighbour_river(game, map_name, sx, sy + 1, sz, SECTOR_BORDER_S);
  s = (arrayp(borders[SECTOR_BORDER_S]) && sizeof(borders[SECTOR_BORDER_S])) ||
      _neighbour_river(game, map_name, sx, sy - 1, sz, SECTOR_BORDER_N);
  e = (arrayp(borders[SECTOR_BORDER_E]) && sizeof(borders[SECTOR_BORDER_E])) ||
      _neighbour_river(game, map_name, sx + 1, sy, sz, SECTOR_BORDER_W);
  w = (arrayp(borders[SECTOR_BORDER_W]) && sizeof(borders[SECTOR_BORDER_W])) ||
      _neighbour_river(game, map_name, sx - 1, sy, sz, SECTOR_BORDER_E);

  return ({ n, s, e, w });
}

// The code of a sector a road and a river both cross. `road` and `river` are
// the arms of each: a river running one way only (one or both of its arms,
// as where it rises or meets another) with the road reaching both of its
// banks is a bridge, whatever other arms the road has; otherwise the river is
// drawn, so it reads unbroken from source to mouth.
private int bridge_code(int * road, int * river)
{
  int river_h, river_v;

  river_h = (river[2] || river[3]) && !river[0] && !river[1];
  river_v = (river[0] || river[1]) && !river[2] && !river[3];

  if (river_v && road[2] && road[3])
    return MAP_GLYPH_BRIDGE_V;
  if (river_h && road[0] && road[1])
    return MAP_GLYPH_BRIDGE_H;

  return MAP_GLYPH_RIVERS + _arms_code(river);
}


/**
 * Which of the two plains glyphs a sector shows: a hash of its coordinates, so
 * the choice looks random but is the same on every redraw.
 */
private int _plains_code(int sx, int sy, int sz)
{
  int h;

  h = sx * 374761393 + sy * 668265263 + sz * 1274126177;
  h = h ^ (h >> 13);
  h = h * 1103515245 + 12345;
  h = h ^ (h >> 16);
  return (h & 1) ? MAP_GLYPH_PLAINS_B : MAP_GLYPH_PLAINS_A;
}

// A sector with more locations than this carrying the ruins component draws
// the ruins glyph over everything but a city.
#define RUINS_OVERRIDE_MIN 5

// Display priority for a sector (highest first):
//   1. city        — any city presence wins; drawn as a solid block, the
//                    surrounding wall added later by _overlay_city_walls.
//                    A forest city (half or more of its city locations are
//                    also forest) shows the bold green forest glyph instead
//                    and is a city for the wall and the name
//   2. many ruins  — more than RUINS_OVERRIDE_MIN locations with the ruins
//                    component draw the ruins glyph whatever the majority,
//                    road, river or terrain; only a city outranks it
//   3. road / path — if the sector has road/path exits, draw the way glyph;
//                    with a river through it as well it is a bridge when the
//                    road crosses it, and the river otherwise (bridge_code)
//   4. forest, taiga, mountain, swamp — win over a river, so they keep
//                    their size
//   5. river       — blue line, over farmland and any other terrain
//   6. ruins, plains — below the river
//   7. majority    — otherwise the dominant remaining terrain type
private int sector_code(string game, string map_name, int sx, int sy, int sz)
{
  object sect;
  string type;
  mapping borders;
  int * road_arms, * river_arms_here;
  int river;

  sect = cached_sector(game, map_name, sx, sy, sz);
  if (!sect) return MAP_GLYPH_EMPTY;

  // a single city location paints the whole sector (present-wins), so two
  // neighbouring cities never merge through the terrain between them
  // unless half or more of its city locations are forest: then the woods glyph
  // in bold green (the sector is still a city for walls and labels)
  if (sect->has_locations_of_type(SECTOR_TYPE_CITY))
    return sect->is_forest_city() ? MAP_GLYPH_FOREST_CITY : MAP_GLYPH_CITY;

  // many ruins locations paint the sector as ruins whatever the majority
  if (sect->count_locations_of_type(SECTOR_TYPE_RUINS) > RUINS_OVERRIDE_MIN)
    return MAP_GLYPH_RUINS;

  // query_sector_type is the majority component, or the programmer-set
  // manual type for a sector with no locations of its own
  type = sect->query_sector_type();

  river = _memo("h", sect, sx, sy, sz);
  if (river)
    river_arms_here = river_arms(sect, game, map_name, sx, sy, sz);

  // Farmland is a landcover that shows THROUGH a track: a field sector is
  // only ever reached by a path from the settlement, which would otherwise
  // stamp the sector with a border way and hide the fields under a road
  // glyph. So a sector whose dominant component is farm renders as cultivated
  // land even when a path clips its edge -- ranked above the way glyph but
  // still below city, which always wins.
  // A river is drawn over it: it is water, not a track.
  if (type == SECTOR_TYPE_FARM)
  {
    if (river)
      return MAP_GLYPH_RIVERS + _arms_code(river_arms_here);
    return MAP_GLYPH_FARM;
  }

  borders = _memo("w", sect, sx, sy, sz);
  if (_has_drawn_way(borders))
  {
    road_arms = way_arms(sect, game, map_name, sx, sy, sz);
    if (river)
      return bridge_code(road_arms, river_arms_here);
    return MAP_GLYPH_WAYS + _arms_code(road_arms);
  }

  // a forest wins over a river running through it: the woods read as their
  // true size; so do the other terrain with a shape of its own
  if (type == SECTOR_TYPE_FOREST)      return MAP_GLYPH_FOREST;
  if (type == SECTOR_TYPE_TAIGA)       return MAP_GLYPH_TAIGA;
  if (type == SECTOR_TYPE_MOUNTAIN)    return MAP_GLYPH_MOUNTAIN;
  if (type == SECTOR_TYPE_SWAMP)       return MAP_GLYPH_SWAMP;

  if (river)
    return MAP_GLYPH_RIVERS + _arms_code(river_arms_here);

  if (type == SECTOR_TYPE_RUINS)       return MAP_GLYPH_RUINS;
  if (type == SECTOR_TYPE_PLAINS)      return _plains_code(sx, sy, sz);

  if (type == SECTOR_TYPE_CITY)        return MAP_GLYPH_CITY;
  if (type == SECTOR_TYPE_COAST)
    return sect->query_beach() ? MAP_GLYPH_BEACH : MAP_GLYPH_COAST;
  if (type == SECTOR_TYPE_UNDERGROUND) return MAP_GLYPH_UNDERGROUND;

  return MAP_GLYPH_EMPTY;
}

// Write a wall glyph into the grid, clamped to bounds. Only blank sectors
// are painted: the player marker and any real terrain glyph (another
// sector's fill — forest, road, a different city) show through, so the
// wall never hides map content, it only fills the empty margin around a
// settlement.
private void _put(string ** grid, int w, int h, int r, int c, string g)
{
  if (r < 0 || r >= h || c < 0 || c >= w)
    return;
  if (grid[r][c] != GLYPH_EMPTY)
    return;
  grid[r][c] = g;
}

// Line glyph for a wall sector, from the mask of which orthogonal
// neighbours are also wall sectors (N=1, S=2, E=4, W=8). This connects the
// ring of wall sectors into a continuous outline.
private string _wall_glyph(int mask)
{
  switch (mask)
  {
    case 1: case 2: case 3:  return GLYPH_BOX_1100;    // vertical
    case 4: case 8: case 12: return GLYPH_BOX_0011;    // horizontal
    case 5:  return GLYPH_BOX_1010;    // N+E
    case 6:  return GLYPH_BOX_0110;    // S+E
    case 9:  return GLYPH_BOX_1001;    // N+W
    case 10: return GLYPH_BOX_0101;    // S+W
    case 7:  return GLYPH_BOX_1110;   // N+S+E
    case 11: return GLYPH_BOX_1101;   // N+S+W
    case 13: return GLYPH_BOX_1011;   // N+E+W
    case 14: return GLYPH_BOX_0111;   // S+E+W
    case 15: return GLYPH_BOX_1111;  // N+S+E+W
  }
  return GLYPH_BOX_0011;                // isolated stub - arbitrary
}

// Post-processing pass: draw a wall that hugs the exact outline of every
// city region. A wall sector is any non-city sector touching a city sector
// (8-connectivity, so corners are included); its glyph connects to the
// neighbouring wall sectors, so the ring follows the silhouette — dipping
// in and out around protrusions instead of squaring off a bounding box.
// City sectors keep their solid fill, so there are no empty sectors inside
// the wall.
private void _overlay_city_walls(string ** grid, int ** is_city,
                                 int w, int h)
{
  int ** is_wall;
  int r, c;

  // pass 1: a wall sector is a non-city sector adjacent (8-dir) to any city
  is_wall = allocate(h);
  for (r = 0; r < h; r++)
  {
    is_wall[r] = allocate_int(w);
    for (c = 0; c < w; c++)
    {
      int dr, dc, touch;

      if (is_city[r][c])
        continue;

      touch = 0;
      for (dr = -1; dr <= 1 && !touch; dr++)
        for (dc = -1; dc <= 1 && !touch; dc++)
        {
          int nr, nc;
          if (!dr && !dc) continue;
          nr = r + dr;
          nc = c + dc;
          if (nr >= 0 && nr < h && nc >= 0 && nc < w && is_city[nr][nc])
            touch = 1;
        }
      is_wall[r][c] = touch;
    }
  }

  // pass 2: connect the ring — each wall sector's glyph follows its
  // orthogonal wall neighbours
  for (r = 0; r < h; r++)
    for (c = 0; c < w; c++)
    {
      int mask;

      if (!is_wall[r][c])
        continue;

      mask = 0;
      if (r - 1 >= 0 && is_wall[r - 1][c]) mask |= 1;   // N (up)
      if (r + 1 < h  && is_wall[r + 1][c]) mask |= 2;   // S (down)
      if (c + 1 < w  && is_wall[r][c + 1]) mask |= 4;   // E
      if (c - 1 >= 0 && is_wall[r][c - 1]) mask |= 8;   // W

      _put(grid, w, h, r, c, _wall_glyph(mask));
    }
}

// The code of the glyph a sector shows on the world map (MAP_GLYPH_* in
// include/maps/glyphs.h), worked out from its own records and its four
// neighbours' border crossings. A sector asks for it whenever it changes and
// keeps it (see save_me() of /lib/location/sector.c), so drawing a map only
// reads the stored codes.
int sector_glyph(object sect)
{
  mixed * place;

  place = sect ? sect->query_map_place() : nil;
  if (!place)
    return MAP_GLYPH_EMPTY;

  sector_cache = ([ ]);
  border_cache = ([ ]);
  sectors = load_object(SECTORS_HANDLER);
  areas = load_object(AREA_HANDLER);
  // the sector asking may be one the handler has not cached yet
  sector_cache[place[2] + "_" + place[3] + "_" + place[4]] = sect;

  return sector_code(place[0], place[1], place[2], place[3], place[4]);
}

// The name of the area a directory of city locations belongs to, asked once
// per directory in a render: its exploration name, or its root area's for a
// part of a town that has none of its own. "" when it has none.
private string _area_name(string dir)
{
  object area;
  string name;

  if (!undefinedp(border_cache["a" + dir]))
    return border_cache["a" + dir];

  name = nil;
  area = areas->query_area(dir);
  if (area)
  {
    name = area->query_exploration_name();
    if ((!name || !strlen(name)) && area->query_root_area())
      name = area->query_root_area()->query_exploration_name();
  }

  border_cache["a" + dir] = name ? name : "";
  return border_cache["a" + dir];
}

// What a city sector is named after: the area of the directory holding most
// of its city locations, from the map's index, or "".
private string _cell_city_name(int sx, int sy)
{
  mixed * cell;

  if (!index_level[sy] || !(cell = index_level[sy][sx]) || !strlen(cell[1]))
    return "";
  return _area_name(cell[1]);
}

// A label's characters, one per map cell: a UTF-8 sequence stays whole.
private string * _cells_of(string text)
{
  string * out;
  int i, start;

  out = ({ });
  for (i = 0; i < strlen(text); i = start)
  {
    start = i + 1;
    while (start < strlen(text) && text[start] >= 128 && text[start] < 192)
      start++;
    out += ({ text[i..start - 1] });
  }
  return out;
}

// The cities whose labels are being placed, and the one being labelled now:
// set while the labels go in, so the fit test can keep each label nearer its
// own city than any other.
private static mixed * label_cities;
private static int label_own;

// How far a label of `n` cells on row `r` from column `c` is from a city's
// box (`city` as _overlay_city_names builds it): the fewest steps, corners
// counting as one, from any of its letters to the box.
private int _box_distance(mixed * city, int r, int c, int n)
{
  int dr, dc;

  if (r < city[1])
    dr = city[1] - r;
  else if (r > city[2])
    dr = r - city[2];

  if (c + n - 1 < city[3])
    dc = city[3] - (c + n - 1);
  else if (c > city[4])
    dc = c - city[4];

  return dr > dc ? dr : dc;
}

// Whether a label of `n` cells fits on row `r` from column `c`: every cell it
// would take is blank, and so is the one at each end, unless that end is the
// edge of the map, so it neither covers nor touches anything already drawn;
// and it is nearer the city it names than any other one on the map.
private int _label_fits(string ** grid, int w, int h, int r, int c, int n)
{
  int i, k, own;

  if (r < 0 || r >= h || c < 0 || c + n > w)
    return 0;
  for (i = c - 1; i <= c + n; i++)
  {
    if (i < 0 || i >= w)
      continue;
    if (grid[r][i] != GLYPH_EMPTY)
      return 0;
  }

  own = _box_distance(label_cities[label_own], r, c, n);
  for (k = 0; k < sizeof(label_cities); k++)
    if (k != label_own && _box_distance(label_cities[k], r, c, n) <= own)
      return 0;

  return 1;
}

// A place for a label of `n` cells on the row above the city (`above`), else
// the row below: centred on it if it fits, else slid a cell at a time to
// either side, as long as it still overlaps the city's columns. Nil if neither
// row has room.
private int * _spot_above_or_below(string ** grid, int w, int h, int above,
                                   int below, int cmin, int cmax, int n)
{
  int * rows;
  int centre, shift, r, start;

  rows = ({ above, below });
  centre = (cmin + cmax) / 2 - n / 2;

  for (r = 0; r < sizeof(rows); r++)
    for (shift = 0; shift <= n; shift++)
    {
      start = centre - shift;
      if (start + n - 1 >= cmin && start <= cmax &&
          _label_fits(grid, w, h, rows[r], start, n))
        return ({ rows[r], start });
      start = centre + shift;
      if (shift && start + n - 1 >= cmin && start <= cmax &&
          _label_fits(grid, w, h, rows[r], start, n))
        return ({ rows[r], start });
    }

  return nil;
}

// Last resort for a label of `n` cells: the spot nearest the city's centre
// with some letter at most two cells from its box. Only the rows and columns
// that close to the city are tried.
private int * _spot_near(string ** grid, int w, int h, mixed * city, int n)
{
  int * best;
  int r, c, dist, best_dist, crow, ccol;

  crow = (city[1] + city[2]) / 2;
  ccol = (city[3] + city[4]) / 2;

  for (r = city[1] - 2; r <= city[2] + 2; r++)
    for (c = city[3] - 1 - n; c <= city[4] + 2; c++)
    {
      if (!_label_fits(grid, w, h, r, c, n))
        continue;

      // distance from the label's middle to the city's
      dist = abs(r - crow) * 2 + abs(c + n / 2 - ccol);
      if (!best || dist < best_dist)
      {
        best = ({ r, c });
        best_dist = dist;
      }
    }

  return best;
}

// Name each city on the map next to it, where there is room: to its right on
// its middle row, else to its left, else above or below it (see
// _spot_above_or_below), else anywhere close enough (see _spot_near). A label
// is only drawn over blank cells; a city with no room for its name goes
// unnamed.
private void _overlay_city_names(string ** grid, int ** is_city, int w, int h,
                                 string game, string map_name,
                                 int col0, int row_top, int sz)
{
  int ** seen;
  string ** cell_names;
  mixed * cities;
  int r, c, k;

  // the name each city cell is after, read once
  seen = allocate(h);
  cell_names = allocate(h);
  for (r = 0; r < h; r++)
  {
    seen[r] = allocate_int(w);
    cell_names[r] = allocate(w);
    for (c = 0; c < w; c++)
      if (is_city[r][c])
        cell_names[r][c] = _cell_city_name(col0 + c, row_top - r);
  }

  // every city first, so a label can be kept nearer its own than any other:
  // each is ({ members, rmin, rmax, cmin, cmax, name })
  cities = ({ });
  for (r = 0; r < h; r++)
    for (c = 0; c < w; c++)
    {
      mixed * stack, * members;
      mapping votes;
      string name, * names, own;
      int rmin, rmax, cmin, cmax, best, i;

      if (!is_city[r][c] || seen[r][c])
        continue;

      // the city: every city cell joined to this one, corners included, but
      // two towns side by side stay apart: a cell named after another place
      // than the city's does not join it
      own = cell_names[r][c];
      votes = ([ ]);
      rmin = rmax = r;
      cmin = cmax = c;
      stack = ({ ({ r, c }) });
      members = ({ });
      seen[r][c] = 1;
      while (sizeof(stack))
      {
        int cr, cc, dr, dc;

        cr = stack[0][0];
        cc = stack[0][1];
        members += ({ stack[0] });
        stack = stack[1..];
        if (cr < rmin) rmin = cr;
        if (cr > rmax) rmax = cr;
        if (cc < cmin) cmin = cc;
        if (cc > cmax) cmax = cc;
        if (strlen(cell_names[cr][cc]))
          votes[cell_names[cr][cc]] = (votes[cell_names[cr][cc]] ?
                                       votes[cell_names[cr][cc]] : 0) + 1;

        for (dr = -1; dr <= 1; dr++)
          for (dc = -1; dc <= 1; dc++)
            if (cr + dr >= 0 && cr + dr < h && cc + dc >= 0 && cc + dc < w &&
                is_city[cr + dr][cc + dc] && !seen[cr + dr][cc + dc] &&
                (own == "" || cell_names[cr + dr][cc + dc] == "" ||
                 cell_names[cr + dr][cc + dc] == own))
            {
              if (own == "")
                own = cell_names[cr + dr][cc + dc];
              seen[cr + dr][cc + dc] = 1;
              stack += ({ ({ cr + dr, cc + dc }) });
            }
      }

      names = map_indices(votes);
      name = sizeof(names) ? names[0] : nil;
      best = name ? votes[name] : 0;
      for (i = 1; i < sizeof(names); i++)
        if (votes[names[i]] > best)
        {
          name = names[i];
          best = votes[name];
        }

      cities += ({ ({ members, rmin, rmax, cmin, cmax, name }) });
    }


  // the fit test keeps each label nearer its city than any other
  label_cities = cities;

  for (k = 0; k < sizeof(cities); k++)
  {
    string * cells;
    int rmin, rmax, cmin, cmax, mid, n, i;
    int * spot;

    if (!cities[k][5])
      continue;

    rmin = cities[k][1];
    rmax = cities[k][2];
    cmin = cities[k][3];
    cmax = cities[k][4];
    cells = _cells_of(cities[k][5]);
    n = sizeof(cells);
    mid = (rmin + rmax) / 2;
    label_own = k;

    if (_label_fits(grid, w, h, mid, cmax + 2, n))
      spot = ({ mid, cmax + 2 });
    else if (_label_fits(grid, w, h, mid, cmin - 1 - n, n))
      spot = ({ mid, cmin - 1 - n });
    else
      spot = _spot_above_or_below(grid, w, h, rmin - 1, rmax + 1,
                                  cmin, cmax, n);
    if (!spot)
      spot = _spot_near(grid, w, h, cities[k], n);


    if (!spot)
      continue;
    for (i = 0; i < n; i++)
      grid[spot[0]][spot[1] + i] = cells[i];
  }

  label_cities = nil;
}

// Render a `width x height` sector viewport centred on the given world
// coord. The viewer's sector is overlaid with '@' regardless of what
// terrain occupies it.
// Draw a map of anywhere. Give it a world coordinate to centre on, the game
// and map to read, and a viewport size; no player is needed. `marker`
// (default 0) stamps a '@' on the centre sector when set, for a "you are here"
// focus point.
// `labels` names the cities; `label_room` is how many blank columns the caller
// has beside the map, where a name may also go: half go to each side.
string render(int center_x, int center_y, int center_z,
              string game, string map_name, int width, int height,
              varargs int marker, int labels, int label_room)
{
  int sx0, sy0, sz0;
  int col0, row_top;
  int row_i, col, left, code;
  string ** grid;
  int ** is_city;
  string result;

  sector_cache = ([ ]);
  border_cache = ([ ]);
  sectors = load_object(SECTORS_HANDLER);
  areas = load_object(AREA_HANDLER);

  sx0 = SECTOR_INDEX(center_x);
  sy0 = SECTOR_INDEX(center_y);
  sz0 = SECTOR_INDEX(center_z);

  // top-left of the viewport, expressed in sector indices. `row_top`
  // is the highest y in the viewport because on screen y grows downward.
  col0    = sx0 - width / 2;
  row_top = sy0 + height / 2;

  // base fill: one glyph per sector, read off the map's index (no sector is
  // loaded), plus a parallel city mask the wall overlay reads. The mask is
  // captured here, before the player marker is stamped in, so a marker sitting
  // on a city sector never breaks the wall.
  index_level = sectors->query_map_index(game, map_name)->query_level(sz0);
  grid = allocate(height);
  is_city = allocate(height);
  for (row_i = 0; row_i < height; row_i++)
  {
    mapping row;

    grid[row_i] = allocate(width);
    is_city[row_i] = allocate_int(width);
    row = index_level[row_top - row_i];
    for (col = 0; col < width; col++)
    {
      code = (row && row[col0 + col]) ? row[col0 + col][0] : MAP_GLYPH_EMPTY;
      grid[row_i][col] = glyphs[code];
      // city mask for the wall overlay and the labels, read off the code (a
      // forest city is a city too) — still before the player marker
      // overwrites it below.
      is_city[row_i][col] = code == MAP_GLYPH_CITY || code == MAP_GLYPH_FOREST_CITY;
    }
  }

  // centre marker (before the overlay, which never overwrites it)
  if (marker)
  {
    int prow, pcol;
    prow = row_top - sy0;
    pcol = sx0 - col0;
    if (prow >= 0 && prow < height && pcol >= 0 && pcol < width)
      grid[prow][pcol] = GLYPH_PLAYER;
  }

  if (WORLDMAP_CITY_WALLS)
    _overlay_city_walls(grid, is_city, width, height);

  // names last, so they only take cells nothing else has drawn on, in the map
  // or in the blank columns the caller has beside it
  // half the blank columns go to the left of the map and half to the right,
  // so the map sits centred and a city at either edge has room for its name
  if (labels)
  {
    if (label_room > 0)
    {
      left = label_room / 2;
      for (row_i = 0; row_i < height; row_i++)
      {
        grid[row_i] = allocate(left) + grid[row_i] +
                      allocate(label_room - left);
        is_city[row_i] = allocate_int(left) + is_city[row_i] +
                         allocate_int(label_room - left);
        for (col = 0; col < width + label_room; col++)
          if (col < left || col >= left + width)
            grid[row_i][col] = GLYPH_EMPTY;
      }
    }
    _overlay_city_names(grid, is_city, width + (label_room > 0 ? label_room : 0),
                        height, game, map_name, col0 - left, row_top, sz0);
  }

  result = "";
  for (row_i = 0; row_i < height; row_i++)
    result += implode(grid[row_i], "") + "\n";

  return result;
}

// What a legend needs, by kind of sector: the glyph to look for in a rendered
// map and the same glyph as it is drawn there (with its colour). Ways are left
// out: a line reads as a road without being told.
mapping query_legend_glyphs()
{
  return ([
    "player":      ({ GLYPH_PLAYER, GLYPH_PLAYER }),
    "city":        ({ glyphs[MAP_GLYPH_CITY], glyphs[MAP_GLYPH_CITY] }),
    "forest":      ({ "%^GREEN%^" + GLYPH_MAP_FOREST, glyphs[MAP_GLYPH_FOREST] }),
    "forest-city": ({ glyphs[MAP_GLYPH_FOREST_CITY], glyphs[MAP_GLYPH_FOREST_CITY] }),
    "farm":        ({ GLYPH_MAP_FARM, glyphs[MAP_GLYPH_FARM] }),
    "plains":      ({ ({ GLYPH_MAP_PLAINS_A, GLYPH_MAP_PLAINS_B }), glyphs[MAP_GLYPH_PLAINS_A] }),
    "swamp":       ({ "%^GREEN%^" + GLYPH_MAP_SWAMP, glyphs[MAP_GLYPH_SWAMP] }),
    "mountain":    ({ "%^ORANGE%^" + GLYPH_MAP_MOUNTAIN, glyphs[MAP_GLYPH_MOUNTAIN] }),
    "taiga":       ({ "%^GREEN%^" + GLYPH_MAP_TAIGA, glyphs[MAP_GLYPH_TAIGA] }),
    "ruins":       ({ GLYPH_MAP_RUINS, glyphs[MAP_GLYPH_RUINS] }),
    "river":       ({ COLOUR_RIVER, glyphs[MAP_GLYPH_RIVERS + 12] }),
    "coast":       ({ COLOUR_RIVER + GLYPH_MAP_COAST, glyphs[MAP_GLYPH_COAST] }),
    "beach":       ({ "%^YELLOW%^" + GLYPH_MAP_COAST, glyphs[MAP_GLYPH_BEACH] }),
    "underground": ({ glyphs[MAP_GLYPH_UNDERGROUND], glyphs[MAP_GLYPH_UNDERGROUND] }),
  ]);
}

// Convenience: pull the world coord + map + game out of the viewer's
// environment and hand off to render(). `marker` (default 0) stamps the '@'
// on the viewer's own sector when set. Returns nil if the viewer is standing in
// something that has no world coord (legacy room, void, a container inside a
// container without location metadata).
string render_around(object viewer, int width, int height, varargs int marker,
                     int labels, int label_room)
{
  object env;
  int * coords;
  string map_name, file, game;

  if (!viewer) return nil;
  env = environment(viewer);
  if (!env) return nil;

  coords = env->query_coordinates();
  if (!coords || sizeof(coords) < 3) return nil;

  map_name = env->query_map_name();
  file = env->query_file_name();
  if (!map_name || !file) return nil;

  game = game_from_path(file);
  if (!game) return nil;

  return render(coords[0], coords[1], coords[2], game, map_name, width,
                height, marker, labels, label_room);
}
