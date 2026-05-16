
#include "/packages/test/src/test.h"

// Battery for the /lib/handlers/frames.c renderer. Each style is exercised
// against the literal text "Hello World"; the simple style is checked
// with exact strings (pure ASCII), the unicode-glyph styles are checked
// structurally so the tests do not depend on the editor encoding.

#define HW         "Hello World"
#define HW_WIDTH   11           // strlen("Hello World", true)
#define FRAMES     "/lib/handlers/frames"

// UTF-8 byte sequences for the glyphs the default style uses, built with
// chr() so the source stays pure-ASCII (matches the frames.c convention).
#define G_TL       (chr(226) + chr(148) + chr(140))   // U+250C ┌
#define G_TR       (chr(226) + chr(148) + chr(144))   // U+2510 ┐
#define G_BL       (chr(226) + chr(148) + chr(148))   // U+2514 └
#define G_BR       (chr(226) + chr(148) + chr(152))   // U+2518 ┘
#define G_VBAR     (chr(226) + chr(148) + chr(130))   // U+2502 │
#define G_HBAR     (chr(226) + chr(148) + chr(128))   // U+2500 ─

private string rep(string s, int n)
{
  string out;
  out = "";
  while (n-- > 0) out += s;
  return out;
}

// Returns 1 iff `needle` appears anywhere inside `haystack`.
private int contains(string haystack, string needle)
{
  return sizeof(explode(haystack, needle)) > 1;
}

// Split frame output into rows. `frame()` always terminates each row with
// a "\n" — DGD's explode drops the trailing empty fragment, so we get
// exactly N rows for N output lines.
private string * rows(string out) { return explode(out, "\n"); }

