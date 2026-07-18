// World-map renderer. Draws a sector-scale bird's-eye map — one glyph
// per 10x10 sector — around a given world coordinate. Called from
// player and admin commands, and (later) from anything that wants to
// show a snapshot of the surrounding territory: quest handers, travel
// commands, gazetteer entries, etc.
//
// Layers:
//   1. Compute the sector index for the target coord and clip a
//      viewport of `width x height` sectors centred on it.
//   2. For each cell, load the corresponding sector (if any) and pick
//      a base glyph from its dominant type (query_sector_type).
//   3. For city and road cells, replace the base glyph with a shape
//      chosen from the sector's 4-neighbourhood: city cells become box-
//      drawing wall segments around a solid interior; road cells become
//      double-line box-drawing joins.
//   4. Where a city wall meets a road, swap the plain wall glyph for a
//      hybrid (╨ ╞ ╥ ╡) so the road visibly enters the settlement.
//   5. Overwrite the viewer's own sector with '@'.
//
// Sector storage is looked up through the maps handler, which already
// caches loaded sector objects. To avoid repeated file_size probes for
// non-existent sectors within one render, we memoise the sector lookup
// inside this handler for the duration of a single render() call.

#include <mud/config.h>
#include <maps/maps.h>
#include <maps/sector.h>
#include <room/location.h>

inherit "/lib/core/object.c";

// ---------------------------------------------------------------
//  Glyph table
// ---------------------------------------------------------------

#define GLYPH_EMPTY       " "
#define GLYPH_PLAYER      "@"

#define GLYPH_FOREST      "♣"
#define GLYPH_COAST       "~"
#define GLYPH_UNDERGROUND "."

// city interior + single-line box wall segments
#define GLYPH_CITY_SOLID  "▓"
#define GLYPH_CITY_NW     "┌"
#define GLYPH_CITY_N      "─"
#define GLYPH_CITY_NE     "┐"
#define GLYPH_CITY_W      "│"
#define GLYPH_CITY_E      "│"
#define GLYPH_CITY_SW     "└"
#define GLYPH_CITY_S      "─"
#define GLYPH_CITY_SE     "┘"

// city wall glyphs when the outer neighbour is a road (single/double
// line hybrid — see docs/en/map_characters.txt)
#define GLYPH_CITY_N_ROAD "╨"
#define GLYPH_CITY_E_ROAD "╞"
#define GLYPH_CITY_S_ROAD "╥"
#define GLYPH_CITY_W_ROAD "╡"

// road glyphs (double-line box drawing)
#define GLYPH_ROAD_NS     "║"
#define GLYPH_ROAD_EW     "═"
#define GLYPH_ROAD_NE     "╚"
#define GLYPH_ROAD_NW     "╝"
#define GLYPH_ROAD_SE     "╔"
#define GLYPH_ROAD_SW     "╗"
#define GLYPH_ROAD_NSE    "╠"
#define GLYPH_ROAD_NSW    "╣"
#define GLYPH_ROAD_NEW    "╩"
#define GLYPH_ROAD_SEW    "╦"
#define GLYPH_ROAD_NSEW   "╬"

// per-render sector cache. Reset at the top of every render(); safe
// because DGD executes each mudlib call chain atomically — there is no
// interleaving of two renders on the same handler.
private mapping sector_cache;

void create()
{
  ::create();
  sector_cache = ([ ]);
}

// Match maps.c's sector-index convention: sector n = n / 10 - (n < 0),
// so a coord of -1 lands in sector -1, not sector 0.
private int sector_index(int n)
{
  return n / 10 - (n < 0);
}

