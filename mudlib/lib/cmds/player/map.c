
// player `map` cmd: ASCII renderer + parchment frame + legend over
// CARTOGRAPHY_HANDLER. The grid contents come from the handler's
// `render_ascii(view)`; this file only adds the surrounding frame and
// the language-dependent legend computed from which marker types
// actually appear in the cells.

#include <mud/cmd.h>
#include <translations/exits.h>
#include <cartography.h>
#include <language.h>

inherit CMD_BASE;

void setup()
{
  set_aliases(_LANG_CMD_MAP_ALIAS);
  set_usage(_LANG_CMD_MAP_SYNTAX);
  set_help(_LANG_CMD_MAP_HELP);
}

private string _format(string str) { return sprintf("%-*s", 24, str); }

// Walk the cells grid and return a mapping of which marker types
// appear so the legend can show only the relevant rows.
private mapping _scan_legend(mapping view)
{
  int ** cells;
  int width, height;
  mapping flags;
  int i, j;

  cells  = view["cells"];
  width  = view["width"];
  height = view["height"];

  flags = ([ ]);

  for (i = 0; i < height; i++)
    for (j = 0; j < width; j++)
      flags[cells[i][j]] = 1;

  return flags;
}

// Take the bare grid string from the handler and wrap it in the
// parchment border together with the legend.
private string _wrap_in_frame(string grid, mapping flags)
{
  string out;
  string * lines;
  int i;

  out  = "\n";
  out += "    ______________________________________________________________\n";
  out += "   /\\                                                             \\\n";
  out += "   \\_|                                                             |\n";

  lines = explode(grid, "\n");
  for (i = 0; i < sizeof(lines); i++)
  {
    if (i == sizeof(lines) - 1 && lines[i] == "")
      continue;
    out += "     |        " + lines[i] + "        |\n";
  }

  out += "     |                                                             |\n";
  out += "     |                            " + _format(_LANG_CMD_MAP_LEGEND + " :") + "         |\n";
  out += "     |                               %^ORANGE%^*%^RESET%^   : " + _format(_LANG_CMD_MAP_YOUR_POS) + "|\n";

  if (flags[CART_ENEMY_ROOM])
    out += "     |                               %^BOLD%^RED%^*%^RESET%^   : " + _format(_LANG_CMD_MAP_ENEMIES) + "|\n";
  if (flags[CART_ADVENTURER_ROOM])
    out += "     |                               %^BOLD%^CYAN%^*%^RESET%^   : " + _format(_LANG_CMD_MAP_FRIENDS) + "|\n";
  if (flags[CART_GUARD_ROOM])
    out += "     |                               %^BOLD%^GREEN%^*%^RESET%^   : " + _format(_LANG_CMD_MAP_GUARDS) + "|\n";
  if (flags[CART_DOOR_ROOM])
    out += "     |                               " + _LANG_CMD_MAP_DOOR_LETTER + "   : " + _format(_LANG_CMD_MAP_DOORS) + "|\n";
  if (flags[CART_UP_ROOM])
    out += "     |                               ^   : " + _format(_LANG_CMD_MAP_UP_STAIRS) + "|\n";
  if (flags[CART_DOWN_ROOM])
    out += "     |                               v   : " + _format(_LANG_CMD_MAP_DOWN_STAIRS) + "|\n";
  if (flags[CART_QUEST_ROOM])
    out += "     |                               %^BOLD%^YELLOW%^!%^RESET%^   : " + _format(_LANG_CMD_MAP_NEW_QUESTS) + "|\n";
  if (flags[CART_FINISH_QUEST_ROOM])
    out += "     |                               %^BOLD%^YELLOW%^?%^RESET%^   : " + _format(_LANG_CMD_MAP_FINISHED_QUESTS) + "|\n";
  if (flags[CART_COAST_ROOM])
    out += "     |                               %^BLUE%^[ ]%^RESET%^ : " + _format(_LANG_CMD_MAP_COAST) + "|\n";

  out += "     |                                                             |\n";
  out += "     |   __________________________________________________________|___\n";
  out += "      \\_/_____________________________________________________________/\n\n";

  return out;
}

static int cmd(string str, object me, string verb)
{
  mapping view;
  object cart;
  string grid;

  // Players always get the full picture (eager BFS, full inventory scan
  // for quest / enemy / guard / etc. markers). If the load cascade ever
  // becomes a problem on a particular area we will revisit; for now the
  // user-facing verb is intentionally a single-shape command without
  // tuning knobs. Code paths that need the cheaper lazy scan call the
  // handler directly with `([ "deep": 0 ])`.
  cart = load_object(CARTOGRAPHY_HANDLER);
  view = cart->query_map_view(me, ([ "deep": 1 ]));

  if (!view)
  {
    if (!environment(me))
      notify_fail(_LANG_CMD_MAP_NO_ENV);
    else if (environment(me)->query_dungeon_room())
      notify_fail(_LANG_CMD_MAP_INVALID);
    else if (environment(me)->query_water_environment())
      notify_fail(_LANG_CMD_MAP_NO_WATER);
    else
      notify_fail(_LANG_CMD_MAP_INVALID);
    return 0;
  }

  grid = cart->render_ascii(view);
  write(_wrap_in_frame(grid, _scan_legend(view)));
  return 1;
}
