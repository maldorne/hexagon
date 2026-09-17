
#include "/packages/test/src/test.h"

// UTF-8: an accented latin letter is two bytes, 195 followed by the letter.
// "Á" is 195 129 and "á" is 195 161, so the lowercase byte sits 32 above
// the uppercase one.

void do_tests()
{
  TEST("lower_case(str) lowers every character, not only the first");
    ASSERT(lower_case("HELLO") == "hello");
    ASSERT(lower_case("CoBaRdIa") == "cobardia");
    ASSERT(lower_case("hello") == "hello");
  END_TEST();

  TEST("lower_case(str) leaves what is not a letter alone");
    ASSERT(lower_case("") == "");
    ASSERT(lower_case("A1-B2 C3") == "a1-b2 c3");
    ASSERT(lower_case("123") == "123");
  END_TEST();

  TEST("lower_case(str) lowers accented letters wherever they are");
    ASSERT(lower_case("ÁÉÍÓÚ") == "áéíóú");
    ASSERT(lower_case("CANCIÓN") == "canción");
    ASSERT(lower_case("MAÑANA") == "mañana");
    ASSERT(lower_case("PINGÜINO") == "pingüino");
  END_TEST();

  TEST("lower_case(str) keeps the byte length of accented letters");
    ASSERT(strlen(lower_case("ÑU")) == strlen("ÑU"));
    ASSERT(lower_case("Ñ") == "ñ");
  END_TEST();

  // Only ASCII and the latin-1 supplement convert. Letters of other blocks
  // -- latin extended-A, greek, cyrillic -- are left as they are.
  TEST("lower_case(str) leaves alphabets outside latin-1 alone");
    ASSERT(lower_case("ŁŚŻĆĘ") == "ŁŚŻĆĘ");
    ASSERT(lower_case("АБВ") == "АБВ");
  END_TEST();

  IT("lower_case(str) is idempotent",
     lower_case(lower_case("ÁRBOL")) == lower_case("ÁRBOL"));
}
