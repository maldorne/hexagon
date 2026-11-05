/*
 * /lib/user/ui.c
 *
 * Terminal interface of a user: a status bar fixed on the top line and a
 * sidebar on the right, drawn with ANSI/VT100 escape sequences over plain
 * telnet. How it works, the sequences and its limits: ui.md, next to this file.
 *
 * Old sidebar prototype, first version in commit 0fcc2ba4. Rewritten for the
 * settings in the "interface" category of the configuration command.
 */

#include <language.h>
#include <user/terminal.h>
#include <cartography.h>

#define CSI (chr(27) + "[")
#define ESC_SAVE    (chr(27) + "7")
#define ESC_RESTORE (chr(27) + "8")

// terminal types that understand the cursor addressing the interface needs
#define UI_TERMS ({ "ansi", "ansi-enye", "ansi-status" })

#define UI_DEFAULT_WIDTH 26
#define UI_MIN_WIDTH     10
#define UI_MAX_WIDTH     40
// the narrowest column of text the sidebar may leave to the game
#define UI_MIN_TEXT      40
#define UI_MIN_ROWS      11
#define UI_MAP_MAX       15
#define UI_WORLD_MAX     12
#define UI_PLACE_LINES    3
// bottom lines of the screen the client keeps for itself (TinTin++ split)
#define UI_MAX_BOTTOM     5

// the status bar has no colours of its own: this ends one of the bars
#define UI_BAR_BASE (CSI + "0m")

// double line box glyphs, the style of the sidebar frame (U+2550 and the
// others of the block)
#define UI_GLYPH(c)  (chr(226) + chr(149) + chr(c))
#define UI_HLINE     UI_GLYPH(144)   // ═
#define UI_VLINE     UI_GLYPH(145)   // ║
#define UI_CORNER_TL UI_GLYPH(148)   // ╔
#define UI_CORNER_TR UI_GLYPH(151)   // ╗
#define UI_CORNER_BL UI_GLYPH(154)   // ╚
#define UI_CORNER_BR UI_GLYPH(157)   // ╝
#define UI_TEE_DOWN  UI_GLYPH(166)   // ╦

// Settings, saved with the account like the terminal type and the size of the
// screen. Whether they are used also depends on the terminal type (see
// ui_terminal_ok): they are never static, but a client that cannot draw them
// never receives a sequence.
int ui_status;     // status bar on the top line
int ui_side;       // sidebar on the right
int ui_width;      // columns of the sidebar, separator included
int ui_bottom;     // lines at the bottom of the screen that are left alone
int ui_map_mode;   // maps of the sidebar: 0 local, 1 world, 2 both, 3 none

// What is on the screen of the connected client right now. This is static on
// purpose: the next connection of the account may be a client with a different
// size, or one that cannot draw any of it, and it starts from a clean screen.
static int ui_live_status;
static int ui_live_side;
static int ui_live_rows;        // the last row of the screen the interface uses
static int ui_live_bottom;
static int ui_live_cols;
static int ui_live_width;

static string ui_status_sent;   // the status bar as last drawn
static string ui_signature;     // what the bar shows, to notice a change
static int ui_pending;          // handle of the call_out of the next repaint
static int ui_last_paint;       // time of the last repaint
static int ui_keep_screen;      // the next layout does not clear the screen
static int ui_busy;             // a sequence of ours is being sent
static int ui_closed;           // the user is leaving: nothing more is drawn

// the map shown in the sidebar is reused for a second
static string * ui_map_lines;
static int ui_map_time;
static int ui_map_width;
static int ui_map_height;
static object ui_map_env;

// and so is the world map, which only changes with the room
static string * ui_world_lines;
static object ui_world_env;
static int ui_world_width;
static int ui_world_height;

void ui_flush();

void create()
{
  ui_status = 0;
  ui_side = 0;
  ui_width = UI_DEFAULT_WIDTH;
  ui_bottom = 0;
  ui_map_mode = 0;
  ui_world_lines = ({ });
  ui_map_lines = ({ });
}

