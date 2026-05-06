
#include "/packages/test/src/test.h"

void do_tests()
{
  int i, j;

  TEST("allocate(N) returns an array of size N for 0 <= N < 10");
    for (i = 0; i < 10; i++)
      ASSERT(sizeof(allocate(i)) == i);
  END_TEST();

  TEST("allocate(N) initialises every element to nil");
    for (i = 0; i < 10; i++)
    {
      mixed * arr;
      arr = allocate(i);
      for (j = 0; j < i; j++)
        ASSERT(arr[j] == nil);
    }
  END_TEST();

  IT("allocate(-10) raises an error",
     catch(allocate(-10)));
}
