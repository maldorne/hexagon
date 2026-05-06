
// strlen - length of a string
// int strlen(string str);
// int strlen(string str, int visible);
//
// Masks the kfun strlen. Without flag (or with `false`), returns the
// number of bytes in the string -- exactly what the kfun does, reached
// here via the `::` scope operator.
//
// With `visible == true`, returns the on-screen width of the string,
// skipping ANSI control sequences and compensating for UTF-8 multibyte
// characters so that one user-perceived character counts as one column.
//
// What the visible counter handles
// --------------------------------
//
// 1. ANSI CSI escape sequences: `ESC '[' parameters... final`, where
//    the final byte is in the range 0x40..0x7E (per ECMA-48). This
//    covers SGR (`\x1b[31m`, `\x1b[0m`), cursor motion (`\x1b[H`,
//    `\x1b[2J`), and any other CSI-shaped escape. The whole sequence
//    counts as zero columns. A bare ESC (not followed by '[') is
//    treated as zero columns; the rest of the string keeps being
//    counted normally.
//
// 2. UTF-8 multibyte characters, classified by the lead byte:
//      0xC2..0xDF -> 2-byte sequence, 1 visible column
//      0xE0..0xEF -> 3-byte sequence, 1 visible column
//      0xF0..0xF7 -> 4-byte sequence, 1 visible column
//    Trail bytes (0x80..0xBF) appearing without a lead are skipped
//    silently as malformed input. This is broader than the previous
//    visible_strlen, which only special-cased 0xC3 (Latin-1 Supplement)
//    and 0xE2 (box drawing/symbols). Now any UTF-8 codepoint counts
//    correctly: greek/cyrillic/cjk/emoji-on-the-BMP/etc.
//
// 3. ASCII printable (0x20..0x7E) and the 8-bit raw range left over
//    after UTF-8 classification: 1 column each.
//
// 4. ASCII control characters below 0x20 (except ESC, handled above):
//    counted as 1 column to preserve the previous behaviour. This is
//    a deliberate compatibility choice -- the table-mode of sprintf
//    splits on '\n', and the legacy `visible_strlen` counted '\n' as
//    1 too. If we ever want stricter accounting (`\b` -> 0 or even
//    -1, `\t` -> width-to-next-tabstop, etc.) it goes here.
//
// What still isn't covered
// ------------------------
//
// - Wide CJK / Hangul / fullwidth forms that occupy two terminal cells
//   even though they are a single Unicode codepoint. Doing this right
//   requires a width table (see Unicode East Asian Width). Out of
//   scope; nobody in Hexagon uses fullwidth output today.
//
// - Combining marks and zero-width joiners. Each codepoint counts as
//   one column even when it visually combines with the previous
//   character (e.g. `e` + U+0301 -> `é` should be 1 column, we give 2).
//
// - Non-CSI escape sequences: OSC (`ESC ']' ... ST`), DCS, SOS, PM,
//   APC, single-character escapes other than CSI. We treat the bare
//   ESC as zero columns and resume counting on the next byte, which
//   may overcount for those sequences. None of them are produced by
//   the mudlib today.
//
// - Bidi controls, variation selectors, etc.
//
// Terminal fallback
// -----------------
//
// If `this_user()` exists and reports a non-`ansi` terminal name, the
// visible mode collapses to byte length. Rationale: a non-ANSI client
// won't render the escape sequences, but our output also won't include
// them in that case (the colour layer is supposed to strip them
// upstream), so the visible width effectively equals the byte width.

static int strlen(string str, varargs int visible)
{
  int i, length, result, b;

  if (!str)
    return 0;

  length = ::strlen(str);

  if (!visible)
    return length;

  if (this_user() && this_user()->query_term_name() != "ansi")
    return length;

  result = 0;

  for (i = 0; i < length; i++)
  {
    b = str[i];

    // ANSI escape: try to consume a CSI sequence ESC '[' ... final
    if (b == 27)
    {
      if (i + 1 < length && str[i + 1] == '[')
      {
        i += 2;  // past ESC and '['
        while (i < length && (str[i] < 0x40 || str[i] > 0x7E))
          i++;
        // i now points at the final byte; the for-loop's i++ steps past it
      }
      // bare ESC or unrecognised escape: just skip the ESC byte and
      // resume counting on the next iteration.
      continue;
    }

    // UTF-8 multibyte leads -- count one visible column, advance over
    // the trail bytes (the for-loop adds the final +1).
    if (b >= 0xC2 && b <= 0xDF)        // 2-byte sequence
    {
      i += 1;
      result++;
      continue;
    }
    if (b >= 0xE0 && b <= 0xEF)        // 3-byte sequence
    {
      i += 2;
      result++;
      continue;
    }
    if (b >= 0xF0 && b <= 0xF7)        // 4-byte sequence
    {
      i += 3;
      result++;
      continue;
    }

    // Stray UTF-8 trail byte without a lead: skip silently.
    if (b >= 0x80 && b <= 0xBF)
      continue;

    // Everything else: ASCII printable, ASCII control (not ESC), or
    // raw 8-bit -- count as one column.
    result++;
  }

  return result;
}
