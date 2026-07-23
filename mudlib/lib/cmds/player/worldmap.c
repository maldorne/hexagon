
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

static int cmd(string str, object me, string verb)
{
  int width, height;
  string map;

  width = 10;
  height = 10;

  if (str && strlen(str))
  {
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
  }

  if (width < 3 || width > 80 || height < 3 || height > 40)
  {
    notify_fail(_LANG_CMD_WORLDMAP_RANGE);
    return 0;
  }

  map = handler(WORLDMAP_HANDLER)->render_around(me, width, height);
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
