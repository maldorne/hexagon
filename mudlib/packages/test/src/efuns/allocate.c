
#include "/packages/test/src/test.h"

void do_tests()
{
  int i, j;

  for (i = 0; i < 10; i++) 
  {
    mixed * arr;
    arr = allocate(i);
    
    ASSERT(sizeof(arr) == i);

    for (j = 0; j < i; j++)
      ASSERT(arr[j] == nil);
  }

  ASSERT(catch(allocate(-10)));
}
