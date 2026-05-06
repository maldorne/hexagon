
#include "/packages/test/src/test.h"

void do_tests()
{
  int s, i;
  s = sizeof(all_inventory(this_object()));

  IT("all_inventory() == all_inventory(this_object()) when called with no args",
     sizeof(all_inventory()) == s);

  for (i = 0; i < 5; i++)
  {
    IT("inventory grows by 1 after each clone (iteration " + i + ")",
       sizeof(all_inventory()) == s + i);
    clone_object(file_name());
  }

  IT("all clones are matched by a custom filter callback",
     filter(all_inventory(this_object()),
            "is_all_inv_test", this_object()));
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

    
