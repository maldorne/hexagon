# fullmap

Draws the whole world map of a game level: every sector of the level, not a
window around the player, with city names and no frame, legend or player
marker. It is shown on screen and can be written as a coloured HTML page, to
capture the complete map for the web.

## Command

Admin level, `fullmap`:

```
fullmap [<game>] [<z>] [html]
```

- `<game>` defaults to the game of the admin's location.
- `<z>` is the sector level; it defaults to the level of the admin's location
  (0 without coordinates). The map is the location's map, `default` otherwise.
- `html` also writes `/games/<game>/docs/maps/world-<z>.html` (a negative level
  as `world-m5.html`).

The extent is the smallest rectangle holding every sector the level has in its
map index. It is drawn with the world map handler, with room beside the map for
the city names, and printed as the rows come, so a narrow terminal wraps them.

## The HTML page

A standalone page with one `<pre>`, monospace on a dark background, UTF-8. Each
run of glyphs with the same colour is a `<span>`; the mud's colour tokens
(`%^GREEN%^`, `%^ORANGE%^`, `%^BOLD%^`, ...) map to the ANSI palette and
`%^RESET%^` clears them. The text is HTML-escaped.

## Files

- `master.c` -- the package object (`package("fullmap")`).
- `src/fullmap.c` -- `query_bounds`, `render_level`, `to_html`, `write_html`.
- `cmds/admin/fullmap.c` -- the command, with its `.lang.<lang>.h` strings.
