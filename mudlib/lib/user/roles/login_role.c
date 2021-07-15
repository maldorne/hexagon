
#include <mud/secure.h>
#include <user/roles.h>

static object _user;

void create()
{
  ::create();
  seteuid(LOGIN_EUID);
}

void role_commands()
{
  add_action("do_clear_screen", "clear");

  add_action("save", ({ "save", "salvar", "grabar" }));
  add_action("quit", ({ "quit", "salir" }));
}

static void start_role(object user)
{

}

static nomask object user() { return _user; }

nomask int set_user(object ob)
{
  // for safety reasons, we allow set_user only to be called from /lib/core/login
  if (!SECURE->valid_progname("/lib/core/login"))
    return 0;

  _user = ob;

  start_role(_user);
  role_commands();

  return 1;
}

nomask int save(string str)
{
  object player;
  player = _user->player();

  catch(player->save_me());
  catch(_user->save_me());

  return 1;
}

nomask int quit(string str)
{
  object player;
  player = _user->player();

  return player->quit();
}

nomask int do_clear_screen(string str)
{
  // if( this_player(1) != this_user() )
  //   return 0;

  write(sprintf("%c[H%c[2J\n", 27, 27));
  return 1;
}

mixed * stats()
{
  return ({
          });
}