// Load the sector at (sx, sy, sz), or nil if no sector.o exists.
// Memoised for the current render pass — the same neighbour cell is
// consulted by the four wall/road decisions around it.
private object cached_sector(string game, string map_name,
                             int sx, int sy, int sz)
{
  string key, path;
  object sect;

  key = sx + "_" + sy + "_" + sz;
  if (!undefinedp(sector_cache[key]))
    return sector_cache[key];

  path = "/save/games/" + game + "/maps/" + map_name + "/" +
         sx + "/" + sy + "/" + sz + "/";

  // create_sector() would persist an empty sector.o for coords that
  // have never been populated; skip the call if the file is missing
  // so we render "no sector" as empty space instead of writing junk.
  if (file_size(path + "sector.o") < 0)
    sect = nil;
  else
    sect = load_object(MAPS_HANDLER)->create_sector(path);

  sector_cache[key] = sect;
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

// Pick the box-drawing wall glyph for a city sector based on which of
// its four cardinal neighbours are also city (interior vs edge vs
// corner) and, where the outer neighbour is a road, promote the wall
// to a road-join hybrid so the settlement visibly connects to it.
private string city_glyph(string game, string map_name,
                          int sx, int sy, int sz)
{
  int cn, cs, ce, cw, count;
  int rn, rs, re, rw;

  cn = (neighbour_type(game, map_name, sx, sy, sz, 0,  1) == SECTOR_TYPE_CITY);
  cs = (neighbour_type(game, map_name, sx, sy, sz, 0, -1) == SECTOR_TYPE_CITY);
  ce = (neighbour_type(game, map_name, sx, sy, sz, 1,  0) == SECTOR_TYPE_CITY);
  cw = (neighbour_type(game, map_name, sx, sy, sz,-1,  0) == SECTOR_TYPE_CITY);

  count = cn + cs + ce + cw;

  // interior (all 4 neighbours city) — plain solid block
  if (count == 4)
    return GLYPH_CITY_SOLID;

  // near-isolated (0-1 city neighbours) — treat as one lone block,
  // avoids emitting single-corner protrusions for weird 1-wide clumps
  if (count <= 1)
    return GLYPH_CITY_SOLID;

  rn = (neighbour_type(game, map_name, sx, sy, sz, 0,  1) == SECTOR_TYPE_ROAD);
  rs = (neighbour_type(game, map_name, sx, sy, sz, 0, -1) == SECTOR_TYPE_ROAD);
  re = (neighbour_type(game, map_name, sx, sy, sz, 1,  0) == SECTOR_TYPE_ROAD);
  rw = (neighbour_type(game, map_name, sx, sy, sz,-1,  0) == SECTOR_TYPE_ROAD);

  // three neighbours city — cell is a straight wall segment on the
  // remaining side. The outer road, if any, promotes the plain wall.
  if (cs && ce && cw && !cn) return rn ? GLYPH_CITY_N_ROAD : GLYPH_CITY_N;
  if (cn && ce && cw && !cs) return rs ? GLYPH_CITY_S_ROAD : GLYPH_CITY_S;
  if (cn && cs && cw && !ce) return re ? GLYPH_CITY_E_ROAD : GLYPH_CITY_E;
  if (cn && cs && ce && !cw) return rw ? GLYPH_CITY_W_ROAD : GLYPH_CITY_W;

  // two neighbours city — corner segments
  if (cs && ce && !cn && !cw) return GLYPH_CITY_NW;
  if (cs && cw && !cn && !ce) return GLYPH_CITY_NE;
  if (cn && ce && !cs && !cw) return GLYPH_CITY_SW;
  if (cn && cw && !cs && !ce) return GLYPH_CITY_SE;

  // two neighbours forming a strip (opposite sides) — fall back on the
  // matching straight segment
  if (cn && cs && !ce && !cw) return GLYPH_CITY_W;
  if (ce && cw && !cn && !cs) return GLYPH_CITY_N;

  return GLYPH_CITY_SOLID;
}

// Pick the double-line box-drawing glyph for a road sector from the
// mask of which cardinal neighbours are also road.
private string road_glyph(string game, string map_name,
                          int sx, int sy, int sz)
{
  int mask;

  mask  = (neighbour_type(game, map_name, sx, sy, sz, 0,  1) == SECTOR_TYPE_ROAD) ? 1 : 0;
  mask |= (neighbour_type(game, map_name, sx, sy, sz, 0, -1) == SECTOR_TYPE_ROAD) ? 2 : 0;
  mask |= (neighbour_type(game, map_name, sx, sy, sz, 1,  0) == SECTOR_TYPE_ROAD) ? 4 : 0;
  mask |= (neighbour_type(game, map_name, sx, sy, sz,-1,  0) == SECTOR_TYPE_ROAD) ? 8 : 0;

  switch (mask)
  {
    case 0:  return GLYPH_ROAD_EW;   // isolated stub — arbitrary
    case 1:  return GLYPH_ROAD_NS;   // stub facing north
    case 2:  return GLYPH_ROAD_NS;   // stub facing south
    case 3:  return GLYPH_ROAD_NS;
    case 4:  return GLYPH_ROAD_EW;
    case 5:  return GLYPH_ROAD_NE;
    case 6:  return GLYPH_ROAD_SE;
    case 7:  return GLYPH_ROAD_NSE;
    case 8:  return GLYPH_ROAD_EW;
    case 9:  return GLYPH_ROAD_NW;
    case 10: return GLYPH_ROAD_SW;
    case 11: return GLYPH_ROAD_NSW;
    case 12: return GLYPH_ROAD_EW;
    case 13: return GLYPH_ROAD_NEW;
    case 14: return GLYPH_ROAD_SEW;
    case 15: return GLYPH_ROAD_NSEW;
  }
  return GLYPH_ROAD_EW;
}

private string render_cell(string game, string map_name,
                           int sx, int sy, int sz)
{
  object sect;
  string type;

  sect = cached_sector(game, map_name, sx, sy, sz);
  if (!sect) return GLYPH_EMPTY;

  type = sect->query_sector_type();

  if (type == SECTOR_TYPE_CITY)        return city_glyph(game, map_name, sx, sy, sz);
  if (type == SECTOR_TYPE_ROAD)        return road_glyph(game, map_name, sx, sy, sz);
  if (type == SECTOR_TYPE_FOREST)      return GLYPH_FOREST;
  if (type == SECTOR_TYPE_COAST)       return GLYPH_COAST;
  if (type == SECTOR_TYPE_UNDERGROUND) return GLYPH_UNDERGROUND;

  return GLYPH_EMPTY;
}

// Render a `width x height` sector viewport centred on the given world
// coord. The viewer's sector is overlaid with '@' regardless of what
// terrain occupies it.
string render(int center_x, int center_y, int center_z,
              string game, string map_name, int width, int height)
{
  int sx0, sy0;
  int col0, row_top;
  int row_i, col;
  int cell_sx, cell_sy;
  string result, row;

  sector_cache = ([ ]);

  sx0 = sector_index(center_x);
  sy0 = sector_index(center_y);

  // top-left of the viewport, expressed in sector indices. `row_top`
  // is the highest y in the viewport because on screen y grows downward.
  col0    = sx0 - width / 2;
  row_top = sy0 + height / 2;

  result = "";
  for (row_i = 0; row_i < height; row_i++)
  {
    row = "";
    cell_sy = row_top - row_i;
    for (col = 0; col < width; col++)
    {
      cell_sx = col0 + col;
      if (cell_sx == sx0 && cell_sy == sy0)
        row += GLYPH_PLAYER;
      else
        row += render_cell(game, map_name, cell_sx, cell_sy, sector_index(center_z));
    }
    result += row + "\n";
  }

  return result;
}

// Convenience: pull the world coord + map + game out of the viewer's
// environment and hand off to render(). Returns nil if the viewer is
// standing in something that has no world coord (legacy room, void, a
// container inside a container without location metadata).
string render_around(object viewer, int width, int height)
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

  return render(coords[0], coords[1], coords[2], game, map_name, width, height);
}
