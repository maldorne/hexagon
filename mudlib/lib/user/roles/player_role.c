
#include <mud/secure.h>
#include <user/roles.h>

inherit base   "/lib/user/roles/login_role.c";

void create()
{
  base::create();
  seteuid(PLAYER_EUID);
}

void role_commands()
{
  // login commands
  base::role_commands();
}

static void start_role(object user)
{
  base::start_role(user);
}

mixed * stats()
{
  return ({ }) + base::stats();
}

