
#include <trace.h>
#include <user/roles.h>

static object _role;

void create()
{

}

object query_role() { return _role; }

int set_role(string name, object player)
{
  // for safety reasons, we allow set_role only to be called from /lib/user/login
  // the login progname should be 3 steps ago in the call trace
  if (!valid_progname(2, "/lib/user/login"))
    return 0;

  if (!interactive(player))
    return 0;

  switch(name)
  {
    case "administrator":
      break;
    case "coder":
      _role = clone_object(CODER_ROLE);
      _role->set_player(player);
      break;
    case "player":
    default:
      break;
  }

  return 1;
}


mixed * stats() 
{
  return (_role ? _role->stats() : ({ }));
}

