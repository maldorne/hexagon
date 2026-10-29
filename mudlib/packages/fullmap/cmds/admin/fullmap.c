// fullmap -- admin tool that draws the whole world map of a game level. The
// work is done by the fullmap package (/packages/fullmap/).
//
//   fullmap [<game>] [<z>] [html]

#include <mud/cmd.h>
#include <language.h>
#include <sector/sector.h>

inherit CMD_BASE;

void setup()
{
  set_aliases(_LANG_CMD_FULLMAP_ALIAS);
  set_usage(_LANG_CMD_FULLMAP_SYNTAX);
  set_help(_LANG_CMD_FULLMAP_HELP);
}

static int cmd(string str, object me, string verb)
{
  string * words;
  string game, map_name, text, file;
  object fullmap, env;
  int z, html, i, has_z;
  int * b;

  if (this_player(1) != this_user())
    return 0;

  fullmap = package("fullmap");
  if (!fullmap)
  {
    tell_object(me, _LANG_CMD_FULLMAP_NOPACKAGE);
    return 1;
  }

  env = environment(me);
  game = env ? game_name(env) : nil;
  map_name = env ? env->query_map_name() : nil;
  if (!map_name || !strlen(map_name))
    map_name = "default";

  // z defaults to the sector level of the admin's location
  if (env && env->query_coordinates() && sizeof(env->query_coordinates()) >= 3)
    z = SECTOR_INDEX(env->query_coordinates()[2]);

  words = str ? explode(trim(str), " ") - ({ "" }) : ({ });
  for (i = 0; i < sizeof(words); i++)
  {
    if (words[i] == "html")
      html = 1;
    else if (!has_z && sscanf(words[i], "%d", z) == 1 && "" + z == words[i])
      has_z = 1;
    else if (i == 0 || (i == 1 && !has_z && !html))
      game = words[i];
    else
    {
      tell_object(me, _LANG_CMD_FULLMAP_USAGE);
      return 1;
    }
  }

  if (!game || !strlen(game))
  {
    tell_object(me, _LANG_CMD_FULLMAP_USAGE);
    return 1;
  }

  text = fullmap->render_level(game, map_name, z);
  if (!text)
  {
    tell_object(me, _LANG_CMD_FULLMAP_NOMAP);
    return 1;
  }

  tell_object(me, text);
  b = fullmap->query_bounds(game, map_name, z);
  tell_object(me, sprintf(_LANG_CMD_FULLMAP_SIZE, b[1] - b[0] + 1, b[3] - b[2] + 1));

  if (html)
  {
    file = fullmap->write_html(game, map_name, z);
    tell_object(me, sprintf(_LANG_CMD_FULLMAP_WRITTEN, file));
  }

  return 1;
}
