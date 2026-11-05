# Terminal interface (`lib/user/ui.c`)

A status bar fixed on the top line (name, life and energy points, game hour)
and a sidebar on the right (map, points, place), drawn with ANSI/VT100 escape
sequences over plain telnet. No GMCP, MSDP or MXP.

## Settings

Account settings (saved with the user, like the terminal type), in the
`interface` category of the `configuration` command (Spanish `interfaz`;
`terminal` and `term` still work as names of the category):

| topic | what it does |
|---|---|
| type | terminal type; the interface needs `ansi`, `ansi-enye` or `ansi-status` |
| columns / rows | size of the screen; the driver does not negotiate NAWS, so they are typed by hand (rows 15-100) |
| status bar | on/off |
| sidebar | on/off; refused if the screen leaves less than 40 columns to the text |
| sidebar width | 10-40 columns, 26 by default; below 18 the points show bars without numbers, the map and the place are cropped to fit |
| sidebar map | `local` (the map around the player), `world` (a window of the world map centred on the player's sector, same glyphs as `worldmap`), `both` or `none`; `local` by default |
| map style | how the local map is drawn: `unicode` (box-drawing glyphs, as `mapa unicode`) or `compact` (as `mapa compacto`); `unicode` by default |
| reserved bottom lines | 0-5, default 0: lines at the bottom of the screen the interface never draws on nor scrolls over (see TinTin++) |

`sidebar <on|off>` and `statusbar <on|off>` (Spanish `lateral`, `barra`) are
shortcuts to the same settings. A terminal that is not ANSI refuses them with a
message and never receives a sequence; changing the terminal type to a non ANSI
one removes the interface from the screen.

## Sequences

- Save and restore cursor: `ESC 7` / `ESC 8`; every repaint is one string
  wrapped in them.
- Scroll region `ESC[3;<last row>r`, only with the status bar on (the sidebar
  alone never sets one): keeps the game text below the bar and its line.
  Reasserted in every repaint (it moves the cursor, hence the save/restore).
  `ESC[r` resets it.
- Absolute position `ESC[<row>;<col>H` for the status bar (row 1), the line
  under it (row 2) and each line of the sidebar (column `cols - width + 1`).
- Layout, with the status bar and the sidebar on: row 1 is text, row 2 a double
  line across the screen where the sidebar hangs (`\u2566` on its left edge,
  `\u2557` closing its right corner), the sidebar box has `\u2551` edges and a `\u255a\u2550\u255d`
  bottom on the last row. With the sidebar alone its box has its own top. No
  colour but the HP and GP bars.
- When a layout is turned on, each row of the interface is cleared (`ESC[<row>;1H` `ESC[2K`) and the cursor parked on the last one; the reserved
  bottom lines are not touched and a login clears nothing. There is no `ESC[2J`.
- Colours: no background on either bar; only the points' meters are coloured
  (SGR foreground), and colour tokens are converted by `/lib/handlers/terminal`
  for the sidebar.
- Turning off, changing size, or leaving: the scroll region (only set with the
  status bar) goes back to the whole screen, or to the rows above the reserved
  lines, the bar line is cleared and the sidebar columns overwritten with spaces.

Everything is drawn between row 1 and row `rows - reserved`; `rows` is the
number of rows of the whole terminal.

## Wrapping

`query_cols()` (`communicate.c`) answers `cols - width - 1` while the sidebar is
on, so everything that wraps for the user (`wrap` callers, frames, `more`) uses
the narrower width. `query_term_cols()` answers the real value. As a last
resort `catch_tell` (the one place all text for a user passes through) wraps any
line wider than that, in single and multi-line messages alike; a line that
already fits is left as it is.

## Repaint

`user.c:send_message` asks for a repaint after any output; `ui_schedule()`
coalesces them in one `call_out` (delay 0, so after the current command, and at
most one per second). The sidebar is repainted every time because the game
scrolls the screen over it; the status bar only when its text changed. The
heart beat repaints when points, place or game hour change. The interface's own
send uses the kfun, which is neither snooped nor repainted.

## Limits

- No NAWS: rows and columns come from the settings. Rows or columns larger
  than the real terminal garble the layout.
- A line sent straight to the connection without `catch_tell` (a few file
  pagers) can still run under the sidebar until the next output.
- The local map is the compact one, cropped to the panel, resident rooms only.
  The world map comes from `render_around` of the world map handler of the
  player's game, cached per room. Where there is no world map (a room without
  coordinates) it falls back to the local one. With `both` the world map gets a
  third of the rows left and is the first to go when the panel is short.
- Clients that ignore cursor addressing or scroll regions show the raw
  sequences or a plain stream; they should leave the options off.
- A resized client window needs `rows` / `columns` set again.

## TinTin++

TinTin++ keeps its own split: a scroll region plus a divider line and an input
line at the bottom (`#split`, on by default; both lines count as 2).

1. Run `stty size` in the shell that holds TinTin++: it prints rows and columns
   of the whole terminal.
2. `configuración interfaz filas <rows>` with exactly those rows, and `columnas`
   with the columns. A `filas` smaller than the terminal leaves blank lines
   between the game and the input line; a larger one draws past the bottom.
3. `configuración interfaz líneas reservadas abajo 2`, so the interface stays
   above the divider and the input line and does not overwrite them.
4. Then `barra on` and `lateral on`.

## Size of the screen

`filas` and `columnas` must be those of the real terminal (`stty size` prints
rows then columns). With the status bar, the scroll region ends at `filas`
(less the reserved lines): if the real terminal is taller, the game text only
reaches that row and the rest stays empty. The driver cannot ask the client its
size (no NAWS), and a cursor position report (`ESC[6n`) is not used: the reply
is typed input that arrives only with the next line the user sends, and a client
such as TinTin++ may keep it for itself.