void do_tests()
{
  string out;
  string * lines;
  object f;

  f = load_object(FRAMES);

  // ============================================================
  // simple style — exact-string assertions (pure ASCII)
  // ============================================================

  // Expected (5 lines, each 19 chars, leading and trailing space margins):
  //
  //      +---------------+
  //      |               |
  //      |  Hello World  |
  //      |               |
  //      +---------------+
  //
  TEST("simple: 'Hello World' renders 5 framed lines");
    lines = rows(f->frame(HW, "", 0, 0, "simple"));
    // top edge + top padding + body + bottom padding + bottom edge
    ASSERT(sizeof(lines) == 5);
    // top edge: 1 margin + "+" + 15 dashes + "+" + 1 margin
    ASSERT(lines[0] == " +" + rep("-", 15) + "+ ");
    // padding row: 1 margin + "|" + 15 spaces + "|" + 1 margin
    ASSERT(lines[1] == " |" + rep(" ", 15) + "| ");
    // body row
    ASSERT(lines[2] == " |  " + HW + "  | ");
    // bottom padding == top padding
    ASSERT(lines[3] == lines[1]);
    // bottom edge == top edge
    ASSERT(lines[4] == lines[0]);
  END_TEST();

  // Expected (title "Title" injected near the left of the top edge):
  //
  //      +-| Title |-----+
  //      |               |
  //      |  Hello World  |
  //      |               |
  //      +---------------+
  //
  TEST("simple: title is injected in the top edge");
    lines = rows(f->frame(HW, "Title", 0, 0, "simple"));
    // top edge: " " + "+" + "-" + "| Title |" + "-"*(15-9-1) + "+" + " "
    ASSERT(lines[0] == " +-| Title |-----+ ");
    // body row unchanged
    ASSERT(lines[2] == " |  " + HW + "  | ");
  END_TEST();

  // Expected (content_width forced to 20; "Hello World" is right-padded
  // with 9 spaces inside the body):
  //
  //      +------------------------+
  //      |                        |
  //      |  Hello World           |
  //      |                        |
  //      +------------------------+
  //
  TEST("simple: explicit width overrides content length");
    lines = rows(f->frame(HW, "", 20, 0, "simple"));
    // body_inner_width = 20 + 4 = 24
    ASSERT(lines[0] == " +" + rep("-", 24) + "+ ");
    // body row: pad "Hello World" (11 chars) on the right to width 20
    ASSERT(lines[2] == " |  " + HW + rep(" ", 9) + "  | ");
  END_TEST();

  // Expected (two body rows; width is the longest line, "Hello World"):
  //
  //      +---------------+
  //      |               |
  //      |  Hello World  |
  //      |  Goodbye      |
  //      |               |
  //      +---------------+
  //
  TEST("simple: multi-line content produces one body row per line");
    lines = rows(f->frame(HW + "\nGoodbye", "", 0, 0, "simple"));
    // top + pad + 2 body rows + pad + bottom = 6
    ASSERT(sizeof(lines) == 6);
    // width is the longest line: "Hello World" (11)
    ASSERT(lines[2] == " |  Hello World  | ");
    ASSERT(lines[3] == " |  Goodbye      | ");
  END_TEST();

  // ============================================================
  // default style — unicode single-line glyphs
  // ============================================================

  // Expected (single-line box-drawing glyphs; same shape as `simple`
  // but with proper corners):
  //
  //      ┌───────────────┐
  //      │               │
  //      │  Hello World  │
  //      │               │
  //      └───────────────┘
  //
  TEST("default: corners use the expected box-drawing glyphs");
    lines = rows(f->frame(HW, "", 0, 0, nil));
    ASSERT(sizeof(lines) == 5);
    ASSERT(lines[0] == " " + G_TL + rep(G_HBAR, HW_WIDTH + 4) + G_TR + " ");
    ASSERT(lines[2] == " " + G_VBAR + "  " + HW + "  " + G_VBAR + " ");
    ASSERT(lines[4] == " " + G_BL + rep(G_HBAR, HW_WIDTH + 4) + G_BR + " ");
  END_TEST();

  // ============================================================
  // alert / notifications / sidebar — structural checks
  // ============================================================

  // Expected (double-line glyphs, padding_x = 3):
  //
  //      ╒═════════════════╕
  //      ║                 ║
  //      ║   Hello World   ║
  //      ║                 ║
  //      ╘═════════════════╛
  //
  TEST("alert: emits 5 framed lines with 3-wide padding");
    lines = rows(f->frame(HW, "", 0, 0, "alert"));
    ASSERT(sizeof(lines) == 5);
    // body row uses padding_x = 3
    ASSERT(contains(lines[2], "   " + HW + "   "));
  END_TEST();

  // Expected (same as alert but the bottom-left corner is a descending
  // pennant "╘═╕/" giving the ribbon a tail):
  //
  //      ╒═════════════════╕
  //      ║                 ║
  //      ║   Hello World   ║
  //      ║                 ║
  //      ╘═╕/══════════════╛
  //
  TEST("notifications: bottom edge carries the descending pennant");
    lines = rows(f->frame(HW, "", 0, 0, "notifications"));
    // pennant tail uses '/' as the last char of the bottom-left segment
    ASSERT(contains(lines[4], "/"));
  END_TEST();

  // Expected (heavy double-line glyphs, padding_x = 1, padding_y = 0 →
  // no inner blank rows):
  //
  //      ╔═════════════╗
  //      ║ Hello World ║
  //      ╚═════════════╝
  //
  TEST("sidebar: padding_y = 0 collapses the inner padding rows");
    lines = rows(f->frame(HW, "", 0, 0, "sidebar"));
    // top + body + bottom = 3
    ASSERT(sizeof(lines) == 3);
  END_TEST();

  // ============================================================
  // scroll / parchment style — multi-row top and bottom
  // ============================================================

  // Expected (parchment / pergamino: 3 top rows with diagonal offset,
  // 1 body row with padding_x = 8, 2 bottom rows widening to the right):
  //
  //         _____________________________
  //        /\                            \
  //        \_|                            |
  //          |        Hello World         |
  //          |   _________________________|___
  //           \_/____________________________/
  //
  TEST("scroll: 3 top rows + 1 body + 2 bottom rows, padding_x = 8");
    lines = rows(f->frame(HW, "", 0, 0, "scroll"));
    ASSERT(sizeof(lines) == 6);
    // distinctive opening glyphs of top rows 2 and 3
    ASSERT(contains(lines[1], "/\\"));
    ASSERT(contains(lines[2], "\\_|"));
    // body row carries the content with padding_x = 8 spaces on each side
    ASSERT(contains(lines[3], rep(" ", 8) + HW + rep(" ", 8)));
    // bottom row 2 ends with the closing '/'
    ASSERT(lines[5][strlen(lines[5]) - 1] == '/');
  END_TEST();

  TEST("scroll: top row 3 right border aligns with body right border");
    lines = rows(f->frame(HW, "", 0, 0, "scroll"));
    // both rows must have the same total width so the rightmost '|' ends
    // up in the same column
    ASSERT(strlen(lines[2]) == strlen(lines[3]));
  END_TEST();

  TEST("scroll: bottom row 1 '|' aligns with body right border");
    lines = rows(f->frame(HW, "", 0, 0, "scroll"));
    // body row ends with '|'; the '|' inside "|___" of bottom row 1 must
    // sit at the very same column (i.e. at strlen(body) - 1)
    ASSERT(lines[3][strlen(lines[3]) - 1] == '|');
    ASSERT(lines[4][strlen(lines[3]) - 1] == '|');
  END_TEST();

  // ============================================================
  // unknown style → return content unchanged
  // ============================================================

  IT("unknown style name returns the content unchanged",
     f->frame(HW, "", 0, 0, "no-such-style") == HW);
}
