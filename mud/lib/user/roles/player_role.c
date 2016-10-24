
#include <mud/secure.h>
#include <user/roles.h>

static object _player;

void create()
{
  ::create();

  seteuid(PLAYER_EUID);
}

void role_commands()
{
  add_action("do_clear_screen", "clear");
  add_action("help_func", ({ "help", "ayuda" }));

  add_action("save", ({ "save", "salvar", "grabar" }));
  add_action("quit", ({ "quit", "salir" }));
}

static void start_role(object player)
{
  
}

static nomask object query_player() { return _player; }

nomask int set_player(object ob)
{
  // for safety reasons, we allow set_player only to be called from /lib/core/login
  if (!SECURE->valid_progname("/lib/core/login"))
    return 0;

  _player = ob;

  start_role(_player);
  role_commands();

  return 1;
}

nomask int save(string str) 
{
  return _player->save_me();
}

nomask int quit(string str)
{
  return _player->quit();
}

nomask int help_func(string str) 
{
  return _player->do_help(str);
}

mixed * stats() 
{
  return ({ 
          });
}

