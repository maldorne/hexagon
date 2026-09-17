
#include "/packages/test/src/test.h"

void do_tests()
{
  TEST("capitalize(str) raises the first character and nothing else");
    ASSERT(capitalize("hello") == "Hello");
    ASSERT(capitalize("hello world") == "Hello world");
    ASSERT(capitalize("cobardia") == "Cobardia");
  END_TEST();

  TEST("capitalize(str) leaves a string it cannot raise alone");
    ASSERT(capitalize("") == "");
    ASSERT(capitalize("Hello") == "Hello");
    ASSERT(capitalize("123abc") == "123abc");
  END_TEST();

  TEST("capitalize(str) raises a leading accented letter");
    ASSERT(capitalize("árbol") == "Árbol");
    ASSERT(capitalize("ñu") == "Ñu");
    ASSERT(capitalize("único") == "Único");
  END_TEST();

  TEST("capitalize(str) does not touch accented letters further in");
    ASSERT(capitalize("canción") == "Canción");
    ASSERT(capitalize("mañana") == "Mañana");
  END_TEST();

  TEST("capitalize(str) knows the whole latin-1 block, not only spanish");
    ASSERT(capitalize("çedilla") == "Çedilla");
    ASSERT(capitalize("àbc") == "Àbc");
    ASSERT(capitalize("øre") == "Øre");
  END_TEST();

  IT("capitalize(str) is idempotent",
     capitalize(capitalize("árbol")) == capitalize("árbol"));
}