// ---------------------------------------------------------------------------
// Text helpers: the strings here carry colour tokens (%^BOLD%^) and UTF-8
// glyphs, so a width is never strlen.
// ---------------------------------------------------------------------------

/**
 * Columns taken by a string without colour tokens or escapes: every byte that
 * is not a UTF-8 continuation byte is one column.
 */
private int ui_text_width(string str)
{
  int i, len, count, b;

  len = strlen(str);
  count = 0;

  for (i = 0; i < len; i++)
  {
    b = str[i];
    if (b < 0x80 || b >= 0xC0)
      count++;
  }

  return count;
}

/**
 * The first `width` columns of a plain string, never cutting a UTF-8 glyph.
 */
private string ui_cut_text(string str, int width)
{
  int i, len, count, b;

  len = strlen(str);
  count = 0;

  for (i = 0; i < len; i++)
  {
    b = str[i];
    if (b < 0x80 || b >= 0xC0)
    {
      if (count == width)
        return i ? str[0..i - 1] : "";
      count++;
    }
  }

  return str;
}

private string ui_spaces(int count)
{
  if (count < 1)
    return "";

  return sprintf("%*s", count, "");
}

private string ui_repeat(string str, int count)
{
  string ret;
  int i;

  ret = "";
  for (i = 0; i < count; i++)
    ret += str;

  return ret;
}

/**
 * The string with its colour tokens removed.
 */
private string ui_plain(string str)
{
  return TERM_HANDLER->clean_string(str);
}

/**
 * A string, colour tokens included, padded with spaces to exactly `width`
 * columns. One that does not fit loses its colours and is cut: a line wider
 * than its column would spill over the screen.
 */
private string ui_fit(string str, int width)
{
  string plain;
  int len;

  plain = ui_plain(str);
  len = ui_text_width(plain);

  if (len > width)
    return ui_cut_text(plain, width);

  return str + ui_spaces(width - len);
}

/**
 * A plain text broken into lines of at most `width` columns, at the spaces
 * when it can, in the middle of a word when it must.
 */
private string * ui_wrap_text(string text, int width)
{
  string * words, * lines;
  string line, word;
  int i;

  words = explode(text, " ") - ({ "" });
  lines = ({ });
  line = "";

  for (i = 0; i < sizeof(words); i++)
  {
    word = words[i];

    while (ui_text_width(word) > width)
    {
      if (strlen(line))
      {
        lines += ({ line });
        line = "";
      }
      lines += ({ ui_cut_text(word, width) });
      word = word[strlen(ui_cut_text(word, width))..];
    }

    if (!strlen(line))
      line = word;
    else if (ui_text_width(line) + 1 + ui_text_width(word) <= width)
      line += " " + word;
    else
    {
      lines += ({ line });
      line = word;
    }
  }

  if (strlen(line))
    lines += ({ line });

  return lines;
}

/**
 * A bar of `len` glyphs showing cur out of max, coloured by how full it is:
 * green, yellow or red. With `raw` the colours are SGR sequences that go back
 * to the status bar's own colours afterwards, as a colour token would end the
 * bar's background; without it they are colour tokens.
 */
private string ui_meter(int cur, int max, int len, int raw)
{
  string full, empty, colour;
  int filled, percent;

  if (max < 1)
    max = 1;
  if (cur < 0)
    cur = 0;
  if (cur > max)
    cur = max;

  filled = (cur * len) / max;
  percent = (cur * 100) / max;

  // U+2593 and U+2591, the glyphs the bar efun draws
  full = ui_repeat(chr(226) + chr(150) + chr(147), filled);
  empty = ui_repeat(chr(226) + chr(150) + chr(145), len - filled);

  if (raw)
  {
    colour = percent >= 60 ? "32" : (percent >= 30 ? "33" : "31");
    return CSI + "1;" + colour + "m" + full + UI_BAR_BASE + empty;
  }

  colour = percent >= 60 ? "GREEN" : (percent >= 30 ? "YELLOW" : "RED");
  return "%^" + colour + "%^" + full + "%^RESET%^" + empty;
}

// ---------------------------------------------------------------------------
// What the user asked for, and what the terminal can do
// ---------------------------------------------------------------------------

