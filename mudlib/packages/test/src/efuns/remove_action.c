
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
  called = 0;
  ADD_LIVING("something");

  add_action("do_foo", "foo");
  command("foo");
  IT("baseline: add_action wires the verb", called == 1);

  called = 0;
  IT("remove_action returns 1 when the matching binding existed",
     remove_action("do_foo", "foo") == 1);

  command("foo");
  IT("after remove_action the callback no longer fires", called == 0);

  IT("remove_action returns 0 when nothing to remove",
     remove_action("do_foo", "foo") == 0);

  add_action("do_foo", "bar");
  IT("remove_action returns 0 when the binding belongs to a different func",
     remove_action("other_func", "bar") == 0);
  IT("that mismatched binding stays live", query_action("bar") == "do_foo");
  remove_action("do_foo", "bar");

  add_action("do_foo", ({ "one", "two" }));
  IT("remove_action accepts an array of verbs",
     remove_action("do_foo", ({ "one", "two", "missing" })) == 1);
  IT("array removal cleared both matching verbs",
     query_action("one") == nil && query_action("two") == nil);

  REMOVE_LIVING();
}
