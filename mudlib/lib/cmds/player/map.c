
// player `map` cmd: thin facade over CARTOGRAPHY_HANDLER.
//
// `map`            — default chunky ASCII (`[ ]`, `---`, `|`, `/`, `\`)
//                    inside the parchment frame, with legend.
// `map compact`    — single-character-per-cell roguelike style. Three
//                    times denser, fits a much larger viewport.
// `map coords`     — coder-only debug view: `(x,y)` of each location
//                    cell. Useful for verifying coordinate inference.
//
// The grid contents come from the handler's `render_*` functions; this
// file only adds the parchment frame and legend (default style only)
// and gates the coder-only style.

#include <mud/cmd.h>
#include <translations/exits.h>
#include <cartography.h>
#include <language.h>

inherit CMD_BASE;

#define MAP_STYLE_DEFAULT  0
#define MAP_STYLE_COMPACT  1
#define MAP_STYLE_UNICODE  2
#define MAP_STYLE_COLOR    3
#define MAP_STYLE_COORDS   4

void setup()
{
  set_aliases(_LANG_CMD_MAP_ALIAS);
  set_usage(_LANG_CMD_MAP_SYNTAX);
  set_help(_LANG_CMD_MAP_HELP);
}

// The help text shown to the player. Coder-only options are hidden
// from regular players so they do not clutter the menu.
string query_help(varargs string str)
{
  string out;

  out = _LANG_CMD_MAP_HELP + " " +
        "Plain `map` shows the standard chunky view. " +
        "`map compact` packs the same map into one character per cell. " +
        "`map unicode` keeps that density but uses proper box-drawing " +
        "glyphs (needs a UTF-8 capable client). " +
        "`map color` is the standard view with each room tinted by the " +
        "area it belongs to.";

  if (this_player() && this_player()->query_coder())
    out += " `map coords` overlays the world (x,y) coordinates of every " +
           "location cell (coder-only).";

  return out;
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

// Wrap the default-style grid in the parchment border + legend.
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
  int style;

  // pick a style from the trailing argument
  style = MAP_STYLE_DEFAULT;
  if (str)
  {
    if (str == "compact")
      style = MAP_STYLE_COMPACT;
    else if (str == "unicode")
      style = MAP_STYLE_UNICODE;
    else if (str == "color" || str == "colour")
      style = MAP_STYLE_COLOR;
    else if (str == "coords")
    {
      if (!me->query_coder())
      {
        notify_fail("Unknown map variant.\n");
        return 0;
      }
      style = MAP_STYLE_COORDS;
    }
    else if (strlen(str))
    {
      notify_fail("Unknown map variant. Try `map`, `map compact`, " +
                  "`map unicode`, `map color`" +
                  (me->query_coder() ? ", or `map coords`" : "") + ".\n");
      return 0;
    }
  }

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

  switch (style)
  {
    case MAP_STYLE_COMPACT:
      // compact stays bare — the parchment frame wouldn't fit a much
      // denser grid usefully and the look is meant to be terse anyway
      write("\n" + cart->render_compact(view) + "\n");
      break;
    case MAP_STYLE_UNICODE:
      write("\n" + cart->render_unicode(view) + "\n");
      break;
    case MAP_STYLE_COLOR:
      // same width as the default style → still fits the parchment
      write(_wrap_in_frame(cart->render_color_by_area(view),
                           _scan_legend(view)));
      break;
    case MAP_STYLE_COORDS:
      // coord cells are 8 chars wide — wraps past the parchment width
      write("\n" + cart->render_coords(view) + "\n");
      break;
    default:
      write(_wrap_in_frame(cart->render_ascii(view),
                           _scan_legend(view)));
      break;
  }

  return 1;
}