/**
 * Whether the terminal type of the user understands the sequences of the
 * interface (the non ANSI ones receive none).
 */
private int ui_terminal_ok()
{
  return member_array((string)this_object()->query_term_name(), UI_TERMS) != -1;
}

/**
 * The last row of the screen the interface may use: the rows of the terminal
 * less the lines at the bottom that are left to the client.
 */
private int ui_last_row()
{
  return (int)this_object()->query_rows() - ui_bottom;
}

/**
 * Whether the status bar is drawn now: it is on, the terminal can show it and
 * the screen has room for it.
 */
int ui_status_wanted()
{
  return ui_status && ui_terminal_ok() && ui_last_row() >= UI_MIN_ROWS;
}

/**
 * Whether the sidebar is drawn now: it is on, the terminal can show it, the
 * screen has room for it and it leaves the game a column of text wide enough.
 */
int ui_side_wanted()
{
  if (!ui_side || !ui_terminal_ok())
    return 0;

  if (ui_last_row() < UI_MIN_ROWS)
    return 0;

  return (int)this_object()->query_term_cols() - ui_width - 1 >= UI_MIN_TEXT;
}

/**
 * Columns of the terminal the sidebar takes, a gap included: the text of the
 * game is wrapped to what is left (see query_cols in communicate.c).
 */
int ui_reserved_cols()
{
  return ui_side_wanted() ? ui_width + 1 : 0;
}

/**
 * Whether anything of the interface is, or has to be, on the screen. Asked
 * for every message sent, so it stays cheap.
 */
int ui_wanted()
{
  if (ui_closed)
    return 0;

  return ui_live_status || ui_live_side || ui_status_wanted() || ui_side_wanted();
}

int query_ui_status() { return ui_status; }
int query_ui_side() { return ui_side; }
int query_ui_width() { return ui_width; }
int query_ui_bottom() { return ui_bottom; }

/**
 * The maps of the sidebar, as the word the player sees: local, world, both
 * or none.
 */
string query_ui_map_name() { return _LANG_UI_MAP_OPTIONS[ui_map_mode]; }

string * query_ui_map_names() { return _LANG_UI_MAP_OPTIONS; }

/**
 * Choose which maps the sidebar shows, by name. Answers 0 for an unknown one.
 */
int set_ui_map_name(string name)
{
  int mode;

  mode = member_array(name, _LANG_UI_MAP_OPTIONS);
  if (mode == -1)
    return 0;

  ui_map_mode = mode;
  ui_flush();
  this_object()->save_me();
  return 1;
}

/**
 * Turn the status bar on or off. Answers 0, after telling why, when the
 * terminal cannot show it.
 */
int set_ui_status(int on)
{
  if (on && !ui_terminal_ok())
  {
    write(_LANG_UI_NO_ANSI);
    return 0;
  }

  ui_status = on ? 1 : 0;
  ui_flush();
  this_object()->save_me();
  if (on)
    write(_LANG_UI_SIZE_HINT);
  return 1;
}

/**
 * Turn the sidebar on or off. Answers 0, after telling why, when the terminal
 * cannot show it or the screen is too narrow to share.
 */
int set_ui_side(int on)
{
  if (on && !ui_terminal_ok())
  {
    write(_LANG_UI_NO_ANSI);
    return 0;
  }

  if (on && (int)this_object()->query_term_cols() - ui_width - 1 < UI_MIN_TEXT)
  {
    write(_LANG_UI_TOO_NARROW);
    return 0;
  }

  ui_side = on ? 1 : 0;
  ui_flush();
  this_object()->save_me();
  if (on)
    write(_LANG_UI_SIZE_HINT);
  return 1;
}

/**
 * Change the width of the sidebar, within its limits.
 */
int set_ui_width(int width)
{
  if (width < UI_MIN_WIDTH)
    width = UI_MIN_WIDTH;
  if (width > UI_MAX_WIDTH)
    width = UI_MAX_WIDTH;

  ui_width = width;
  ui_flush();
  this_object()->save_me();
  return 1;
}

