# fullmap

Draws the whole world map of a game level: every sector of the level, not a
window around the player, with city names and no frame, legend or player
marker. It is shown on screen and can be written as a coloured HTML page, for a
quick look, or as JSON, for a web renderer to draw it.

## Command

Admin level, `fullmap`:

```
fullmap [<game>] [<z>] [html] [json]
```

- `<game>` defaults to the game of the admin's location.
- `<z>` is the sector level; it defaults to the level of the admin's location
  (0 without coordinates). The map is the location's map, `default` otherwise.
- `html` also writes `/games/<game>/docs/maps/world-<z>.html` (a negative level
  as `world-m5.html`).
- `json` also writes `/games/<game>/docs/maps/world-<z>.json` (same naming).

The extent is the smallest rectangle holding every sector the level has in its
map index. It is drawn with the world map handler, with room beside the map for
the city names, and printed as the rows come, so a narrow terminal wraps them.

## The HTML page

A standalone page with one `<pre>`, monospace on a dark background, UTF-8. Each
run of glyphs with the same colour is a `<span>`; the mud's colour tokens
(`%^GREEN%^`, `%^ORANGE%^`, `%^BOLD%^`, ...) map to the ANSI palette and
`%^RESET%^` clears them. The text is HTML-escaped.

## The JSON file

Format `worldmap/1`: the same grid of glyphs the HTML page shows, with the
city names apart so a renderer can draw them as text over the map.

- `game`, `map`, `z`, `generated`.
- `sector_size`, and `sectors`: `min_x`, `max_x`, `min_y`, `max_y`, the extent
  in sector indices.
- `columns`, `rows_count`: the size of the grid, one cell per glyph.
  `map_left` is how many blank columns stand left of the map: the cell at
  column `c` and row `r` is the sector `min_x + c - map_left`, `max_y - r`
  (columns outside the map are room for names).
- `rows`: one per row, top first: `text`, the glyphs of the row without the
  names (one character per cell), and `runs`, `[first cell, length, style]`
  for every run not in the plain style.
- `labels`: `text`, `row`, `col` (its first cell) and `style` of each name.
- `legend`: `kind` of sector, the `glyph` it is drawn with and its `style`.
- `styles`: `colour` (a token of `palette`, empty for the plain text) and
  `bold`; style 0 is the plain text.
- `palette`: colour token -> CSS colour, the ones the HTML page uses.

## Files

- `master.c` -- the package object (`package("fullmap")`).
- `src/fullmap.c` -- `query_bounds`, `render_level`, `to_html`, `write_html`,
  `to_json`, `write_json`.
- `cmds/admin/fullmap.c` -- the command, with its `.lang.<lang>.h` strings.
