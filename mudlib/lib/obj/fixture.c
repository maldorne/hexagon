// A fixed object: part of the place it stands in, like a great tree or a heavy
// stone. It cannot be taken and weighs nothing, and a location clones it again
// every time it loads (see restore_fixtures in /lib/location.c), so it is
// always there. What a fixture does belongs to what inherits it.
//
// The actions a fixture answers to are declared with add_fixture_action, not
// add_action: the fixture registers them on whoever comes near, and looking at
// it ends with the same "you can" line a prop shows, so nobody has to guess the
// verb. A fixture whose use depends on the viewer (a skill, a state) redefines
// query_usage_verbs.

#include <translations/props.h>

inherit "/lib/item.c";

// ({ function, verbs }) for every action declared
private mixed * fixture_actions;

void create()
{
  fixture_actions = ({ });
  ::create();
  reset_get();
  set_weight(0);
}

// Locations and room2loc ask this to tell a fixture from a loose object.
int query_fixture() { return 1; }

/**
 * Declare an action of this fixture: `function` answers to `verbs` (a verb
 * or an array of them, the first being the one the "you can" line shows).
 */
void add_fixture_action(string function, mixed verbs)
{
  if (!fixture_actions)
    fixture_actions = ({ });
  if (stringp(verbs))
    verbs = ({ verbs });
  fixture_actions += ({ ({ function, verbs }) });
}

void init()
{
  int i;

  ::init();
  for (i = 0; i < sizeof(fixture_actions); i++)
    add_action(fixture_actions[i][0], fixture_actions[i][1]);
}

// The verbs `viewer` can use on this fixture, one per action.
string * query_usage_verbs(object viewer)
{
  string * verbs;
  int i;

  verbs = ({ });
  for (i = 0; i < sizeof(fixture_actions); i++)
    if (sizeof(fixture_actions[i][1]))
      verbs += ({ fixture_actions[i][1][0] });
  return verbs;
}

// The "you can" line for `viewer`, or "" when there is nothing to do.
string query_actions_hint(object viewer)
{
  string * verbs;

  verbs = query_usage_verbs(viewer);
  if (!sizeof(verbs))
    return "";
  return _LANG_PROPS_YOU_CAN + implode(verbs, _LANG_PROPS_LIST_SEPARATOR) +
         _LANG_PROPS_LIST_TERMINATOR + "\n";
}

string long(varargs string str, int dark)
{
  return ::long(str, dark) + query_actions_hint(this_player());
}
