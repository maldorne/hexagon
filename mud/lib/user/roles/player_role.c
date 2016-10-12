

static object _player;
static string _name;

void create()
{
  ::create();
  _name = "player";
}

void role_commands()
{

}

static void initialize_roles(object player)
{
  
}

static nomask object query_player() { return _player; }

nomask int set_player(object ob)
{
  // for safety reasons, we allow set_player only to be called from /lib/user/login
  // the login progname should be 3 steps ago in the call trace
  if (!valid_progname(3, "/lib/user/login"))
    return 0;

  _player = ob;

  initialize_roles(_player);
  role_commands();

  return 1;
}

mixed * stats() 
{
  return ({ 
      ({ "(role) Role name", _name }),
          });
}

