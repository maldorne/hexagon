
// this is the manager/coordinator role object

inherit base "/lib/user/roles/coder_role.c";

void create()
{
  base::create();
}

static void start_role(object user)
{
  base::start_role(user);
}

static void role_commands()
{
  // player and coder commands
  base::role_commands();
}

mixed stats()
{
  return ({ }) + base::stats();
}