/**
 * Change how many lines at the bottom of the screen are left alone (0 to 5).
 * A client with an input line of its own, like TinTin++ with its split, needs
 * them: the interface draws, and scrolls, above them.
 */
int set_ui_bottom(int lines)
{
  if (lines < 0)
    lines = 0;
  if (lines > UI_MAX_BOTTOM)
    lines = UI_MAX_BOTTOM;

  ui_bottom = lines;
  ui_flush();
  this_object()->save_me();
  return 1;
}

// ---------------------------------------------------------------------------
// Commands: shortcuts to the same settings as the configuration command
// ---------------------------------------------------------------------------

void ui_commands()
{
  int i;
  string * verbs;

  verbs = _LANG_UI_VERBS_SIDEBAR;
  for (i = 0; i < sizeof(verbs); i++)
    add_action("do_sidebar", verbs[i]);

  verbs = _LANG_UI_VERBS_STATUS;
  for (i = 0; i < sizeof(verbs); i++)
    add_action("do_statusbar", verbs[i]);
}

/**
 * Shared by both verbs: with nothing it tells the state, with on or off it
 * changes it. `side` picks the sidebar, otherwise the status bar.
 */
private int ui_toggle(string str, int side)
{
  int on;

  if (!str || !strlen(trim(str)))
  {
    if (side)
      write(_LANG_UI_SIDEBAR_STATE + _LANG_UI_SIDEBAR_SYNTAX);
    else
      write(_LANG_UI_STATUS_STATE + _LANG_UI_STATUS_SYNTAX);
    return 1;
  }

  str = lower_case(trim(str));

  if (affirmative(str))
    on = 1;
  else if (negative(str))
    on = 0;
  else
  {
    notify_fail(side ? _LANG_UI_SIDEBAR_SYNTAX : _LANG_UI_STATUS_SYNTAX);
    return 0;
  }

  // the setting says by itself why it could not be turned on
  if (!(side ? set_ui_side(on) : set_ui_status(on)))
    return 1;

  if (side)
    write(on ? _LANG_UI_SIDEBAR_ON : _LANG_UI_SIDEBAR_OFF);
  else
    write(on ? _LANG_UI_STATUS_ON : _LANG_UI_STATUS_OFF);

  return 1;
}

int do_sidebar(string str) { return ui_toggle(str, 1); }
int do_statusbar(string str) { return ui_toggle(str, 0); }

// ---------------------------------------------------------------------------
// Content
// ---------------------------------------------------------------------------

/**
 * The hour of the game clock of the game the player is in, or -1.
 */
private int ui_game_hour(object player)
{
  object weather;
  int * date;

  weather = handler("weather", player);
  if (!weather)
    return -1;

  date = weather->query_date_data();
  return sizeof(date) ? date[0] : -1;
}

private string ui_place_text(object player)
{
  string place;

  if (!environment(player))
    return "";

  place = environment(player)->short();
  return stringp(place) ? ui_plain(place) : "";
}

/**
 * What the status bar and the panel show, as a string: it changes when
 * anything of them does.
 */
private string ui_make_signature(object player)
{
  return player->query_cap_name() + ":" + player->query_hp() + "/" +
         player->query_max_hp() + ":" + player->query_gp() + "/" +
         player->query_max_gp() + ":" + object_name(environment(player)) + ":" +
         ui_game_hour(player);
}

/**
 * The line under the status bar, on row 2, across the whole screen. Where the
 * sidebar is, its frame hangs from it: a tee down on its left edge and one
 * that closes the corner on its right.
 */
private string ui_rule(int cols)
{
  string out;

  if (!ui_live_side)
    return CSI + "2;1H" + ui_repeat(UI_HLINE, cols);

  out = ui_repeat(UI_HLINE, cols - ui_live_width) + UI_TEE_DOWN +
        ui_repeat(UI_HLINE, ui_live_width - 2) + UI_CORNER_TR;

  return CSI + "2;1H" + out;
}

/**
 * The status bar as one string: the position of its line, colours and the
 * text padded to the whole width of the screen. It shows the name, life and
 * energy points and the game hour, losing the least important parts when the
 * screen is narrow. The place is left to the sidebar.
 */
