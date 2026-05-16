
/*
 * windows, frames, etc
 *
 * just strings management to make them pretty
 *
 * Each style is a mapping with the following shape (all fields optional
 * unless noted):
 *
 *   "padding_x"  : int                inner horizontal padding (spaces
 *                                     between border and content)
 *   "padding_y"  : int                blank rows top/bottom inside the
 *                                     frame
 *   "margin_x"   : int                outer horizontal margin (spaces
 *                                     before the frame on each side)
 *   "body_left"  : string             left border on every body row
 *   "body_right" : string             right border on every body row
 *   "top"        : ({ row, row... })  ordered list of edge-row specs for
 *                                     the top edge (renders one line each)
 *   "bottom"     : ({ row, row... })  same for the bottom edge
 *   "title_left" : string             optional decoration before title
 *   "title_right": string             optional decoration after title
 *
 * Each row in `top` and `bottom` is an array
 *
 *   ({ left_segment, fill_char, right_segment, adjustment })
 *
 * and renders as
 *
 *   left_segment + fill_char * (body_inner_width + adjustment) + right_segment
 *
 * `adjustment` lets a single style produce the diagonal-offset look of
 * the parchment ("scroll") style, where each border row shifts one
 * column relative to the previous one.
 */

#include <common/frames.h>

inherit "/lib/core/object.c";

mapping styles;

void create()
{
  // box-drawing glyphs. Built with chr(x) so the source remains
  // pure-ASCII and renders identically across editors / OS encodings.
  string vbar, hbar, tl, tr, br, bl, ledge, redge;
  string dvbar, dhbar, dtl, dtr, dbr, dbl, dledge, dredge;
  string vbar2, tl2, tr2, br2, bl2, hbar2, ledge2, redge2;

  ::create();
  styles = ([ ]);

  // light single-line glyphs (default style)
  vbar   = chr(226) + chr(148) + chr(130);              // │  U+2502
  hbar   = chr(226) + chr(148) + chr(128);              // ─  U+2500
  tl     = chr(226) + chr(148) + chr(140);              // ┌  U+250C
  tr     = chr(226) + chr(148) + chr(144);              // ┐  U+2510
  br     = chr(226) + chr(148) + chr(152);              // ┘  U+2518
  bl     = chr(226) + chr(148) + chr(148);              // └  U+2514
  ledge  = chr(226) + chr(148) + chr(164) + " ";        // ┤  U+2524 + space
  redge  = " " + chr(226) + chr(148) + chr(156);        // space + ├  U+251C

  // double-single mixed glyphs (alert / notifications style)
  dvbar  = chr(226) + chr(148) + chr(130);              // │  U+2502
  dhbar  = chr(226) + chr(149) + chr(144);              // ═  U+2550
  dtl    = chr(226) + chr(149) + chr(146);              // ╒  U+2552
  dtr    = chr(226) + chr(149) + chr(149);              // ╕  U+2555
  dbr    = chr(226) + chr(149) + chr(155);              // ╛  U+255B
  dbl    = chr(226) + chr(149) + chr(152);              // ╘  U+2558
  dledge = chr(226) + chr(149) + chr(161) + " ";        // ╡
  dredge = " " + chr(226) + chr(149) + chr(158);        //  ╞

  // double-line glyphs (sidebar style)
  vbar2  = chr(226) + chr(149) + chr(145);              // ║  U+2551
  hbar2  = chr(226) + chr(149) + chr(144);              // ═  U+2550
  tl2    = chr(226) + chr(149) + chr(148);              // ╔  U+2554
  tr2    = chr(226) + chr(149) + chr(151);              // ╗  U+2557
  br2    = chr(226) + chr(149) + chr(157);              // ╝  U+255D
  bl2    = chr(226) + chr(149) + chr(154);              // ╚  U+255A
  ledge2 = chr(226) + chr(149) + chr(161) + " ";        // ╡
  redge2 = " " + chr(226) + chr(149) + chr(158);        //  ╞

  // pure-ASCII fallback for dumb terminals
  styles["simple"] = ([
    "padding_x":   2,
    "padding_y":   1,
    "margin_x":    1,
    "body_left":   "|",
    "body_right":  "|",
    "top":         ({ ({ "+", "-", "+", 0 }) }),
    "bottom":      ({ ({ "+", "-", "+", 0 }) }),
    "title_left":  "| ",
    "title_right": " |",
  ]);

  styles[DEFAULT_FRAME_STYLE] = ([
    "padding_x":   2,
    "padding_y":   1,
    "margin_x":    1,
    "body_left":   vbar,
    "body_right":  vbar,
    "top":         ({ ({ tl, hbar, tr, 0 }) }),
    "bottom":      ({ ({ bl, hbar, br, 0 }) }),
    "title_left":  ledge,
    "title_right": redge,
  ]);

  styles["alert"] = ([
    "padding_x":   3,
    "padding_y":   1,
    "margin_x":    1,
    "body_left":   dvbar,
    "body_right":  dvbar,
    "top":         ({ ({ dtl, dhbar, dtr, 0 }) }),
    "bottom":      ({ ({ dbl, dhbar, dbr, 0 }) }),
    "title_left":  dledge,
    "title_right": dredge,
  ]);

  // notifications: same as alert but the bottom-left corner has a
  // descending pennant, "╘═╕/", giving the ribbon a "tail". The pennant
  // is 3 visible chars wider than a plain corner, so the bottom row
  // shrinks its fill by 3 to keep the right edge aligned with the body.
  styles["notifications"] = ([
    "padding_x":   3,
    "padding_y":   1,
    "margin_x":    1,
    "body_left":   dvbar,
    "body_right":  dvbar,
    "top":         ({ ({ dtl, dhbar, dtr, 0 }) }),
    "bottom":      ({ ({ dbl + dhbar + dtr + "/",
                         dhbar, dbr, -3 }) }),
    "title_left":  dledge,
    "title_right": dredge,
  ]);

  styles["sidebar"] = ([
    "padding_x":   1,
    "padding_y":   0,
    "margin_x":    1,
    "body_left":   vbar2,
    "body_right":  vbar2,
    "top":         ({ ({ tl2, hbar2, tr2, 0 }) }),
    "bottom":      ({ ({ bl2, hbar2, br2, 0 }) }),
    "title_left":  ledge2,
    "title_right": redge2,
  ]);

  // pergamino / scroll. Each top/bottom row is offset by one column
  // relative to its neighbours, using ascii-only chars so the look is
  // stable across encodings. No title support — the visual is too
  // "horizontal scroll" to fit one cleanly.
  // The scroll/parchment style does not have a symmetric outer
  // margin: each border row carries its own diagonal-offset leading
  // whitespace. So margin_x stays at 0 and every row spec bakes in
  // its own leading spaces.
  styles["scroll"] = ([
    "padding_x":   8,
    "padding_y":   0,
    "margin_x":    0,
    "body_left":   "     |",
    "body_right":  "|",
    "top":         ({
                     ({ "    ",      "_", "",     2 }),
                     ({ "   /\\",    " ", "\\",   1 }),
                     ({ "   \\_|",   " ", "|",    0 }),
                   }),
    "bottom":      ({
                     ({ "     |   ", "_", "|___", -3 }),
                     ({ "      \\_/","_", "/",     0 }),
                   }),
  ]);

  // Visual reference for the scroll style (no inline ASCII art in the
  // mapping itself — `\` at the end of a `//` comment is a line
  // continuation in our preprocessor and would silently swallow the
  // next array entry):
  //
  //     ____________________________________________
  //    /                                            \
  //    \_|                                          |
  //      |               <body content>             |
  //      |   _______________________________________|___
  //       \_/________________________________________/
}

