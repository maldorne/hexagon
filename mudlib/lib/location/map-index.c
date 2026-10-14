// The world map index of one map of one game: what every sector of it shows,
// so a map is drawn without loading any sector. Kept by the sectors handler
// (query_map_index() of /lib/handlers/sectors.c), one per game and map, saved
// beside the map's sectors in glyphs.o. Each sector writes its own cell from
// refresh_glyph() of /lib/location/sector.c whenever it changes.

#include <maps/glyphs.h>

inherit "/lib/core/object.c";

// ([ sz : ([ sy : ([ sx : ({ glyph code, city directory }) ]) ]) ]): the code
// of the glyph a sector shows (MAP_GLYPH_* in include/maps/glyphs.h) and the
// directory holding most of its city locations ("" if none), by sector index.
// A sector that shows nothing has no cell.
mapping cells;
string file_name;
// a save is already scheduled
static int save_pending;

void create()
{
  cells = ([ ]);
  ::create();
}

int restore_from_file_name(string name)
{
  file_name = name;
  if (file_size(name) >= 0)
    restore_object(name);
  if (!cells)
    cells = ([ ]);
  return 1;
}

void save_me()
{
  save_pending = 0;
  save_object(file_name);
}

// The cells of one level of the map, ([ sy : ([ sx : ({ code, dir }) ]) ]).
// The mapping is the index's own: read it, never change it.
mapping query_level(int sz)
{
  return cells[sz] ? cells[sz] : ([ ]);
}

// What the sector at (sx, sy, sz) shows. Saved once at the end of the current
// execution, so many sectors changing together write the file once.
void set_cell(int sx, int sy, int sz, int code, string dir)
{
  if (code == MAP_GLYPH_EMPTY && !strlen(dir))
  {
    if (cells[sz] && cells[sz][sy])
    {
      map_delete(cells[sz][sy], sx);
      if (!map_sizeof(cells[sz][sy]))
        map_delete(cells[sz], sy);
      if (!map_sizeof(cells[sz]))
        map_delete(cells, sz);
    }
  }
  else
  {
    if (!cells[sz])
      cells[sz] = ([ ]);
    if (!cells[sz][sy])
      cells[sz][sy] = ([ ]);
    cells[sz][sy][sx] = ({ code, dir ? dir : "" });
  }

  if (!save_pending)
  {
    save_pending = 1;
    call_out("save_me", 0);
  }
}
