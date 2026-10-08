
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

// The legend under the map: your position always, and each other kind of
// sector only when the map shows one.
private string legend(string map)
{
  mapping glyphs;
  string * kinds, * labels;
  string out;
  int i;

  glyphs = handler("worldmap")->query_legend_glyphs();
  kinds = ({ "player", "city", "forest", "farm", "coast", "underground" });
  labels = ({ _LANG_CMD_WORLDMAP_YOUR_POS, _LANG_CMD_WORLDMAP_CITY,
              _LANG_CMD_WORLDMAP_FOREST, _LANG_CMD_WORLDMAP_FARM,
              _LANG_CMD_WORLDMAP_COAST, _LANG_CMD_WORLDMAP_UNDERGROUND });

  out = "\n" + _LANG_CMD_WORLDMAP_LEGEND + ":\n";
  for (i = 0; i < sizeof(kinds); i++)
    if (kinds[i] == "player" || sizeof(explode("#" + map + "#", glyphs[kinds[i]][0])) > 1)
      out += "  " + glyphs[kinds[i]][1] + " : " + labels[i] + "\n";

  return out;
}

static int cmd(string str, object me, string verb)
{
  int width, height;
  string map;

  // players always get the default viewport; resizing it is a coder tool
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

  map = handler("worldmap")->render_around(me, width, height, 1, 1);
  if (!map || !strlen(map))
  {
    notify_fail(_LANG_CMD_WORLDMAP_NOMAP);
    return 0;
  }

  // wrap the grid in the parchment frame, matching the location-level
  // map command's presentation
  write("\n" + handler("frames")->frame(map + legend(map), "", 0, 0, "scroll") + "\n");
  return 1;
}
