
#include <mud/cmd.h>
#include <maps/maps.h>
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

  map = handler(WORLDMAP_HANDLER)->render_around(me, width, height, 1);
  if (!map || !strlen(map))
  {
    notify_fail(_LANG_CMD_WORLDMAP_NOMAP);
    return 0;
  }

  // wrap the grid in the parchment frame, matching the location-level
  // map command's presentation
  write("\n" + handler("frames")->frame(map, "", 0, 0, "scroll") + "\n");
  return 1;
}
