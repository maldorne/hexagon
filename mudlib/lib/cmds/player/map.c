
// player `map` cmd: thin facade over CARTOGRAPHY_HANDLER.
//
// `map`            — default chunky ASCII (`[ ]`, `---`, `|`, `/`, `\`)
//                    inside the parchment frame, with legend.
// `map compact`    — single-character-per-location roguelike style. Three
//                    times denser, fits a much larger viewport.
// `map coords`     — coder-only debug view: `(x,y)` of each location.
//                    Useful for verifying coordinate inference.
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

// Seconds after connecting (or reconnecting) before the map answers: what it
// draws is the neighbourhood the cleaner prewarms, and that walk runs a few
// locations per tick.
#define MAP_SETTLE_SECONDS 10

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

  out = _LANG_CMD_MAP_HELP + "\n" + _LANG_CMD_MAP_VARIANTS_HELP;

  if (this_player() && this_player()->query_coder())
    out += _LANG_CMD_MAP_VARIANTS_HELP_CODER;

  return out;
}

private string _format(string str) { return sprintf("%-*s", 24, str); }

// Walk the locations grid and return a mapping of which marker types
// appear so the legend can show only the relevant rows.
private mapping _scan_legend(mapping view)
{
  int ** locations;
  int width, height;
  mapping flags;
  int i, j;

  locations = view["locations"];
  width  = view["width"];
  height = view["height"];

  flags = ([ ]);

  for (i = 0; i < height; i++)
    for (j = 0; j < width; j++)
      flags[locations[i][j]] = 1;

  return flags;
}

// Build the inner content of the parchment frame: the rendered grid,
// a blank line, and the per-marker legend rows. The frames handler
// wraps this in the actual scroll borders.
private string _build_inner(string grid, mapping flags)
{
  string out;
  string * lines;
  int i;

  out = "";

  lines = explode(grid, "\n");
  for (i = 0; i < sizeof(lines); i++)
  {
    if (i == sizeof(lines) - 1 && lines[i] == "")
      continue;
    out += lines[i] + "\n";
  }

  out += "\n";
  out += "                    " + _format(_LANG_CMD_MAP_LEGEND + " :") + "\n";
  out += "                       %^ORANGE%^*%^RESET%^   : " + _format(_LANG_CMD_MAP_YOUR_POS) + "\n";

  if (flags[CART_ENEMY_ROOM])
    out += "                       %^BOLD%^RED%^*%^RESET%^   : " + _format(_LANG_CMD_MAP_ENEMIES) + "\n";
  if (flags[CART_ADVENTURER_ROOM])
    out += "                       %^BOLD%^CYAN%^*%^RESET%^   : " + _format(_LANG_CMD_MAP_FRIENDS) + "\n";
  if (flags[CART_GUARD_ROOM])
    out += "                       %^BOLD%^GREEN%^*%^RESET%^   : " + _format(_LANG_CMD_MAP_GUARDS) + "\n";
  // A door is drawn on the exit segment (a crossed bar), not on the room:
  // show the legend row whenever any door-segment variant is on the grid.
  if (flags[CART_HORIZONTAL_DOOR] || flags[CART_VERTICAL_DOOR] ||
      flags[CART_SLASH_DOOR] || flags[CART_BACKSLASH_DOOR])
    out += "                       " + CART_DOOR_GLYPH_H + "   : " + _format(_LANG_CMD_MAP_DOORS) + "\n";
  if (flags[CART_HOME_ROOM])
    out += "                       " + CART_HOME_GLYPH + "   : " + _format(_LANG_CMD_MAP_HOMES) + "\n";
  if (flags[CART_UP_ROOM])
    out += "                       ^   : " + _format(_LANG_CMD_MAP_UP_STAIRS) + "\n";
  if (flags[CART_DOWN_ROOM])
    out += "                       v   : " + _format(_LANG_CMD_MAP_DOWN_STAIRS) + "\n";
  if (flags[CART_QUEST_ROOM])
    out += "                       %^BOLD%^YELLOW%^!%^RESET%^   : " + _format(_LANG_CMD_MAP_NEW_QUESTS) + "\n";
  if (flags[CART_FINISH_QUEST_ROOM])
    out += "                       %^BOLD%^YELLOW%^?%^RESET%^   : " + _format(_LANG_CMD_MAP_FINISHED_QUESTS) + "\n";
  if (flags[CART_COAST_ROOM])
    out += "                       %^BLUE%^[ ]%^RESET%^ : " + _format(_LANG_CMD_MAP_COAST) + "\n";

  return out;
}

static int cmd(string str, object me, string verb)
{
  mapping view;
  object cart;
  int style;

  // The view is only cheap because the neighbourhood around the player is
  // already resident, and that is true of a player the cleaner can see and
  // has had time to warm. The two cases where it is not:
  //
  //   - a fully invisible coder: they are left out of the retain set, so
  //     their surroundings are not kept loaded for them;
  //   - the first seconds of a session: the prewarm walk is chunked across
  //     ticks and is still running.
  if (me->user() && me->user()->query_invis() == 2)
  {
    notify_fail(_LANG_CMD_MAP_INVISIBLE);
    return 0;
  }

  if (time() - me->query_ontime() < MAP_SETTLE_SECONDS)
  {
    notify_fail(_LANG_CMD_MAP_JUST_ARRIVED);
    return 0;
  }

  // pick a style from the trailing argument
  style = MAP_STYLE_DEFAULT;
  if (str && strlen(str))
  {
    if (member_array(str, _LANG_CMD_MAP_OPT_COMPACT) != -1)
      style = MAP_STYLE_COMPACT;
    else if (member_array(str, _LANG_CMD_MAP_OPT_UNICODE) != -1)
      style = MAP_STYLE_UNICODE;
    else if (member_array(str, _LANG_CMD_MAP_OPT_COLOR) != -1)
      style = MAP_STYLE_COLOR;
    else if (member_array(str, _LANG_CMD_MAP_OPT_COORDS) != -1)
    {
      if (!me->query_coder())
      {
        notify_fail(_LANG_CMD_MAP_UNKNOWN_VARIANT + query_help());
        return 0;
      }
      style = MAP_STYLE_COORDS;
    }
    else
    {
      notify_fail(_LANG_CMD_MAP_UNKNOWN_VARIANT + query_help());
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
      write("\n" +
            handler("frames")->frame(
              _build_inner(cart->render_color_by_area(view),
                           _scan_legend(view)),
              "", 0, 0, "scroll") +
            "\n");
      break;
    case MAP_STYLE_COORDS:
      // each (x,y) tag is 8 chars wide — wraps past the parchment width
      write("\n" + cart->render_coords(view) + "\n");
      break;
    default:
      write("\n" +
            handler("frames")->frame(
              _build_inner(cart->render_ascii(view), _scan_legend(view)),
              "", 0, 0, "scroll") +
            "\n");
      break;
  }

  return 1;
}
