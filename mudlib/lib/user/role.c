
#include <trace.h>
#include <basic/id.h>
#include <user/roles.h>
#include <mud/secure.h>

static object _role;
string role_name;
int invis;

void create()
{
  role_name = LOGIN_ROLE;
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
  object player;

  // for safety reasons, we allow set_role only to be called from /lib/core/login
  if (!SECURE->valid_progname("/lib/core/login"))
    return 0;

  if (!interactive(this_object()))
    return 0;

  player = this_object()->player();

  switch(name)
  {
    case ADMIN_ROLE:
      role_name = name;
      _role = clone_object(ADMIN_ROLE_OB);
      _role->set_user(this_object());

      // euid as coder
      seteuid(player->query_name());
      break;

    case CODER_ROLE:
      role_name = name;
      _role = clone_object(CODER_ROLE_OB);
      _role->set_user(this_object());

      // euid as coder
      seteuid(player->query_name());
      break;

    case PLAYER_ROLE:
      role_name = PLAYER_ROLE;
      _role = clone_object(PLAYER_ROLE_OB);
      _role->set_user(this_object());
      break;

    case LOGIN_ROLE:
    default:
      role_name = LOGIN_ROLE;
      _role = clone_object(LOGIN_ROLE_OB);
      _role->set_user(this_object());
      break;
  }

  return 1;
}

int query_admin()         { return role_name == ADMIN_ROLE; }
int query_administrator() { return query_admin(); }
int query_coder()         { return (role_name == CODER_ROLE || role_name == ADMIN_ROLE); }

string query_object_type()
{
  // not yet logged in with a character
  if (this_object()->query_name() == "object")
  {
    // logged with a login role, player is a /lib/link object
    if (this_object()->player() && (this_object()->player()->query_name() == "logged"))
      return O_LOGGED;

    return O_LOGIN;
  }

  switch(role_name)
  {
    case LOGIN_ROLE:
      return O_LOGGED;
    case PLAYER_ROLE:
      return O_PLAYER;
    case CODER_ROLE:
      return O_CODER;
    case ADMIN_ROLE:
      return O_ADMINISTRATOR;
  }

  return PLAYER_ROLE;
}

nomask int set_invis(int i)
{
  if (!query_admin())
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

