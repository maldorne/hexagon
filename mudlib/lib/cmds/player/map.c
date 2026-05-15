
// player `map` cmd: thin ASCII renderer over CARTOGRAPHY_HANDLER.
//
// The structure of the map (cell types, viewer position, dimensions)
// is computed by /lib/handlers/cartography.c. This file is presentation
// only — porting to a different UI (web, JSON, alternate ASCII style)
// just means writing a new renderer that consumes the same mapping.

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

private string _draw_map(mapping view)
{
  int width, height;
  int ** cells;
  int vx, vy;
  string result, line;
  int quests, fquests, doors, ups, downs, coasts, adventurers, guards, enemies;
  int i, j, type;

  width  = view["width"];
  height = view["height"];
  cells  = view["cells"];
  vx     = view["viewer_x"];
  vy     = view["viewer_y"];

  quests = fquests = doors = ups = downs = coasts = 0;
  adventurers = guards = enemies = 0;

  result  = "\n";
  result += "    ______________________________________________________________\n";
  result += "   /\\                                                             \\\n";
  result += "   \\_|                                                             |\n";

  for (i = 0; i < height; i++)
  {
    line = "";
    for (j = 0; j < width; j++)
    {
      type = cells[i][j];

      // viewer's own cell — overlay the asterisk on top of the underlying type
      if (i == vy && j == vx)
      {
        if (type == CART_COAST_ROOM)
        {
          line += "%^BLUE%^[%^ORANGE%^*%^BLUE%^]%^RESET%^";
          coasts = 1;
        }
        else
          line += "[%^ORANGE%^*%^RESET%^]";
        continue;
      }

      switch (type)
      {
        case CART_ROOM:
          line += "[ ]"; break;
        case CART_COAST_ROOM:
          line += "%^BLUE%^[ ]%^RESET%^"; coasts = 1; break;
        case CART_DOOR_ROOM:
          line += "[" + _LANG_CMD_MAP_DOOR_LETTER + "]"; doors = 1; break;
        case CART_UP_ROOM:
          line += "[^]"; ups = 1; break;
        case CART_DOWN_ROOM:
          line += "[v]"; downs = 1; break;
        case CART_FINISH_QUEST_ROOM:
          line += "[%^BOLD%^YELLOW%^?%^RESET%^]"; fquests = 1; break;
        case CART_QUEST_ROOM:
          line += "[%^BOLD%^YELLOW%^!%^RESET%^]"; quests = 1; break;
        case CART_ADVENTURER_ROOM:
          line += "[%^BOLD%^CYAN%^*%^RESET%^]"; adventurers = 1; break;
        case CART_GUARD_ROOM:
          line += "[%^BOLD%^GREEN%^*%^RESET%^]"; guards = 1; break;
        case CART_ENEMY_ROOM:
          line += "[%^BOLD%^RED%^*%^RESET%^]"; enemies = 1; break;
        case CART_HORIZONTAL_EXIT:
          line += "---"; break;
        case CART_VERTICAL_EXIT:
          line += " | "; break;
        case CART_SLASH_EXIT:
          line += " / "; break;
        case CART_BACKSLASH_EXIT:
          line += " \\ "; break;
        default:
          line += "   "; break;
      }
    }

    result += "     |        " + line + "        |\n";
  }

  result += "     |                                                             |\n";
  result += "     |                            " + _format(_LANG_CMD_MAP_LEGEND + " :") + "         |\n";
  result += "     |                               %^ORANGE%^*%^RESET%^   : " + _format(_LANG_CMD_MAP_YOUR_POS) + "|\n";

  if (enemies)
    result += "     |                               %^BOLD%^RED%^*%^RESET%^   : " + _format(_LANG_CMD_MAP_ENEMIES) + "|\n";
  if (adventurers)
    result += "     |                               %^BOLD%^CYAN%^*%^RESET%^   : " + _format(_LANG_CMD_MAP_FRIENDS) + "|\n";
  if (guards)
    result += "     |                               %^BOLD%^GREEN%^*%^RESET%^   : " + _format(_LANG_CMD_MAP_GUARDS) + "|\n";
  if (doors)
    result += "     |                               " + _LANG_CMD_MAP_DOOR_LETTER + "   : " + _format(_LANG_CMD_MAP_DOORS) + "|\n";
  if (ups)
    result += "     |                               ^   : " + _format(_LANG_CMD_MAP_UP_STAIRS) + "|\n";
  if (downs)
    result += "     |                               v   : " + _format(_LANG_CMD_MAP_DOWN_STAIRS) + "|\n";
  if (quests)
    result += "     |                               %^BOLD%^YELLOW%^!%^RESET%^   : " + _format(_LANG_CMD_MAP_NEW_QUESTS) + "|\n";
  if (fquests)
    result += "     |                               %^BOLD%^YELLOW%^?%^RESET%^   : " + _format(_LANG_CMD_MAP_FINISHED_QUESTS) + "|\n";
  if (coasts)
    result += "     |                               %^BLUE%^[ ]%^RESET%^ : " + _format(_LANG_CMD_MAP_COAST) + "|\n";

  result += "     |                                                             |\n";
  result += "     |   __________________________________________________________|___\n";
  result += "      \\_/_____________________________________________________________/\n\n";

  return result;
}

static int cmd(string str, object me, string verb)
{
  mapping view;

  // Players always get the full picture (eager BFS, full inventory scan
  // for quest / enemy / guard / etc. markers). If the load cascade ever
  // becomes a problem on a particular area we will revisit; for now the
  // user-facing verb is intentionally a single-shape command without
  // tuning knobs. Code paths that need the cheaper lazy scan call the
  // handler directly with `([ "deep": 0 ])`.
  view = load_object(CARTOGRAPHY_HANDLER)->query_map_view(me, ([ "deep": 1 ]));

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

  write(_draw_map(view));
  return 1;
}
