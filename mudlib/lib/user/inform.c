
#include <translations/inform.h>

mapping informs;

// A new account starts with the events every player should see; a saved one
// restores its own choice over this.
void create()
{
  informs = ([ INFORM_EVENT_HEALTH : TRUE ]);
}

mapping query_informs() { return informs; }

int query_inform(string what)
{
  string * on;

  if (undefinedp(informs[what]))
    return 0;

  return (informs[what] == TRUE);
}

void set_inform(string what, int value)
{
  string * types;

  if (this_object()->query_admin())
    types = INFORM_EVENTS_ADMINS;
  else if (this_object()->query_coder())
    types = INFORM_EVENTS_CODERS;
  else
    types = INFORM_EVENTS_PLAYERS;

  // off 
  if ((value == 0) && (member_array(what, types) != -1))
  {
    // inform[what] = FALSE;
    m_delete(informs, what);
    return;
  }

  // on
  if ((value == 1) && (member_array(what, types) != -1))
  {
    informs[what] = TRUE;
    return;
  }
}

mixed * stats()
{
  return ({
    ({ "Informs", informs }),
  });
}