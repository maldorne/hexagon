
#include <trace.h>
#include <user/roles.h>
#include <mud/secure.h>

static object _role;
string role_name;

void create()
{
  role_name = "";
}

object query_role() { return _role; }

int set_role(string name, object player)
{
  // for safety reasons, we allow set_role only to be called from /lib/core/login
  // the login progname should be 1 steps ago in the call trace
  if (!SECURE->valid_progname(1, "/lib/core/login"))
    return 0;

  if (!interactive(player))
    return 0;

  switch(name)
  {
    case "administrator":
      break;

    case "coder":
      role_name = name;
      _role = clone_object(CODER_ROLE_OB);
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
  return (_role ? _role->stats() : ({ })) + ({ 
    ({ "Role Name" , role_name }),
        });
}

