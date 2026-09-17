
#include "/packages/test/src/test.h"

#define RELAY "/packages/test/src/efuns/.support/previous-object-relay"

mixed * probe()
{
  return ({ previous_object(), previous_object(0), previous_object(1),
            previous_object(2), previous_object(-1) });
}

// a local call does not step back in the chain
mixed * local_probe()
{
  return probe();
}

mixed * self_probe()
{
  return this_object()->probe();
}

void do_tests()
{
  object runner, relay;
  mixed * seen;

  runner = previous_object();
  relay = load_object(RELAY);

  TEST("previous_object() is the object that called with call_other");
    ASSERT(objectp(runner));
    ASSERT(runner != this_object());
  END_TEST();

  TEST("previous_object(n) steps back n call_others more");
    seen = relay->relay(this_object());
    ASSERT(seen[0] == relay);
    ASSERT(seen[1] == relay);
    ASSERT(seen[2] == this_object());
    ASSERT(seen[3] == runner);
  END_TEST();

  TEST("previous_object(-1) lists the chain nearest first");
    seen = relay->relay(this_object());
    ASSERT(sizeof(seen[4]) >= 3);
    ASSERT(seen[4][0] == relay);
    ASSERT(seen[4][1] == this_object());
    ASSERT(seen[4][2] == runner);
  END_TEST();

  TEST("local calls stay in the current object");
    seen = local_probe();
    ASSERT(seen[0] == runner);
  END_TEST();

  TEST("a call_other to itself counts as a step");
    seen = self_probe();
    ASSERT(seen[0] == this_object());
    ASSERT(seen[2] == runner);
  END_TEST();

  IT("previous_object() out of the chain is nil", previous_object(1000) == nil);

  TEST("initial_object() is the bottom of the chain");
    ASSERT(objectp(initial_object()));
    ASSERT(initial_object() == previous_object(-1)[sizeof(previous_object(-1)) - 1]);
  END_TEST();
}
