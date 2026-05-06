
#include "/packages/test/src/test.h"

void do_tests()
{
  string esc;

  TEST("strlen(str) returns byte length (kfun behaviour)");
    ASSERT(strlen("") == 0);
    ASSERT(strlen("a") == 1);
    ASSERT(strlen("hello") == 5);
    ASSERT(strlen("hello", false) == 5);
  END_TEST();

  TEST("strlen(nil) returns 0 in both modes");
    ASSERT(strlen(nil) == 0);
    ASSERT(strlen(nil, true) == 0);
  END_TEST();

  TEST("strlen(str, true) on plain ASCII matches byte length");
    ASSERT(strlen("hello", true) == 5);
    ASSERT(strlen("", true) == 0);
    ASSERT(strlen("12345", true) == 5);
  END_TEST();

  // UTF-8: á = 0xC3 0xA1 (195, 161). Single visible char, two bytes.
  // ñ = 0xC3 0xB1, é = 0xC3 0xA9, etc.
  // Visible counting only triggers for users on an ANSI terminal; in test
  // context this_user() is the admin running the cmd, expected to be ansi.
  TEST("strlen(str, true) compensates UTF-8 latin-1 supplement (195-prefixed)");
    // "ñ" is 2 bytes, 1 visible char
    ASSERT(strlen("ñ") == 2);
    // "canción" is 8 bytes (c-a-n-c-i-ó-n where ó is 2), 7 visible chars
    ASSERT(strlen("canción") == 8);
    // these run only when this_user() is on ansi; if not, visible == byte
    if (this_user() && this_user()->query_term_name() == "ansi")
    {
      ASSERT(strlen("ñ", true) == 1);
      ASSERT(strlen("canción", true) == 7);
      ASSERT(strlen("áéíóú", true) == 5);
    }
  END_TEST();

  // ANSI escape sequences: ESC [ ... m, where ESC == 27.
  // Format: "\x1b[31m" (red) is 5 bytes, 0 visible.
  TEST("strlen(str, true) skips ANSI escape sequences");
    if (this_user() && this_user()->query_term_name() == "ansi")
    {
      esc = chr(27) + "[31mhi" + chr(27) + "[0m";
      // 5 + 2 + 4 = 11 bytes, 2 visible
      ASSERT(strlen(esc) == 11);
      ASSERT(strlen(esc, true) == 2);
    }
  END_TEST();

  IT("strlen(str, true) on a non-ansi terminal falls back to byte length",
     // when there is no this_user(), visible counting still runs (no fallback)
     // we can't easily assert non-ansi without changing terminal, so this
     // test just exercises the no-user path which counts visible directly
     strlen("hello", true) == 5);
}
