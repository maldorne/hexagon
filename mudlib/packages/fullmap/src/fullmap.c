// fullmap engine: finds the extent of a level of a map index, asks the world
// map handler to draw all of it and turns the colour codes into HTML, or into
// JSON for a web renderer.

#include <sector/sector.h>

// blank columns beside the map where a city name may go
#define LABEL_ROOM 24

// colour token -> CSS colour, on a dark background (the ANSI palette)
private mapping css_colours;

// the styles of one JSON export: "<colour>|<bold>" -> index, and the list
private mapping style_index;
private mixed * style_list;

void create()
{
  css_colours = ([
    "BLACK":   "#555555",
    "RED":     "#cd3131",
    "GREEN":   "#0dbc79",
    "ORANGE":  "#b5892a",
    "YELLOW":  "#e5e510",
    "BLUE":    "#4a8fe8",
    "MAGENTA": "#bc3fbc",
    "CYAN":    "#11a8cd",
    "WHITE":   "#e5e5e5",
  ]);
}

/**
 * The extent of the sectors a level of a map has.
 *
 * @param game the game slug
 * @param map_name the map of the game
 * @param z the sector level
 * @return ({ min x, max x, min y, max y }) in sector indices, nil when the
 *         level has no sector
 */
int * query_bounds(string game, string map_name, int z)
{
  mapping level, row;
  int * ys, * xs;
  int i, j, minx, maxx, miny, maxy, found;

  level = load_object("/lib/handlers/sectors")->query_map_index(game, map_name)->query_level(z);
  if (!level)
    return nil;

  ys = map_indices(level);
  for (i = 0; i < sizeof(ys); i++)
  {
    row = level[ys[i]];
    if (!row)
      continue;
    xs = map_indices(row);
    for (j = 0; j < sizeof(xs); j++)
    {
      if (!row[xs[j]])
        continue;
      if (!found)
      {
        minx = maxx = xs[j];
        miny = maxy = ys[i];
        found = 1;
      }
      if (xs[j] < minx) minx = xs[j];
      if (xs[j] > maxx) maxx = xs[j];
      if (ys[i] < miny) miny = ys[i];
      if (ys[i] > maxy) maxy = ys[i];
    }
  }

  return found ? ({ minx, maxx, miny, maxy }) : nil;
}

/**
 * Draws every sector of a level, with city names and nothing else.
 *
 * @param game the game slug
 * @param map_name the map of the game
 * @param z the sector level
 * @return the rows with their colour codes, nil when the level is empty
 */
string render_level(string game, string map_name, int z)
{
  int * b;
  int width, height;

  b = query_bounds(game, map_name, z);
  if (!b)
    return nil;

  width = b[1] - b[0] + 1;
  height = b[3] - b[2] + 1;

  // render() centres on a world coord: the sector at width/2 columns from the
  // left and height/2 rows from the top
  return handler("worldmap")->render(
    (b[0] + width / 2) * SECTOR_SIZE, (b[3] - height / 2) * SECTOR_SIZE,
    z * SECTOR_SIZE, game, map_name, width, height, 0, 1, LABEL_ROOM);
}

/**
 * Turns text with %^COLOUR%^ codes into HTML spans, escaped.
 *
 * @param text the rows of the map
 * @return the body of the page: a <pre> with one span per run of one style
 */
string to_html(string text)
{
  string * parts;
  string out, colour, chunk, style;
  int i, bold;

  out = "";
  colour = "";
  bold = 0;

  // a leading marker keeps the text before the first code as part 0, whatever
  // explode does with a separator at the start
  parts = explode("X" + text, "%^");
  parts[0] = parts[0][1..];
  for (i = 0; i < sizeof(parts); i++)
  {
    // odd parts sit between two %^ and are tokens; even ones are text
    if (i % 2 == 1)
    {
      if (parts[i] == "RESET")
      {
        colour = "";
        bold = 0;
      }
      else if (parts[i] == "BOLD")
        bold = 1;
      else if (css_colours[parts[i]])
        colour = css_colours[parts[i]];
      continue;
    }

    chunk = parts[i];
    if (!strlen(chunk))
      continue;

    chunk = replace_string(chunk, "&", "&amp;");
    chunk = replace_string(chunk, "<", "&lt;");
    chunk = replace_string(chunk, ">", "&gt;");

    style = "";
    if (strlen(colour))
      style += "color:" + colour + ";";
    if (bold)
      style += "font-weight:bold;";

    if (strlen(style))
      out += "<span style=\"" + style + "\">" + chunk + "</span>";
    else
      out += chunk;
  }

  return "<!doctype html>\n<html>\n<head>\n<meta charset=\"utf-8\">\n" +
    "<title>World map</title>\n<style>\n" +
    "body { background: #111; margin: 0; }\n" +
    "pre { color: #aaa; font: 10px/10px monospace; margin: 0; padding: 8px; }\n" +
    "</style>\n</head>\n<body>\n<pre>" + out + "</pre>\n</body>\n</html>\n";
}

