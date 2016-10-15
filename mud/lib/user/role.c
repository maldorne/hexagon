
#include <trace.h>
#include <basic/id.h>
#include <user/roles.h>
#include <mud/secure.h>

static object _role;
string role_name;

void create()
{
  role_name = PLAYER_ROLE;
}

void dest_me()
{
  destruct(_role);
}

object query_role() { return _role; }
string query_role_name() { return role_name; }

int set_role(string name)
{
  // for safety reasons, we allow set_role only to be called from /lib/core/login
  // the login progname should be 1 steps ago in the call trace
  if (!SECURE->valid_progname(1, "/lib/core/login"))
    return 0;

  if (!interactive(this_object()))
    return 0;

  switch(name)
  {
    case ADMIN_ROLE:
      break;

    case MANAGER_ROLE:
      break;

    case CODER_ROLE:
      role_name = name;
      _role = clone_object(CODER_ROLE_OB);
      _role->set_player(this_object());

      // euid as coder
      seteuid(this_object()->query_name());
      
      break;

    case PLAYER_ROLE:
    default:
      role_name = PLAYER_ROLE;
      _role = clone_object(PLAYER_ROLE_OB);
      _role->set_player(this_object());
      break;
  }

  return 1;
}

int query_coder()         { return role_name == CODER_ROLE; }
int query_manager()       { return role_name == MANAGER_ROLE; }
int query_admin()         { return role_name == ADMIN_ROLE; }
int query_administrator() { return role_name == ADMIN_ROLE; }

string query_object_type()
{
  string name;
  name = this_object()->query_name();

  if (!strlen(name) || (name == DEF_NAME))
    return "X";

  switch(role_name)
  {
    case PLAYER_ROLE:
      return O_PLAYER;
    case CODER_ROLE:
      return O_CODER;
    case MANAGER_ROLE:
      return O_MANAGER;
    case ADMIN_ROLE:
      return O_ADMINISTRATOR;
  }

  return PLAYER_ROLE;
} /* query_object_type() */


mixed * stats() 
{
  return (_role ? _role->stats() : ({ })) + ({ 
    ({ "Role Name" , role_name }),
        });
}

