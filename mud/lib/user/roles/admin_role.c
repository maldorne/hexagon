// this is the lord player object

inherit base   "/lib/user/roles/coder_role.c";

void create()
{
  base::create();
}

static void start_role(object player)
{
  base::start_role(player);
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