mapping query_styles() { return styles; }

private string _repeat(string s, int n)
{
  string out;

  if (n <= 0 || !s || !strlen(s))
    return "";

  out = "";
  while (n-- > 0)
    out += s;

  return out;
}

private string _spaces(int n) { return _repeat(" ", n); }

// Render one edge row: optional outer margin, left segment, fill chars
// repeated (`body_inner_width + adjustment`) times, right segment, and
// the same outer margin again.
private string _edge_row(mixed * spec, int body_inner_width, int margin_x)
{
  int fill_count;
  string fill_run;

  fill_count = body_inner_width + spec[3];
  if (fill_count < 0)
    fill_count = 0;

  fill_run = _repeat(spec[1], fill_count);

  return _spaces(margin_x) + spec[0] + fill_run + spec[2] +
         _spaces(margin_x) + "\n";
}

// Render an edge row that has a title injected in the middle. The
// title sits between `style[title_left]` and `style[title_right]`,
// surrounded by fill chars on both sides so the row reaches the same
// total width as a plain edge row.
private string _edge_row_with_title(mixed * spec, string title,
                                    string title_left, string title_right,
                                    int body_inner_width, int margin_x)
{
  int fill_count, left_fill, right_fill;
  string fill_unit, decorated;
  int title_visible;

  fill_count = body_inner_width + spec[3];
  if (fill_count < 0)
    fill_count = 0;

  decorated = title_left + title + title_right;
  title_visible = strlen(decorated, true);

  if (title_visible >= fill_count)
    // title overflows the row — fall back to plain edge, no title
    return _edge_row(spec, body_inner_width, margin_x);

  // distribute the remaining fill: a touch on the left, the rest on
  // the right (matches the legacy `_header` look)
  fill_unit = spec[1];
  left_fill  = 1;
  right_fill = fill_count - title_visible - left_fill;
  if (right_fill < 0) { left_fill = 0; right_fill = fill_count - title_visible; }

  return _spaces(margin_x) + spec[0] +
         _repeat(fill_unit, left_fill) + decorated +
         _repeat(fill_unit, right_fill) + spec[2] +
         _spaces(margin_x) + "\n";
}

