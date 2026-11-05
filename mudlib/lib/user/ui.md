# Terminal interface (`lib/user/ui.c`)

A status bar fixed on the top line and a sidebar on the right, drawn with
ANSI/VT100 escape sequences over plain telnet. No GMCP, MSDP or MXP.

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
| sidebar width | 20-40 columns, 26 by default |

`sidebar <on|off>` and `statusbar <on|off>` (Spanish `lateral`, `barra`) are
shortcuts to the same settings. A terminal that is not ANSI refuses them with a
message and never receives a sequence; changing the terminal type to a non ANSI
one removes the interface from the screen.

## Sequences

- Save and restore cursor: `ESC 7` / `ESC 8`; every repaint is one string
  wrapped in them.
- Scroll region `ESC[2;<rows>r`: keeps the game text below the status bar.
  Reasserted in every repaint (it moves the cursor, hence the save/restore).
  `ESC[r` resets it.
- Absolute position `ESC[<row>;<col>H` for the status bar (row 1) and for
  each line of the sidebar (column `cols - width + 1`).
- `ESC[2J` + `ESC[<rows>;1H` clear the screen and park the cursor on the last
  line when a layout is turned on, but not at login.
- Colours: SGR for the status bar (white on blue); colour tokens converted by
  `/lib/handlers/terminal` for the sidebar.
- Turning off, changing size, or leaving: the scroll region is reset, the bar
  line cleared and the sidebar columns overwritten with spaces.

## Wrapping

`query_cols()` (`communicate.c`) answers `cols - width - 1` while the sidebar is
on, so everything that wraps for the user (`catch_tell`, `wrap` callers, frames,
`more`) uses the narrower width. `query_term_cols()` answers the real value.

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
- Text already wider than the text column (a prepared multi-line message that
  does not wrap) is overwritten by the sidebar until the next output.
- The map is the compact one, cropped to the panel, resident rooms only.
- Clients that ignore cursor addressing or scroll regions show the raw
  sequences or a plain stream; they should leave the options off.
- A resized client window needs `rows` / `columns` set again.
