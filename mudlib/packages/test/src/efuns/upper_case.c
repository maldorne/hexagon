
#include "/packages/test/src/test.h"

void do_tests()
{
  TEST("upper_case(str) raises every character, not only the first");
    ASSERT(upper_case("hello") == "HELLO");
    ASSERT(upper_case("CoBaRdIa") == "COBARDIA");
    ASSERT(upper_case("HELLO") == "HELLO");
  END_TEST();

  TEST("upper_case(str) leaves what is not a letter alone");
    ASSERT(upper_case("") == "");
    ASSERT(upper_case("a1-b2 c3") == "A1-B2 C3");
    ASSERT(upper_case("123") == "123");
  END_TEST();

  TEST("upper_case(str) raises accented letters wherever they are");
    ASSERT(upper_case("áéíóú") == "ÁÉÍÓÚ");
    ASSERT(upper_case("canción") == "CANCIÓN");
    ASSERT(upper_case("mañana") == "MAÑANA");
    ASSERT(upper_case("pingüino") == "PINGÜINO");
  END_TEST();

  TEST("upper_case(str) and lower_case(str) undo each other");
    ASSERT(lower_case(upper_case("canción")) == "canción");
    ASSERT(upper_case(lower_case("MAÑANA")) == "MAÑANA");
  END_TEST();

  // Only ASCII and the latin-1 supplement convert. Letters of other blocks
  // -- latin extended-A, greek, cyrillic -- are left as they are.
  TEST("upper_case(str) leaves alphabets outside latin-1 alone");
    ASSERT(upper_case("łśżćę") == "łśżćę");
    ASSERT(upper_case("αβγ") == "αβγ");
  END_TEST();

  IT("upper_case(str) is idempotent",
     upper_case(upper_case("árbol")) == upper_case("árbol"));
}
