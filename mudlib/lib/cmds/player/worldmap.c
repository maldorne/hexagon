
#include <mud/cmd.h>
#include <language.h>

inherit CMD_BASE;

void setup()
{
  set_aliases(_LANG_CMD_WORLDMAP_ALIAS);
  set_usage(_LANG_CMD_WORLDMAP_SYNTAX);
  set_help(_LANG_CMD_WORLDMAP_HELP);
}

// Players see only the base help; resizing is a coder tool, so its syntax is
// appended only for coders (same pattern as map.c).
string query_help(varargs string str)
{
  string out;

  out = _LANG_CMD_WORLDMAP_HELP;
  if (this_player() && this_player()->query_coder())
    out += "\n\n" + _LANG_CMD_WORLDMAP_HELP_CODER;

  return out;
}

private string * legend_labels()
{
  return ({ _LANG_CMD_WORLDMAP_YOUR_POS, _LANG_CMD_WORLDMAP_CITY,
            _LANG_CMD_WORLDMAP_FOREST, _LANG_CMD_WORLDMAP_FOREST_CITY,
            _LANG_CMD_WORLDMAP_FARM,
            _LANG_CMD_WORLDMAP_PLAINS, _LANG_CMD_WORLDMAP_SWAMP,
            _LANG_CMD_WORLDMAP_MOUNTAIN, _LANG_CMD_WORLDMAP_TAIGA,
            _LANG_CMD_WORLDMAP_RUINS,
            _LANG_CMD_WORLDMAP_RIVER,
            _LANG_CMD_WORLDMAP_COAST, _LANG_CMD_WORLDMAP_BEACH,
            _LANG_CMD_WORLDMAP_UNDERGROUND });
}

// How wide the widest legend line can be ("  x : label").
private int legend_width()
{
  string * labels;
  int i, best;

  labels = legend_labels();
  best = strlen(_LANG_CMD_WORLDMAP_LEGEND + ":");
  for (i = 0; i < sizeof(labels); i++)
    if (strlen(fix_string(labels[i]), 1) + 6 > best)
      best = strlen(fix_string(labels[i]), 1) + 6;
  return best;
}

/**
 * Lays the legend entries out in two columns, filling the first top to
 * bottom and then the second. Each column is padded to the widest entry of
 * the first one; with one entry, or when two columns do not fit in `avail`
 * visible columns, it falls back to one column.
 *
 * @param entries the legend lines, colour codes allowed
 * @param avail the visible width the frame content has
 * @return the lines joined, each ending in a line break
 */
private string legend_columns(string * entries, int avail)
{
  string out;
  int i, rows, pad, widest, size;

  size = sizeof(entries);
  widest = 0;
  rows = (size + 1) / 2;
  for (i = 0; i < rows; i++)
    if (strlen(fix_string(entries[i]), 1) > widest)
      widest = strlen(fix_string(entries[i]), 1);

  // one column when the second would not fit
  if (size < 2 || widest + 2 + widest > avail)
    rows = size;

  out = "";
  for (i = 0; i < rows; i++)
  {
    out += entries[i];
    if (i + rows < size)
    {
      for (pad = widest - strlen(fix_string(entries[i]), 1) + 2; pad > 0; pad--)
        out += " ";
      out += entries[i + rows];
    }
    out += "\n";
  }

  return out;
}

// The legend under the map: your position always, and each other kind of
// sector only when the map shows one. `avail` is the width of the frame.
private string legend(string map, int avail)
{
  mapping glyphs;
  string * kinds, * labels, * entries;
  string out;
  int i;

  glyphs = handler("worldmap")->query_legend_glyphs();
  kinds = ({ "player", "city", "forest", "forest-city", "farm", "plains", "swamp", "mountain", "taiga", "ruins", "river", "coast", "beach", "underground" });
  labels = legend_labels();

  entries = ({ });
  for (i = 0; i < sizeof(kinds); i++)
  {
    mixed seen;
    int shown, j;

    // a kind the renderer does not draw (no entry) has no legend line; an
    // entry's search glyph may be a list of alternatives
    if (!glyphs[kinds[i]])
      continue;

    seen = glyphs[kinds[i]][0];
    if (!arrayp(seen))
      seen = ({ seen });

    shown = (kinds[i] == "player");
    for (j = 0; j < sizeof(seen) && !shown; j++)
      shown = sizeof(explode("#" + map + "#", seen[j])) > 1;

    if (shown)
      entries += ({ "  " + glyphs[kinds[i]][1] + " : " + labels[i] });
  }

  out = "\n" + _LANG_CMD_WORLDMAP_LEGEND + ":\n";
  return out + legend_columns(entries, avail);
}

static int cmd(string str, object me, string verb)
{
  int width, height, cols;
  string map;

  // players always get the default viewport, as wide as their terminal lets
  // the framed map be; resizing it is a coder tool
  cols = me->user() ? (int)me->user()->query_cols() : 0;
  width = (cols > 0 ? cols : 80) -
          handler("frames")->query_frame_overhead("scroll");
  if (width > 80)
    width = 80;
  if (width < 20)
    width = 20;
  height = 20;

  if (str && strlen(str))
  {
    if (!me->query_coder())
    {
      notify_fail(_LANG_CMD_WORLDMAP_CODER);
      return 0;
    }

    // "worldmap N M" -> width x height; "worldmap N" -> N x N
    if (sscanf(str, "%d %d", width, height) == 2)
      ;
    else if (sscanf(str, "%d", width) == 1)
      height = width;
    else
    {
      notify_fail(_LANG_CMD_WORLDMAP_USAGE);
      return 0;
    }

    if (width < 3 || width > 80 || height < 3 || height > 40)
    {
      notify_fail(_LANG_CMD_WORLDMAP_RANGE);
      return 0;
    }
  }

  // the frame is as wide as its longest line, so the widest legend line leaves
  // blank columns to the right of a narrower map: room for city names
  map = handler("worldmap")->render_around(me, width, height, 1, 1,
                                           legend_width() - width);
  if (!map || !strlen(map))
  {
    notify_fail(_LANG_CMD_WORLDMAP_NOMAP);
    return 0;
  }

  // wrap the grid in the parchment frame, matching the location-level
  // map command's presentation
  write("\n" + handler("frames")->frame(map + legend(map, (width > legend_width() ? width : legend_width())), "", 0, 0, "scroll") + "\n");
  return 1;
}
