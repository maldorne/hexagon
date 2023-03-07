
#include "/packages/test/src/test.h"

void do_tests()
{
  int s, i;
  s = sizeof(all_inventory(this_object()));
  
  ASSERT(sizeof(all_inventory()) == s);
  
  for (i = 0; i < 5; i++)
  {
    ASSERT(sizeof(all_inventory()) == s + i);
    clone_object(file_name());
  }

  ASSERT(filter(all_inventory(this_object()), "is_all_inv_test", this_object()));
}

int is_all_inv_test(mixed something, varargs mixed args...)
{
  return 1;
}

void create()
{
  if (clonep()) 
  {
    string s;
    s = file_name();
    sscanf(s, "%s#%*d", s);    
    move(find_object(s));
  }
}

    
