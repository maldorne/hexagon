
#include <trace.h>
#include <basic/id.h>
#include <user/roles.h>
#include <mud/secure.h>

static object _role;
string role_name;
int invis;

void create()
{
  role_name = PLAYER_ROLE;
  invis = 0;
}

void dest_me()
{
  if (_role)
    destruct(_role);
}

object query_role() { return _role; }
string query_role_name() { return role_name; }

int set_role(string name)
{
  // for safety reasons, we allow set_role only to be called from /lib/core/login
  if (!SECURE->valid_progname("/lib/core/login"))
    return 0;

  if (!interactive(this_object()))
    return 0;

  switch(name)
  {
    case ADMIN_ROLE:
      role_name = name;
      _role = clone_object(ADMIN_ROLE_OB);
      _role->set_player(this_object());

      // euid as coder
      seteuid(this_object()->query_name());
      break;

    case MANAGER_ROLE:
      role_name = name;
      _role = clone_object(MANAGER_ROLE_OB);
      _role->set_player(this_object());

      // euid as coder
      seteuid(this_object()->query_name());
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

int query_admin()         { return role_name == ADMIN_ROLE; }
int query_administrator() { return query_admin(); }
int query_manager()       { return role_name == MANAGER_ROLE ||
                                   query_administrator(); 
                          }
int query_coder()         { return (role_name == CODER_ROLE) || 
                                    query_manager() || 
                                    query_administrator(); 
                          }

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

nomask int set_invis(int i) 
{
  if (query_coder())
  { 
    if (i)
        invis = 1;
    else
        invis = 0;
  }
  else
    invis = i;
}

nomask int query_invis() { return invis; }

nomask string query_path()
{
  string path;
  path = _role->query_path();

  if (strlen(path))
    return path;
  return "";
}

mixed * stats() 
{
  return (_role ? _role->stats() : ({ })) + ({ 
    ({ "Role Name" , role_name }),
        });
}