// Render one body row: outer margin, left border, padding, content
// padded right to the inner-content width, padding, right border,
// outer margin.
private string _body_row(string left, string right, string content,
                         int padding_x, int margin_x, int content_width)
{
  string padded;
  int gap;

  padded = content;
  gap = content_width - strlen(padded, true);
  if (gap > 0)
    padded += _spaces(gap);

  return _spaces(margin_x) + left + _spaces(padding_x) + padded +
         _spaces(padding_x) + right + _spaces(margin_x) + "\n";
}

// Empty body row — used for top/bottom vertical padding inside the
// frame.
private string _spacer_row(string left, string right,
                           int padding_x, int margin_x, int content_width)
{
  return _body_row(left, right, "", padding_x, margin_x, content_width);
}

string frame(string content, varargs string title, int width,
             int height, string style_name)
{
  mapping style;
  string * lines;
  mixed ** top_rows, ** bottom_rows;
  int padding_x, padding_y, margin_x;
  int content_width, body_inner_width;
  int i, len, max_len;
  string body_left, body_right;
  string title_left, title_right;
  string out;

  if (!style_name)
    style_name = DEFAULT_FRAME_STYLE;

  // dumb terminals get the ASCII-only fallback regardless of which
  // style was requested
  if (handler("terminal")->is_dumb_terminal(this_user()))
    style_name = "simple";

  style = styles[style_name];
  if (!style)
    return content;

  if (!title)
    title = "";

  title    = fix_string(title);
  lines    = explode(content, "\n");
  for (i = 0; i < sizeof(lines); i++)
    lines[i] = fix_string(lines[i]);

  padding_x = style["padding_x"];
  padding_y = style["padding_y"];
  margin_x  = style["margin_x"];

  body_left   = style["body_left"]   ? style["body_left"]   : "";
  body_right  = style["body_right"]  ? style["body_right"]  : "";
  title_left  = style["title_left"]  ? style["title_left"]  : "";
  title_right = style["title_right"] ? style["title_right"] : "";

  // determine content width: caller's `width` overrides; otherwise
  // pick the longest body line, with the title as a lower bound
  if (width > 0)
  {
    content_width = width;
  }
  else
  {
    max_len = 0;
    if (strlen(title))
    {
      max_len = strlen(title, true) +
                strlen(title_left, true) + strlen(title_right, true);
    }
    for (i = 0; i < sizeof(lines); i++)
    {
      if ((len = strlen(lines[i], true)) > max_len)
        max_len = len;
    }
    content_width = max_len;
  }

  body_inner_width = content_width + 2 * padding_x;

  top_rows    = style["top"]    ? style["top"]    : ({ });
  bottom_rows = style["bottom"] ? style["bottom"] : ({ });

  out = "";

  // top edge: title (if any) goes in the LAST top row, fill rows
  // before it have no title
  for (i = 0; i < sizeof(top_rows); i++)
  {
    if (i == sizeof(top_rows) - 1 && strlen(title))
      out += _edge_row_with_title(top_rows[i], title,
                                  title_left, title_right,
                                  body_inner_width, margin_x);
    else
      out += _edge_row(top_rows[i], body_inner_width, margin_x);
  }

  // top vertical padding
  for (i = 0; i < padding_y; i++)
    out += _spacer_row(body_left, body_right, padding_x, margin_x,
                       content_width);

  // body content
  for (i = 0; i < sizeof(lines); i++)
    out += _body_row(body_left, body_right, lines[i], padding_x,
                    margin_x, content_width);

  // bottom vertical padding
  for (i = 0; i < padding_y; i++)
    out += _spacer_row(body_left, body_right, padding_x, margin_x,
                       content_width);

  // bottom edge
  for (i = 0; i < sizeof(bottom_rows); i++)
    out += _edge_row(bottom_rows[i], body_inner_width, margin_x);

  return out;
}
