
#include "/packages/test/src/test.h"

void do_tests()
{
  int i, j;

  TEST("allocate_int(N) returns an array of size N for 0 <= N < 10");
    for (i = 0; i < 10; i++)
      ASSERT(sizeof(allocate_int(i)) == i);
  END_TEST();

  TEST("allocate_int(N) initialises every element to 0");
    for (i = 0; i < 10; i++)
    {
      int * arr;
      arr = allocate_int(i);
      for (j = 0; j < i; j++)
        ASSERT(arr[j] == 0);
    }
  END_TEST();

  IT("allocate_int(-10) raises an error",
     catch(allocate_int(-10)));
}
