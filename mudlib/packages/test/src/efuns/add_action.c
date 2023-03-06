
#include "/packages/test/src/test.h"

int called;

// needed to be added and removed from the livings handler
string query_name() { return "something"; }

int do_foo(string str)
{
  called = 1;
  return 1;
}

void do_tests()
{
  object tp;
  called = 0;

  // command() do not work if the object is not living
  ADD_LIVING("something");

  add_action("do_foo" , "foo");
  command("foo");
  
  ASSERT(called == 1);

  REMOVE_LIVING();
}