private string ui_status_line(object player, int cols)
{
  string name, hp_text, gp_text, hour_text;
  string left, middle, right;
  int hp, max_hp, gp, max_gp, hour;
  int tier, blen, left_width, rest;

  name = ui_plain(player->query_cap_name());
  hp = player->query_hp();
  max_hp = player->query_max_hp();
  gp = player->query_gp();
  max_gp = player->query_max_gp();
  hp_text = hp + "/" + max_hp;
  gp_text = gp + "/" + max_gp;

  // from the full bar to the barest: bars of points, only their numbers, only life
  for (tier = 0; tier < 3; tier++)
  {
    blen = tier == 0 ? 8 : 0;

    left = " ";
    left_width = 1;

    if (tier < 2)
    {
      left += name + "  ";
      left_width += ui_text_width(name) + 2;
    }

    left += _LANG_UI_HP + " ";
    left_width += strlen(_LANG_UI_HP) + 1;
    if (blen)
    {
      left += ui_meter(hp, max_hp, blen, 1) + " ";
      left_width += blen + 1;
    }
    left += hp_text + "  ";
    left_width += strlen(hp_text) + 2;

    if (tier < 2)
    {
      left += _LANG_UI_GP + " ";
      left_width += strlen(_LANG_UI_GP) + 1;
      if (blen)
      {
        left += ui_meter(gp, max_gp, blen, 1) + " ";
        left_width += blen + 1;
      }
      left += gp_text + "  ";
      left_width += strlen(gp_text) + 2;
    }

    if (left_width <= cols)
      break;
  }

  rest = cols - left_width;

  // the game hour, on the right, when there is room for it
  right = "";
  hour = ui_game_hour(player);
  if (hour >= 0 && rest >= 7)
  {
    hour_text = " " + (hour < 10 ? "0" : "") + hour + ":00 ";
    right = hour_text;
    rest -= ui_text_width(hour_text);
  }

  middle = ui_spaces(rest);

  // a status bar too wide for the screen would wrap and push everything down
  if (rest < 0)
    left = ui_cut_text(ui_plain(left), cols);

  return CSI + "1;1H" + left + middle + right + UI_BAR_BASE + ui_rule(cols);
}

/**
 * The lines of the compact map of the cartography handler around the player.
 */
private string * ui_render_map(object player, int width, int height)
{
  mapping view;

  view = CARTOGRAPHY_HANDLER->query_map_view(player,
           ([ "deep" : 0, "width" : width, "height" : height ]));

  if (!view)
    return ({ });

  return explode(CARTOGRAPHY_HANDLER->render_compact(view), "\n");
}

/**
 * The map around the player, `height` lines of `width` columns, from the map
 * of the cartography handler. It is reused for a second while the player does
 * not move. Empty lines when there is nothing to draw (a place without a map).
 */
private string * ui_map(object player, int width, int height)
{
  string * lines;
  int i;

  if (ui_map_env == environment(player) && ui_map_time == time() &&
      ui_map_width == width && ui_map_height == height)
    return ui_map_lines;

  lines = ({ });

  // a place that cannot draw itself leaves the map empty
  catch(lines = ui_render_map(player, width, height));

  for (i = sizeof(lines); i < height; i++)
    lines += ({ "" });

  ui_map_lines = lines[0..height - 1];
  ui_map_env = environment(player);
  ui_map_time = time();
  ui_map_width = width;
  ui_map_height = height;

  return ui_map_lines;
}

/**
 * A window of the world map around the sector of the player, `height` lines of
 * `width` columns, from the world map handler. Empty when the place the player
 * is in has no world map (the caller then falls back to the local map).
 */
private string * ui_world_map(object player, int width, int height)
{
  string * lines;
  string text;
  int i;

  if (ui_world_env == environment(player) && ui_world_width == width &&
      ui_world_height == height)
    return ui_world_lines;

  lines = ({ });
  text = nil;

  catch(text = handler("worldmap", player)->render_around(player, width, height, 1, 0));

  if (stringp(text) && strlen(text))
  {
    lines = explode(text, "\n");
    for (i = sizeof(lines); i < height; i++)
      lines += ({ "" });
    lines = lines[0..height - 1];
  }

  ui_world_lines = lines;
  ui_world_env = environment(player);
  ui_world_width = width;
  ui_world_height = height;

  return lines;
}

