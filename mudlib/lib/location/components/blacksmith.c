// Blacksmith component. Marks a location as a smithy: it has a forge, an anvil
// and the tools to work metal, and it is where the repair skill can be used.
//
// The forge is lit while somebody works it: somebody whose work is this place
// and is standing in it. Without a job declared here the forge is always lit,
// so a smithy that nobody staffs is still a smithy.

#include <room/location.h>
#include <language.h>

inherit component "/lib/location/component.c";

void create()
{
  component::create();
  set_type(LOCATION_COMPONENT_BLACKSMITH);
}

void init() {}
void dest_me() {}

// Whether the forge is burning: the same condition a shop opens under, somebody
// who works here is in it (or nobody is meant to). See
// /lib/ventures/conditions/attended.c.
int query_forge_lit()
{
  object loc;

  loc = query_my_location();
  if (!loc)
    return 0;

  return call_other("/lib/ventures/conditions/attended", "check_open",
                    this_object(), loc, nil, ([ ])) == 1;
}

string query_info()
{
  return query_forge_lit() ? "forge lit" : "forge cold";
}

mapping query_hooks()
{
  return ([ "extra_look": HOOK_PRIORITY_ATMOSPHERE ]);
}

// The forge is part of the place: under the description, a line saying whether
// it is burning or cold.
string hook_extra_look(mixed * args)
{
  return query_forge_lit() ? _LANG_BLACKSMITH_FORGE_LIT
                           : _LANG_BLACKSMITH_FORGE_COLD;
}
