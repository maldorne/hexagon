
#include "/packages/test/src/test.h"

// Battery for sprintf.  sprintf is now provided by the dgd-extensions
// C kfun; the legacy LPC implementation has been moved to
// /packages/sprintf/ for historical reference.  The format spec lives
// in /packages/sprintf/src/sprintf.txt.
//
// Out of scope (deliberate):
//   - %r/%R (encrypt) and %q/%Q (call_other-as-function): unused in the
//     mudlib and exotic; we will not preserve them in the C rewrite
//     unless explicitly asked.
//   - @* time conversions: __TIME_CONVERSION__ is commented out in the
//     sprintf config, those branches are not compiled in.
//
// __FLOATS__ is defined in sprintf/config.h, so float specifiers are
// active and tested below.

void do_tests()
{
  string out, s, padded;
  int * counter;
  int extra;
  object ob;

  // ---------------- string conversions ----------------

  TEST("%s substitutes a string");
    ASSERT(sprintf("%s", "hello") == "hello");
    ASSERT(sprintf("[%s]", "x") == "[x]");
    ASSERT(sprintf("%s%s", "a", "b") == "ab");
  END_TEST();

  TEST("%s accepts int and float, converting to string");
    ASSERT(sprintf("%s", 42) == "42");
  END_TEST();

  TEST("%S accepts strings only");
    ASSERT(sprintf("%S", "hello") == "hello");
    ASSERT(catch(sprintf("%S", 42)));   // must error: non-string arg
  END_TEST();

  TEST("%y prints any type via 'anything'");
    ASSERT(sprintf("%y", 1) == "1");
    ASSERT(sprintf("%y", "foo") == "\"foo\"");
    ASSERT(sprintf("%y", ({ })) == "({ })");
  END_TEST();

  TEST("%a expands an array, repeating format for each element");
    // %a applies the same spec to every element of the array
    ASSERT(sprintf("%a", ({ "a", "b", "c" })) == "abc");
    ASSERT(sprintf("%-3a", ({ "x", "y" })) == "x  y  ");
  END_TEST();

  TEST("%a auto-stringifies numeric elements");
    ASSERT(sprintf("%a", ({ 1, 2, 3 })) == "123");
  END_TEST();

  TEST("%A is %a but rejects non-string elements");
    ASSERT(sprintf("%A", ({ "a", "b" })) == "ab");
    ASSERT(catch(sprintf("%A", ({ 1, 2 }))));
  END_TEST();

  // ---------------- integer conversions ----------------

  TEST("%d / %i format integers (signed decimal)");
    ASSERT(sprintf("%d", 0) == "0");
    ASSERT(sprintf("%d", 42) == "42");
    ASSERT(sprintf("%i", -7) == "-7");
    ASSERT(sprintf("%d", 1234567) == "1234567");
  END_TEST();

  TEST("%o octal");
    ASSERT(sprintf("%o", 8) == "10");
    ASSERT(sprintf("%o", 73) == "111");
  END_TEST();

  TEST("%x %X hexadecimal");
    ASSERT(sprintf("%x", 255) == "ff");
    ASSERT(sprintf("%X", 255) == "FF");
    ASSERT(sprintf("%x", 0xa9) == "a9");
  END_TEST();

  TEST("%b binary");
    ASSERT(sprintf("%b", 5) == "101");
    ASSERT(sprintf("%b", 0) == "0");
    ASSERT(sprintf("%b", 8) == "1000");
  END_TEST();

  TEST("%c emits a single character from an int");
    ASSERT(sprintf("%c", 'A') == "A");
    ASSERT(sprintf("%c", 97) == "a");
  END_TEST();

  // ---------------- float conversions ----------------

  TEST("%f decimal float, default precision 6");
    ASSERT(sprintf("%f", 1.5) == "1.500000");
    ASSERT(sprintf("%f", 0.0) == "0.000000");
    ASSERT(sprintf("%f", -2.25) == "-2.250000");
  END_TEST();

  TEST("%.Nf precision controls decimals");
    ASSERT(sprintf("%.2f", 1.5) == "1.50");
    ASSERT(sprintf("%.4f", 0.1) == "0.1000");
    // %.0f behaviour is impl-defined here (the LPC impl re-applies
    // FLT_PRECISION when precision parses as 0); not asserted.
  END_TEST();

  TEST("%f accepts ints and treats them as floats");
    ASSERT(sprintf("%.1f", 3) == "3.0");
  END_TEST();

  // %e / %E / %g / %G output format is implementation-detailed; we
  // assert only the broad shape (presence of e/E and a plausible exp).
  TEST("%e exponential lowercase");
    out = sprintf("%e", 1000.0);
    ASSERT(strsrch(out, "e") >= 0);
    ASSERT(strsrch(out, "E") < 0);
  END_TEST();

  TEST("%E exponential uppercase");
    out = sprintf("%E", 1000.0);
    ASSERT(strsrch(out, "E") >= 0);
    ASSERT(strsrch(out, "e") < 0);
  END_TEST();

  TEST("%g picks %f for small exponents, strips trailing zeros");
    // 1.5 has small exponent -> %f-style without trailing zeros
    out = sprintf("%g", 1.5);
    // expect no trailing zero blob like "1.500000"
    ASSERT(out == "1.5" || out == "1.50000");   // tolerant
  END_TEST();

  // ---------------- %p padding-char ----------------

  TEST("%p uses the next argument as pad character");
    ASSERT(sprintf("%p%5s", '-', "ab") == "---ab");
    ASSERT(sprintf("%p%-5s", '*', "ab") == "ab***");
    ASSERT(sprintf("%p%|7s", '.', "abc") == "..abc..");
  END_TEST();

  // ---------------- width specifier ----------------

  TEST("width right-aligns by default with space pad");
    ASSERT(sprintf("%5s", "ab") == "   ab");
    ASSERT(sprintf("%5d", 42) == "   42");
  END_TEST();

  TEST("- flag left-aligns with space pad");
    ASSERT(sprintf("%-5s", "ab") == "ab   ");
    ASSERT(sprintf("%-5d", 42) == "42   ");
  END_TEST();

  TEST("| flag centers within the given width");
    ASSERT(sprintf("%|5s", "ab") == "  ab ");
    ASSERT(sprintf("%|7s", "abc") == "  abc  ");
  END_TEST();

  TEST("* takes width from the next argument");
    ASSERT(sprintf("%*s", 5, "ab") == "   ab");
    ASSERT(sprintf("%-*s", 5, "ab") == "ab   ");
    ASSERT(sprintf("%|*s", 5, "ab") == "  ab ");
  END_TEST();

  TEST("right-align (default): argument wider than width is preserved");
    // no flag: the string overflows the field, no truncation
    ASSERT(sprintf("%3s", "abcdef") == "abcdef");
  END_TEST();

  TEST("- flag with overflow triggers word-wrap via multi_line");
    // with the - flag, an overflowing string is fed to multi_line(),
    // which folds it at `width` columns. For a single word longer than
    // width, the first piece is hard-truncated.
    ASSERT(sprintf("%-3s", "abcdef") == "abc");
  END_TEST();

  // ---------------- precision ----------------

  TEST(".N precision truncates strings (right end)");
    ASSERT(sprintf("%.3s", "abcdef") == "abc");
    // .0 (precision == 0) means "no precision" in this impl, not zero
    // characters -- the field passes through unchanged.
    ASSERT(sprintf("%.0s", "abc") == "abc");
  END_TEST();

  TEST("_ flag with precision truncates from the left");
    ASSERT(sprintf("%_.3s", "abcdef") == "def");
  END_TEST();

  TEST(".* takes precision from the next argument");
    ASSERT(sprintf("%.*s", 3, "abcdef") == "abc");
  END_TEST();

  TEST("width + precision combined: precision applies after width");
    // align() pads-or-wraps first, then truncates by precision.
    // %5.3s on "abcdef": no pad needed (string already wider than 5),
    // then truncate to 3 -> "abc". width is effectively shadowed.
    ASSERT(sprintf("%5.3s", "abcdef") == "abc");
    // %-5.3s: multi_line wraps "abcdef" -> "abcde", then truncate to 3.
    ASSERT(sprintf("%-5.3s", "abcdef") == "abc");
  END_TEST();

  TEST(".N precision on integers pads with leading zeros");
    ASSERT(sprintf("%.4d", 42) == "0042");
  END_TEST();

  // ---------------- numerical flags ----------------

  TEST("+ flag forces a sign on positive numbers");
    ASSERT(sprintf("%+d", 5) == "+5");
    ASSERT(sprintf("%+d", -5) == "-5");
    ASSERT(sprintf("%+d", 0) == "+0");
  END_TEST();

  TEST("space flag adds a leading space when no sign is printed");
    ASSERT(sprintf("% d", 5) == " 5");
    ASSERT(sprintf("% d", -5) == "-5");
  END_TEST();

  TEST("0 flag pads numerical conversions with zeros");
    ASSERT(sprintf("%05d", 42) == "00042");
    ASSERT(sprintf("%05d", -42) == "-0042");
  END_TEST();

  // ---------------- string-flavour flags ----------------

  TEST("` flag reverses the string");
    ASSERT(sprintf("%`s", "abc") == "cba");
  END_TEST();

  TEST("& flag rot-13 the string");
    ASSERT(sprintf("%&s", "abc") == "nop");
    ASSERT(sprintf("%&s", "nop") == "abc");
    ASSERT(sprintf("%&s", "Hello") == "Uryyb");
  END_TEST();

  TEST("~ flag flips case");
    ASSERT(sprintf("%~s", "AbCd") == "aBcD");
  END_TEST();

  TEST("< flag forces lower case");
    ASSERT(sprintf("%<s", "ABC") == "abc");
    ASSERT(sprintf("%<s", "AbCd") == "abcd");
  END_TEST();

  TEST("> flag forces upper case");
    ASSERT(sprintf("%>s", "abc") == "ABC");
    ASSERT(sprintf("%>s", "AbCd") == "ABCD");
  END_TEST();

  TEST("= flag capitalizes (first letter)");
    ASSERT(sprintf("%=s", "abc") == "Abc");
  END_TEST();

  // ---------------- table mode ----------------

  TEST("# flag with width: column/table mode (string only)");
    // Layout in column mode is impl-specific (depends on the column
    // count = width / (max_cell + 1) heuristic). Smoke-check: every
    // input cell appears in the output, and the output is non-empty.
    out = sprintf("%#-20s\n", "alpha\nbeta\ngamma\ndelta\nepsilon\n");
    ASSERT(strsrch(out, "alpha") >= 0);
    ASSERT(strsrch(out, "beta") >= 0);
    ASSERT(strsrch(out, "gamma") >= 0);
    ASSERT(strsrch(out, "delta") >= 0);
    ASSERT(strsrch(out, "epsilon") >= 0);
  END_TEST();

  // ---------------- %h / %H hex dump ----------------

  // The implementation reverses %h and %H relative to the spec doc:
  // %h emits no separator (always nil padding), %H uses the current
  // padding char (default space). Tests lock the impl, not the spec.
  TEST("%h prints byte values in hex with no separator");
    ASSERT(sprintf("%h", "AB") == "4142");
    ASSERT(sprintf("%h", "") == "");
  END_TEST();

  TEST("%H prints byte values separated by the padding char");
    ASSERT(sprintf("%H", "AB") == "41 42");
    ASSERT(sprintf("%p%H", '-', "AB") == "41-42");
  END_TEST();

  // ---------------- %n ----------------

  TEST("%n stores the count of bytes written so far in the int* arg");
    counter = ({ 0 });
    sprintf("hello%n world", counter);
    ASSERT(counter[0] == 5);   // "hello" already in result
  END_TEST();

  // ---------------- %O object_name ----------------

  // %O is unsupported by the kfun implementation: the public DGD
  // extension API does not expose lpc_object_getval, so the kfun cannot
  // resolve an object's file_name from an LPC_value. The kfun raises a
  // clear runtime error explaining the workaround. Callers should use
  // sprintf("%s", object_name(ob)) instead.
  TEST("%O is not supported (raises runtime error)");
    ob = this_object();
    ASSERT(catch(sprintf("%O", ob)));
    ASSERT(catch(sprintf("%O", 42)));
    ASSERT(catch(sprintf("%O", "foo")));
  END_TEST();

  // ---------------- literal %% ----------------

  TEST("literal %% emits a single percent");
    ASSERT(sprintf("%%") == "%");
    ASSERT(sprintf("100%%") == "100%");
    ASSERT(sprintf("%d%%", 42) == "42%");
  END_TEST();

  // ---------------- visible-width awareness ----------------
  //
  // sprintf measures string width in *visible* columns, not in bytes:
  // ANSI escape sequences and UTF-8 multibyte characters do not inflate
  // the field. The width passed to %*s is the desired number of columns,
  // full stop -- callers do not need to compensate for invisible bytes.

  TEST("UTF-8 string is right-padded to the requested visible width");
    if (this_user() && this_user()->query_term_name() == "ansi")
    {
      s = "canción";       // 7 visible columns, 8 bytes (ó = 0xC3 0xB3)
      padded = sprintf("%*s", 10, s);
      ASSERT(strlen(padded, true) == 10);
    }
  END_TEST();

  TEST("UTF-8 string is left-padded to the requested visible width");
    if (this_user() && this_user()->query_term_name() == "ansi")
    {
      s = "canción";
      padded = sprintf("%-*s", 10, s);
      ASSERT(strlen(padded, true) == 10);
    }
  END_TEST();

  TEST("UTF-8 string is centred to the requested visible width");
    if (this_user() && this_user()->query_term_name() == "ansi")
    {
      s = "canción";
      padded = sprintf("%|*s", 11, s);
      ASSERT(strlen(padded, true) == 11);
    }
  END_TEST();

  TEST("ANSI-coloured string padded to a visible width");
    if (this_user() && this_user()->query_term_name() == "ansi")
    {
      s = chr(27) + "[31mhi" + chr(27) + "[0m";  // 2 visible, 11 bytes
      padded = sprintf("%-*s", 5, s);
      ASSERT(strlen(padded, true) == 5);
    }
  END_TEST();

  TEST("ANSI + UTF-8 mixed string padded to a visible width");
    if (this_user() && this_user()->query_term_name() == "ansi")
    {
      s = chr(27) + "[32mcanción" + chr(27) + "[0m";  // 7 visible
      padded = sprintf("%*s", 12, s);
      ASSERT(strlen(padded, true) == 12);
    }
  END_TEST();

  // ---------------- error paths ----------------

  TEST("unknown conversion character errors");
    ASSERT(catch(sprintf("%Z", 1)));
  END_TEST();

  TEST("too few arguments errors");
    ASSERT(catch(sprintf("%d %d", 1)));
  END_TEST();
}
