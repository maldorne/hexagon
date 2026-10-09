// A fixed object: part of the place it stands in, like a great tree or a heavy
// stone. It cannot be taken and weighs nothing, and a location clones it again
// every time it loads (see restore_fixtures in /lib/location.c), so it is
// always there. What a fixture does belongs to what inherits it.

inherit "/lib/item.c";

void create()
{
  ::create();
  reset_get();
  set_weight(0);
}

// Locations and room2loc ask this to tell a fixture from a loose object.
int query_fixture() { return 1; }