/**
 * One line of the points: label, bar and numbers, `width` columns wide. The
 * numbers are right aligned in a column of `numbers` columns, so both bars
 * have the same length.
 */
private string ui_points_line(string label, int cur, int max, int width,
                              int numbers)
{
  string text;
  int blen;

  if (!numbers)
  {
    blen = width - strlen(label) - 1;
    return "%^BOLD%^" + label + "%^RESET%^ " + ui_meter(cur, max, blen, 0);
  }

  text = cur + "/" + max;
  text = ui_spaces(numbers - strlen(text)) + text;

  blen = width - strlen(label) - 2 - numbers;
  if (blen < 3)
    blen = 3;

  return "%^BOLD%^" + label + "%^RESET%^ " + ui_meter(cur, max, blen, 0) +
         " " + text;
}

/**
 * The lines of the sidebar, exactly `rows` of them, each `width` columns wide
 * and in colour tokens: a separator and a margin, then the map, the points and
 * the place. When the screen is too low for the map it is left out.
 */
private string * ui_panel_lines(object player, int width, int rows)
{
  string * lines, * place, * map, * world;
  string text_hp, text_gp;
  int inner, numbers, local_height, world_height, available, usable;
  int max_place, i;

  // the frame takes the two edges, and a margin the left one
  inner = width - 3;

  // the place the player is in, at most a few lines of it
  place = ui_wrap_text(ui_place_text(player), inner);
  max_place = inner < 16 ? UI_PLACE_LINES + 2 : UI_PLACE_LINES;
  if (sizeof(place) > max_place)
    place = place[0..max_place - 1];

  // what is left for the maps once the rest is in: the points (a title and two
  // lines), the place (a title and its lines) and a blank line after each; a
  // map costs its own title and a blank line besides its rows
  available = rows - (5 + sizeof(place));
  local_height = 0;
  world_height = 0;

  if (ui_map_mode == 0)
    local_height = available - 2;
  else if (ui_map_mode == 1)
    world_height = available - 2;
  else if (ui_map_mode == 2)
  {
    // the world map is the first to go when the panel is short
    usable = available - 4;
    world_height = usable / 3;
    local_height = usable - world_height;
    if (world_height < 3)
    {
      world_height = 0;
      local_height = available - 2;
    }
  }

  if (local_height > UI_MAP_MAX)
    local_height = UI_MAP_MAX;
  if (world_height > UI_WORLD_MAX)
    world_height = UI_WORLD_MAX;
  if (local_height < 3)
    local_height = 0;
  if (world_height < 3)
    world_height = 0;

  world = ({ });
  if (world_height)
  {
    world = ui_world_map(player, inner, world_height);

    // no world map where the player is: the local one takes its place
    if (!sizeof(world))
    {
      world_height = 0;
      local_height = available - 2 > UI_MAP_MAX ? UI_MAP_MAX : available - 2;
      if (local_height < 3)
        local_height = 0;
    }
  }

  lines = ({ });

  if (local_height)
  {
    lines += ({ "%^BOLD%^" + _LANG_UI_MAP + "%^RESET%^" });
    map = ui_map(player, inner, local_height);
    lines += map;
    lines += ({ "" });
  }

  if (world_height)
  {
    lines += ({ "%^BOLD%^" + _LANG_UI_WORLD + "%^RESET%^" });
    lines += world;
    lines += ({ "" });
  }

  text_hp = player->query_hp() + "/" + player->query_max_hp();
  text_gp = player->query_gp() + "/" + player->query_max_gp();
  numbers = strlen(text_hp) > strlen(text_gp) ? strlen(text_hp) : strlen(text_gp);

  // a narrow panel has no room for the numbers, only the bars
  if (inner < 18)
    numbers = 0;

  lines += ({ "%^BOLD%^" + _LANG_UI_STATS + "%^RESET%^" });
  lines += ({ ui_points_line(_LANG_UI_HP, player->query_hp(),
                             player->query_max_hp(), inner, numbers) });
  lines += ({ ui_points_line(_LANG_UI_GP, player->query_gp(),
                             player->query_max_gp(), inner, numbers) });
  lines += ({ "" });
  lines += ({ "%^BOLD%^" + _LANG_UI_PLACE + "%^RESET%^" });
  lines += place;

  for (i = sizeof(lines); i < rows; i++)
    lines += ({ "" });

  lines = lines[0..rows - 1];

  // every line gets its edges and margin and is padded to the full width, so
  // what was there before is overwritten
  for (i = 0; i < sizeof(lines); i++)
    lines[i] = UI_VLINE + " " + ui_fit(lines[i], inner) + UI_VLINE;

  return lines;
}

