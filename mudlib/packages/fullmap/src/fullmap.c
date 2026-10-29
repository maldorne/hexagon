// fullmap engine: finds the extent of a level of a map index, asks the world
// map handler to draw all of it and turns the colour codes into HTML.

#include <sector/sector.h>

// blank columns beside the map where a city name may go
#define LABEL_ROOM 24

// colour token -> CSS colour, on a dark background (the ANSI palette)
private mapping css_colours;

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