/**
 * Draws a level and writes it as a standalone HTML page.
 *
 * @param game the game slug
 * @param map_name the map of the game
 * @param z the sector level
 * @return the file written, nil when the level is empty
 */
string write_html(string game, string map_name, int z)
{
  string text, file;

  text = render_level(game, map_name, z);
  if (!text)
    return nil;

  mkdir("/games/" + game + "/docs");
  mkdir("/games/" + game + "/docs/maps");
  file = "/games/" + game + "/docs/maps/world-" +
         (z < 0 ? "m" + (-z) : "" + z) + ".html";
  remove_file(file);
  write_file(file, to_html(text));
  return file;
}

/**
 * Today as YYYY-MM-DD (UTC), from the clock.
 */
private string today()
{
  int days, era, doe, yoe, year, doy, mp, day, month;

  // days since 1970-01-01 -> civil date
  days = time() / 86400 + 719468;
  era = days / 146097;
  doe = days - era * 146097;
  yoe = (doe - doe / 1460 + doe / 36524 - doe / 146096) / 365;
  year = yoe + era * 400;
  doy = doe - (365 * yoe + yoe / 4 - yoe / 100);
  mp = (5 * doy + 2) / 153;
  day = doy - (153 * mp + 2) / 5 + 1;
  month = mp < 10 ? mp + 3 : mp - 9;

  if (month <= 2)
    year++;

  return year + "-" + (month < 10 ? "0" : "") + month + "-" +
         (day < 10 ? "0" : "") + day;
}

/**
 * The index of a style in the export being built, added when new. Style 0 is
 * the plain text colour.
 */
private int style_of(string colour, int bold)
{
  string key;

  key = colour + "|" + bold;
  if (undefinedp(style_index[key]))
  {
    style_index[key] = sizeof(style_list);
    style_list += ({ ([ "colour": colour, "bold": bold ]) });
  }
  return style_index[key];
}

/**
 * Splits a row with %^COLOUR%^ codes into its cells: one character each, UTF-8
 * aware, with the style it is drawn in.
 *
 * @param text one row of the map
 * @return ({ glyphs, styles }), two arrays of the same size
 */
private mixed * row_cells(string text)
{
  string * parts, * glyphs;
  int * styles;
  string colour, chunk;
  int i, j, k, bold, style;

  glyphs = ({ });
  styles = ({ });
  colour = "";
  bold = 0;

  parts = explode("X" + text, "%^");
  parts[0] = parts[0][1..];
  for (i = 0; i < sizeof(parts); i++)
  {
    if (i % 2 == 1)
    {
      if (parts[i] == "RESET")
      {
        colour = "";
        bold = 0;
      }
      else if (parts[i] == "BOLD")
        bold = 1;
      else if (css_colours[parts[i]])
        colour = parts[i];
      continue;
    }

    chunk = parts[i];
    style = style_of(colour, bold);

    // a cell is a lead byte and the continuation bytes (10xxxxxx) after it
    for (j = 0; j < strlen(chunk); j = k)
    {
      for (k = j + 1; k < strlen(chunk) && (chunk[k] & 0xC0) == 0x80; k++);
      glyphs += ({ chunk[j..k - 1] });
      styles += ({ style });
    }
  }

  return ({ glyphs, styles });
}

/**
 * The runs of one style in a row, as ({ first cell, length, style }), plain
 * text left out.
 */
private mixed * style_runs(int * styles)
{
  mixed * runs;
  int i, start;

  runs = ({ });
  for (i = 0; i < sizeof(styles); i = start)
  {
    for (start = i + 1; start < sizeof(styles) && styles[start] == styles[i];
         start++);
    if (styles[i])
      runs += ({ ({ i, start - i, styles[i] }) });
  }
  return runs;
}

/**
 * The city names of a level, told apart by drawing it with and without them:
 * each run of cells that changed is a name, a single blank between two runs
 * kept inside it.
 *
 * @return ({ ([ "text", "row", "col", "style" ]) ... })
 */
private mapping * find_labels(mixed ** with_names, mixed ** without)
{
  mapping * labels;
  string * a, * b;
  string text;
  int r, c, start, end;

  labels = ({ });
  for (r = 0; r < sizeof(with_names); r++)
  {
    a = with_names[r][0];
    b = without[r][0];
    for (c = 0; c < sizeof(a); c++)
    {
      if (c >= sizeof(b) || a[c] == b[c])
        continue;

      start = c;
      end = c;
      while (end + 1 < sizeof(a) &&
             (a[end + 1] != b[end + 1] ||
              (a[end + 1] == " " && end + 2 < sizeof(a) &&
               a[end + 2] != b[end + 2])))
        end++;

      text = implode(a[start..end], "");
      labels += ({ ([ "text": text, "row": r, "col": start,
                      "style": with_names[r][1][start] ]) });
      c = end;
    }
  }
  return labels;
}

