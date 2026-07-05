
#include "/packages/test/src/test.h"

int called;
int private_called;

// needed to be added and removed from the livings handler
string query_name() { return "something"; }

int do_foo(string str)
{
  called = 1;
  return 1;
}

int do_bar(string str)
{
  called = 2;
  return 1;
}

int do_private(string str)
{
  private_called = 1;
  return 1;
}

void do_tests()
{
  called = 0;
  ADD_LIVING("something");

  add_action("do_foo", "foo");
  command("foo");
  IT("add_action callback fires when matching command is issued",
     called == 1);

  called = 0;
  IT("query_action returns the registered func",
     query_action("foo") == "do_foo");

  IT("query_action returns nil for an unregistered verb",
     query_action("never_registered") == nil);

  add_action("do_foo", ({ "one", "two" }));
  command("one");
  IT("add_action accepts an array of verbs — first entry wires", called == 1);
  called = 0;
  command("two");
  IT("add_action accepts an array of verbs — second entry wires", called == 1);

  called = 0;
  add_action("do_bar", "foo");
  command("foo");
  IT("re-adding the same verb overwrites the previous binding",
     called == 2);

  {
    string * verbs;
    verbs = map_indices(query_actions());
    IT("query_actions exposes every registered verb",
       member_array("foo", verbs) != -1 &&
       member_array("one", verbs) != -1 &&
       member_array("two", verbs) != -1);
  }

  // Private actions gate on this_object() == this_player(). Simulating
  // that here would require forging an interactive user; instead we
  // just verify the registration path does not error and the verb is
  // NOT visible to callers where this_object() != this_player() —
  // that's the whole reason the private variant exists.
  add_private_action("do_private", "shhh");
  IT("add_private_action doesn't leak into the public action list for outside callers",
     query_action("shhh") == nil);

  REMOVE_LIVING();
}
