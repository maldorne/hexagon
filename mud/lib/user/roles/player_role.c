
#include <mud/secure.h>

static object _player;

void create()
{
  ::create();

  seteuid(PLAYER_EUID);
}

void role_commands()
{

}

static void initialize_role(object player)
{
  
}

static nomask object query_player() { return _player; }

nomask int set_player(object ob)
{
  // for safety reasons, we allow set_player only to be called from /lib/core/login
  // the login progname should be 2 steps ago in the call trace
  if (!SECURE->valid_progname(2, "/lib/core/login"))
    return 0;

  _player = ob;

  initialize_role(_player);
  role_commands();

  return 1;
}

mixed * stats() 
{
  return ({ 
          });
}

