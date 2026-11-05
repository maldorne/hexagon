
#include <mud/cmd.h>
#include <sector/sector.h>
#include <language.h>

inherit CMD_BASE;

// player `docks` cmd: the places of the game where a ship can land, one per
// port, as the world map draws them (see query_dock_state of the world map
// handler). Both the dock and the place are shown by name only.

void setup()
{
  set_aliases(_LANG_CMD_DOCKS_ALIAS);
  set_usage(_LANG_CMD_DOCKS_SYNTAX);
  set_help(_LANG_CMD_DOCKS_HELP);
}

/**
 * Called back by the sectors handler (list_docks) with the sectors holding a
 * dock: tells the player the docks by name and the place they belong to.
 */
void show_docks(object who, string game, string map_name, mixed * docks)
{
  string ** list, text;
  int i;

  if (!who)
    return;

  list = load_object(WORLDMAP_HANDLER)->dock_list(docks);
  if (!sizeof(list))
  {
    tell_object(who, _LANG_DOCKS_NONE);
    return;
  }

  text = "%^BOLD%^" + _LANG_DOCKS_TITLE + "%^RESET%^\n";
  for (i = 0; i < sizeof(list); i++)
    text += strlen(list[i][1]) ?
            sprintf(_LANG_DOCKS_LINE, list[i][0], list[i][1]) :
            sprintf(_LANG_DOCKS_LINE_NO_PLACE, list[i][0]);

  tell_object(who, text);
}

static int cmd(string str, object me, string verb)
{
  string game, map_name;
  object env;

  game = game_name(me);
  if (!strlen(game))
  {
    notify_fail(_LANG_DOCKS_NO_GAME);
    return 0;
  }

  env = environment(me);
  map_name = (env && env->query_location()) ? env->query_map_name() : "default";

  write(_LANG_DOCKS_SEARCHING);
  load_object(SECTORS_HANDLER)->list_docks(game, map_name, me, this_object(),
                                           "show_docks");
  return 1;
}