/**
 * Draws a level as JSON (format worldmap/1): the rows of glyphs with their
 * style runs and the city names apart, so a web renderer can draw the same
 * map the HTML page shows.
 *
 * @param game the game slug
 * @param map_name the map of the game
 * @param z the sector level
 * @return the JSON text, nil when the level is empty
 */
string to_json(string game, string map_name, int z)
{
  object worldmap;
  string text, out;
  string * rows_named, * rows_plain, * legend_kinds;
  mixed ** named, ** plain;
  mixed * legend_cell;
  mapping legend, * labels;
  int * b;
  int width, height, left, cx, cy, i, j;

  b = query_bounds(game, map_name, z);
  if (!b)
    return nil;

  width = b[1] - b[0] + 1;
  height = b[3] - b[2] + 1;
  left = LABEL_ROOM / 2;
  cx = (b[0] + width / 2) * SECTOR_SIZE;
  cy = (b[3] - height / 2) * SECTOR_SIZE;
  worldmap = handler("worldmap");

  style_index = ([ ]);
  style_list = ({ });
  style_of("", 0);

  text = worldmap->render(cx, cy, z * SECTOR_SIZE, game, map_name,
                          width, height, 0, 1, LABEL_ROOM);
  rows_named = explode(text, "\n");
  text = worldmap->render(cx, cy, z * SECTOR_SIZE, game, map_name,
                          width, height, 0, 0, 0);
  rows_plain = explode(text, "\n");

  named = allocate(sizeof(rows_named));
  plain = allocate(sizeof(rows_plain));
  for (i = 0; i < sizeof(rows_named); i++)
    named[i] = row_cells(rows_named[i]);

  // the plain rows get the same blank columns the named ones have
  for (i = 0; i < sizeof(rows_plain); i++)
  {
    plain[i] = row_cells(rows_plain[i]);
    plain[i][0] = allocate(left) + plain[i][0] +
                  allocate(LABEL_ROOM - left);
    plain[i][1] = allocate_int(left) + plain[i][1] +
                  allocate_int(LABEL_ROOM - left);
    for (j = 0; j < sizeof(plain[i][0]); j++)
      if (!plain[i][0][j])
        plain[i][0][j] = " ";
  }

  labels = find_labels(named, plain);

  out = "{\"format\":\"worldmap/1\",\"game\":" + json_encode(game) +
        ",\"map\":" + json_encode(map_name) + ",\"z\":" + z +
        ",\"generated\":" + json_encode(today()) +
        ",\n\"sector_size\":" + SECTOR_SIZE +
        ",\"sectors\":{\"min_x\":" + b[0] + ",\"max_x\":" + b[1] +
        ",\"min_y\":" + b[2] + ",\"max_y\":" + b[3] + "}" +
        ",\n\"columns\":" + (width + LABEL_ROOM) + ",\"rows_count\":" +
        sizeof(plain) + ",\"map_left\":" + left +
        ",\n\"palette\":" + json_encode(css_colours) +
        ",\n\"rows\":[\n";

  for (i = 0; i < sizeof(plain); i++)
    out += (i ? ",\n" : "") + "{\"text\":" +
           json_encode(implode(plain[i][0], "")) + ",\"runs\":" +
           json_encode(style_runs(plain[i][1])) + "}";

  out += "\n],\n\"labels\":[\n";
  for (i = 0; i < sizeof(labels); i++)
    out += (i ? ",\n" : "") + json_encode(labels[i]);

  // the glyph each kind of sector is drawn with, for a legend
  legend = worldmap->query_legend_glyphs();
  legend_kinds = map_indices(legend) - ({ "player" });
  out += "\n],\n\"legend\":[\n";
  for (i = 0; i < sizeof(legend_kinds); i++)
  {
    legend_cell = row_cells(legend[legend_kinds[i]][1]);
    out += (i ? ",\n" : "") + json_encode(([
      "kind": legend_kinds[i],
      "glyph": sizeof(legend_cell[0]) ? legend_cell[0][0] : "",
      "style": sizeof(legend_cell[1]) ? legend_cell[1][0] : 0 ]));
  }

  // styles last: the rows, labels and legend above add to them
  out += "\n],\n\"styles\":" + json_encode(style_list) + "}\n";
  return out;
}

/**
 * Draws a level and writes it as JSON for a web renderer.
 *
 * @param game the game slug
 * @param map_name the map of the game
 * @param z the sector level
 * @return the file written, nil when the level is empty
 */
string write_json(string game, string map_name, int z)
{
  string json, file;

  json = to_json(game, map_name, z);
  if (!json)
    return nil;

  mkdir("/games/" + game + "/docs");
  mkdir("/games/" + game + "/docs/maps");
  file = "/games/" + game + "/docs/maps/world-" +
         (z < 0 ? "m" + (-z) : "" + z) + ".json";
  remove_file(file);
  write_file(file, json);
  return file;
}