/**
 * The whole sidebar as one string: each line has its own position, so it can
 * be painted over whatever the game scrolled over it.
 */
private string ui_panel(object player, int cols, int rows)
{
  string * lines;
  string out;
  int i, top, column, width;

  // the top edge is row 1, or the line of the status bar on row 2, which
  // already carries the corners of the frame
  top = ui_live_status ? 2 : 1;
  width = ui_live_width;
  column = cols - width + 1;
  lines = ui_panel_lines(player, width, rows - top - 1);
  out = "";

  if (!ui_live_status)
    out += CSI + top + ";" + column + "H" + UI_CORNER_TL +
           ui_repeat(UI_HLINE, width - 2) + UI_CORNER_TR;

  for (i = 0; i < sizeof(lines); i++)
    out += CSI + (top + 1 + i) + ";" + column + "H" +
           TERM_HANDLER->_fix_string(lines[i], this_object());

  out += CSI + rows + ";" + column + "H" + UI_CORNER_BL +
         ui_repeat(UI_HLINE, width - 2) + UI_CORNER_BR;

  return out;
}

// ---------------------------------------------------------------------------
// The screen
// ---------------------------------------------------------------------------

/**
 * The sequences that take away what the interface has on the screen: the
 * scroll region goes back to the whole screen, the status bar line and the
 * columns of the sidebar are cleared. Nothing when nothing is drawn. The
 * cursor stays where it was.
 */
private string ui_teardown()
{
  string out;
  int i, top;

  if (!ui_live_status && !ui_live_side)
    return "";

  out = ESC_SAVE;

  // only the status bar set a scroll region; with lines left to the client it
  // goes back to what is above them, not to the whole screen
  if (ui_live_status)
    out += ui_live_bottom ? CSI + "1;" + ui_live_rows + "r" : CSI + "r";

  if (ui_live_status)
    out += CSI + "1;1H" + CSI + "2K" + CSI + "2;1H" + CSI + "2K";

  if (ui_live_side)
  {
    top = ui_live_status ? 2 : 1;
    for (i = top; i <= ui_live_rows; i++)
      out += CSI + i + ";" + (ui_live_cols - ui_live_width + 1) + "H" +
             ui_spaces(ui_live_width);
  }

  return out + ESC_RESTORE;
}

/**
 * Whether what is on the screen is what the settings ask for now.
 */
private int ui_layout_matches(int status_on, int side_on, int rows, int cols)
{
  if (status_on != ui_live_status || side_on != ui_live_side)
    return 0;

  if (!status_on && !side_on)
    return 1;

  if (rows != ui_live_rows || cols != ui_live_cols)
    return 0;

  return !side_on || ui_width == ui_live_width;
}

/**
 * Bring the screen of the client to what the settings ask for: take away
 * what is no longer wanted, set up a new layout, and draw the status bar when
 * it changed and the sidebar always, in one string, so that a client never
 * shows it half drawn. Does nothing for a user whose options are off.
 *
 * The sidebar is repainted after every output, because the game scrolls the
 * screen over it; the status bar sits outside the scroll region and only when
 * its text changes. The cursor is saved before and restored after.
 */
