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

// road glyphs (double-line box drawing) — indexed by an n/s/e/w mask
// (n=1, s=2, e=4, w=8)
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

// path glyphs (single-line box drawing) — same mask indexing. A sector
// whose border crossings are all paths draws in single line; any road
// crossing promotes it to the double-line set above.
#define GLYPH_PATH_NS     "│"
#define GLYPH_PATH_EW     "─"
#define GLYPH_PATH_NE     "└"
#define GLYPH_PATH_NW     "┘"
#define GLYPH_PATH_SE     "┌"
#define GLYPH_PATH_SW     "┐"
#define GLYPH_PATH_NSE    "├"
#define GLYPH_PATH_NSW    "┤"
#define GLYPH_PATH_NEW    "┴"
#define GLYPH_PATH_SEW    "┬"
#define GLYPH_PATH_NSEW   "┼"

// Toggle for the decorative city-wall overlay (see _overlay_city_walls).
// Walls are a post-processing pass drawn AROUND contiguous city regions
// to highlight where a settlement is; they do not correspond one-to-one
// with the underlying sectors. Set to 0 to render cities as plain solid
// blocks with no surrounding wall.
#define WORLDMAP_CITY_WALLS 1

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

// Glyph for a road/path sector, driven by its own border crossings
// (sector->query_border_ways, the S2 data) rather than by guessing from
// neighbour sector types. The mask (n=1, s=2, e=4, w=8) says which
// borders carry a way; `heavy` picks the double-line road set when any
// crossing is a road, else the single-line path set.
private string way_glyph(object sect)
{
  mapping borders;
  int mask, heavy;
  string * types;

  borders = sect->query_border_ways();
  if (!mappingp(borders)) borders = ([ ]);

  mask  = arrayp(borders[SECTOR_BORDER_N]) && sizeof(borders[SECTOR_BORDER_N]) ? 1 : 0;
  mask |= arrayp(borders[SECTOR_BORDER_S]) && sizeof(borders[SECTOR_BORDER_S]) ? 2 : 0;
  mask |= arrayp(borders[SECTOR_BORDER_E]) && sizeof(borders[SECTOR_BORDER_E]) ? 4 : 0;
  mask |= arrayp(borders[SECTOR_BORDER_W]) && sizeof(borders[SECTOR_BORDER_W]) ? 8 : 0;

  // any road on any border promotes the whole cell to the road set
  heavy = 0;
  types = ({ SECTOR_BORDER_N, SECTOR_BORDER_S, SECTOR_BORDER_E, SECTOR_BORDER_W });
  {
    int i;
    for (i = 0; i < sizeof(types); i++)
      if (arrayp(borders[types[i]]) &&
          member_array(SECTOR_WAY_ROAD, borders[types[i]]) != -1)
        heavy = 1;
  }

  if (heavy)
  {
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

  switch (mask)
  {
    case 0:  return GLYPH_PATH_EW;
    case 1:  return GLYPH_PATH_NS;
    case 2:  return GLYPH_PATH_NS;
    case 3:  return GLYPH_PATH_NS;
    case 4:  return GLYPH_PATH_EW;
    case 5:  return GLYPH_PATH_NE;
    case 6:  return GLYPH_PATH_SE;
    case 7:  return GLYPH_PATH_NSE;
    case 8:  return GLYPH_PATH_EW;
    case 9:  return GLYPH_PATH_NW;
    case 10: return GLYPH_PATH_SW;
    case 11: return GLYPH_PATH_NSW;
    case 12: return GLYPH_PATH_EW;
    case 13: return GLYPH_PATH_NEW;
    case 14: return GLYPH_PATH_SEW;
    case 15: return GLYPH_PATH_NSEW;
  }
  return GLYPH_PATH_EW;
}

private string render_cell(string game, string map_name,
                           int sx, int sy, int sz)
{
  object sect;
  string type;

  sect = cached_sector(game, map_name, sx, sy, sz);
  if (!sect) return GLYPH_EMPTY;

  type = sect->query_sector_type();

  // city cells are a plain solid block; the surrounding wall is drawn
  // later by the _overlay_city_walls post-processing pass.
  if (type == SECTOR_TYPE_CITY)        return GLYPH_CITY_SOLID;
  if (type == SECTOR_TYPE_ROAD)        return way_glyph(sect);
  if (type == SECTOR_TYPE_FOREST)      return GLYPH_FOREST;
  if (type == SECTOR_TYPE_COAST)       return GLYPH_COAST;
  if (type == SECTOR_TYPE_UNDERGROUND) return GLYPH_UNDERGROUND;

  return GLYPH_EMPTY;
}

// Write a glyph into the grid, clamped to bounds and never clobbering
// the player marker.
private void _put(string ** grid, int w, int h, int r, int c, string g)
{
  if (r < 0 || r >= h || c < 0 || c >= w)
    return;
  if (grid[r][c] == GLYPH_PLAYER)
    return;
  grid[r][c] = g;
}

// Draw the border of the screen-space rectangle [top,left]..[bottom,right]
// (top is the visually higher row). Corners + straight single-line edges.
private void _draw_rect(string ** grid, int w, int h,
                        int top, int left, int bottom, int right)
{
  int r, c;

  _put(grid, w, h, top,    left,  GLYPH_CITY_NW);   // ┌
  _put(grid, w, h, top,    right, GLYPH_CITY_NE);   // ┐
  _put(grid, w, h, bottom, left,  GLYPH_CITY_SW);   // └
  _put(grid, w, h, bottom, right, GLYPH_CITY_SE);   // ┘

  for (c = left + 1; c < right; c++)
  {
    _put(grid, w, h, top,    c, GLYPH_CITY_N);      // ─
    _put(grid, w, h, bottom, c, GLYPH_CITY_S);      // ─
  }
  for (r = top + 1; r < bottom; r++)
  {
    _put(grid, w, h, r, left,  GLYPH_CITY_W);       // │
    _put(grid, w, h, r, right, GLYPH_CITY_E);       // │
  }
}

// Post-processing pass: highlight every contiguous region of city
// sectors by drawing a rectangular wall one cell outside its bounding
// box. Deliberately decorative — the rectangle marks "there is a city
// here", it does not trace the region's exact silhouette, so a single
// city sector or an irregular blob both get a clean surrounding wall.
private void _overlay_city_walls(string ** grid, int ** is_city,
                                 int w, int h)
{
  int ** visited;
  int r, c;

  visited = allocate(h);
  for (r = 0; r < h; r++)
    visited[r] = allocate_int(w);

  for (r = 0; r < h; r++)
    for (c = 0; c < w; c++)
    {
      int minr, maxr, minc, maxc;
      int * sr, * sc;

      if (!is_city[r][c] || visited[r][c])
        continue;

      // flood-fill the blob (4-connectivity), tracking its bounding box
      minr = maxr = r;
      minc = maxc = c;
      sr = ({ r });
      sc = ({ c });
      visited[r][c] = 1;

      while (sizeof(sr))
      {
        int cr, cc, k;
        int * dr, * dc;

        cr = sr[sizeof(sr) - 1];
        cc = sc[sizeof(sc) - 1];
        sr = sr[0 .. sizeof(sr) - 2];
        sc = sc[0 .. sizeof(sc) - 2];

        if (cr < minr) minr = cr;
        if (cr > maxr) maxr = cr;
        if (cc < minc) minc = cc;
        if (cc > maxc) maxc = cc;

        dr = ({ -1, 1, 0, 0 });
        dc = ({ 0, 0, -1, 1 });
        for (k = 0; k < 4; k++)
        {
          int nr, nc;
          nr = cr + dr[k];
          nc = cc + dc[k];
          if (nr >= 0 && nr < h && nc >= 0 && nc < w &&
              is_city[nr][nc] && !visited[nr][nc])
          {
            visited[nr][nc] = 1;
            sr += ({ nr });
            sc += ({ nc });
          }
        }
      }

      _draw_rect(grid, w, h, minr - 1, minc - 1, maxr + 1, maxc + 1);
    }
}

// Render a `width x height` sector viewport centred on the given world
// coord. The viewer's sector is overlaid with '@' regardless of what
// terrain occupies it.
string render(int center_x, int center_y, int center_z,
              string game, string map_name, int width, int height)
{
  int sx0, sy0, sz0;
  int col0, row_top;
  int row_i, col;
  string ** grid;
  int ** is_city;
  string result;

  sector_cache = ([ ]);

  sx0 = sector_index(center_x);
  sy0 = sector_index(center_y);
  sz0 = sector_index(center_z);

  // top-left of the viewport, expressed in sector indices. `row_top`
  // is the highest y in the viewport because on screen y grows downward.
  col0    = sx0 - width / 2;
  row_top = sy0 + height / 2;

  // base fill: one glyph per sector, plus a parallel city mask the
  // wall overlay reads (so it works off sector type, not the glyph,
  // which the player marker would otherwise hide).
  grid = allocate(height);
  is_city = allocate(height);
  for (row_i = 0; row_i < height; row_i++)
  {
    int cell_sy;
    grid[row_i] = allocate(width);
    is_city[row_i] = allocate_int(width);
    cell_sy = row_top - row_i;
    for (col = 0; col < width; col++)
    {
      int cell_sx;
      object sect;
      cell_sx = col0 + col;
      sect = cached_sector(game, map_name, cell_sx, cell_sy, sz0);
      is_city[row_i][col] =
        (sect && sect->query_sector_type() == SECTOR_TYPE_CITY);
      grid[row_i][col] = render_cell(game, map_name, cell_sx, cell_sy, sz0);
    }
  }

  // player marker (before the overlay, which never overwrites it)
  {
    int prow, pcol;
    prow = row_top - sy0;
    pcol = sx0 - col0;
    if (prow >= 0 && prow < height && pcol >= 0 && pcol < width)
      grid[prow][pcol] = GLYPH_PLAYER;
  }

  if (WORLDMAP_CITY_WALLS)
    _overlay_city_walls(grid, is_city, width, height);

  result = "";
  for (row_i = 0; row_i < height; row_i++)
    result += implode(grid[row_i], "") + "\n";

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