void ui_flush()
{
  object player;
  string out, paint, bar;
  int status_on, side_on, rows, cols, redraw_bar, i;

  if (ui_pending)
  {
    remove_call_out(ui_pending);
    ui_pending = 0;
  }

  if (ui_closed || !interactive(this_object()))
    return;

  // nothing is drawn before the login is over: the account is known as soon
  // as its name is typed, but there is no game to show yet
  player = this_object()->player();
  if (!player || !environment(player))
    return;

  status_on = ui_status_wanted();
  side_on = ui_side_wanted();
  rows = ui_last_row();
  cols = (int)this_object()->query_term_cols();
  out = "";

  if (!ui_layout_matches(status_on, side_on, rows, cols))
  {
    out += ui_teardown();

    ui_live_status = status_on;
    ui_live_side = side_on;
    ui_live_rows = rows;
    ui_live_bottom = ui_bottom;
    ui_live_cols = cols;
    ui_live_width = ui_width;
    ui_status_sent = "";

    // a new layout starts on clean lines, the cursor on the last one, but only
    // those of the interface: the lines the client keeps for itself are not
    // touched, and a login does not wipe what the player has just been shown
    if ((status_on || side_on) && !ui_keep_screen)
    {
      for (i = 1; i <= rows; i++)
        out += CSI + i + ";1H" + CSI + "2K";
      out += CSI + rows + ";1H";
    }
    ui_keep_screen = 0;
  }

  if (ui_live_status || ui_live_side)
  {
    paint = "";
    redraw_bar = 0;

    if (ui_live_status)
    {
      bar = ui_status_line(player, ui_live_cols);
      redraw_bar = (bar != ui_status_sent);
    }

    // the scroll region keeps the game away from the top line; DECSTBM moves
    // the cursor, which is why everything is inside a save and restore
    if (ui_live_status && (redraw_bar || ui_live_side))
      paint += CSI + "3;" + ui_live_rows + "r";

    if (redraw_bar)
    {
      paint += bar;
      ui_status_sent = bar;
    }

    if (ui_live_side)
      paint += ui_panel(player, ui_live_cols, ui_live_rows);

    if (strlen(paint))
      out += ESC_SAVE + paint + ESC_RESTORE;

    ui_signature = ui_make_signature(player);
  }

  ui_last_paint = time();

  if (strlen(out))
  {
    ui_busy = 1;
    // not this object's send_message: that one feeds snoopers and schedules a
    // repaint of its own
    send_message(out);
    ui_busy = 0;
  }
}

/**
 * Call_out target of the coalesced repaint.
 */
void ui_tick()
{
  ui_pending = 0;
  ui_flush();
}

/**
 * Ask for a repaint after what the game is sending now. Many messages sent in
 * the same moment cost one repaint, which runs when the current command has
 * finished, and never more than one per second.
 */
void ui_schedule()
{
  if (ui_pending || ui_busy)
    return;

  ui_pending = call_out("ui_tick", ui_last_paint == time() ? 1 : 0);
}

/**
 * Whether a sequence of the interface is being sent: the sending of it must
 * not ask for a repaint.
 */
int ui_in_use() { return ui_busy; }

/**
 * Called from the heart beat: repaint when what the bar shows has changed
 * (points, place, game hour), which no output announces.
 */
void ui_heart_beat()
{
  object player;

  if (!ui_wanted())
    return;

  player = this_object()->player();
  if (!player || !environment(player))
    return;

  if (ui_make_signature(player) != ui_signature)
    ui_schedule();
}

/**
 * Called at the end of the login: the settings of the account are applied.
 * What the player has just been shown stays on the screen.
 */
void ui_start()
{
  if (!ui_wanted())
    return;

  ui_keep_screen = 1;
  ui_schedule();
}

/**
 * Leave the screen of the client as it was: used when the user leaves.
 */
void ui_reset()
{
  string out;

  out = ui_teardown();

  ui_closed = 1;
  ui_live_status = 0;
  ui_live_side = 0;

  if (strlen(out) && interactive(this_object()))
    send_message(out);
}

mixed * stats()
{
  return ({
    ({ "UI Status bar", ui_status, }),
    ({ "UI Sidebar", ui_side, }),
    ({ "UI Sidebar width", ui_width, }),
          });
}
