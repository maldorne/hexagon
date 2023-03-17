
// link.c is the type of object that is created when a user logs in.
// is equivalent to a player object, but it is not saved.
// Once the user logs in, the link object is replaced by a player object.

#include <user/player.h>
#include <user/roles.h>
#include <user/login.h>
#include <mud/secure.h>


inherit history "/lib/player/history";
inherit ob "/lib/core/object";

static object _user;      // the user/account object that handles the connection
string account_name;      // user email, used to find the owner account
string role_name;         // role this player has (player, coder, etc)

static int last_command;  // time of last command
static int _net_dead;     // has lost connection?

// Function prototypes
nomask object user();
nomask int query_coder();
nomask int save_me();

void create()
{
  history::create();
  ob::create();

  seteuid(LOGIN_EUID);

  account_name = "";
  // player role by default
  role_name    = LOGIN_ROLE;
  _net_dead    = 0;
  last_command = time();

  if (clonep(this_object()))
    set_heart_beat(1);
}

void init()
{
  history::init();
  ob::init();
}

nomask int query_link() { return 1; }
nomask int query_player() { return 0; }
nomask int query_user() { return 0; }
nomask object user() { return _user; }
nomask int query_coder()
{
  if (_user)
    return _user->query_coder();
  return 0;
}
nomask int query_admin()
{
  if (_user)
    return _user->query_admin();
  return 0;
}
nomask int save_me()
{
  // we don't save link objects
  return 1;
}

nomask void set_user_ob(object ob)
{
  // for safety reasons, we allow this call only from /lib/core/login
  if (!SECURE->valid_progname("/lib/core/login"))
    return;

  _user = ob;
}

string query_account_name() { return account_name; }
void set_account_name(string str)
{
  // for safety reasons, we allow set_account_name only to be called from /lib/core/login
  if (!SECURE->valid_progname("/lib/core/login"))
    return;

  if (account_name != "")
    return; // changing account name is not allowed
  
  account_name = str;
  // save_me();
}

string query_role_name() { return role_name; }

void dest_me()
{
  // remove the user from the user handler
  // if (clonep(this_object()))
  //   find_object(USER_HANDLER)->remove_user(this_object());

  // main dest_me
  ob::dest_me();
}

// do not allow movement of this kind of "avatar"
int move(mixed dest, varargs mixed messin, mixed messout)
{
  return ob::move(LOGIN_LOUNGE, messin, messout);
}

void heart_beat()
{
  ob::heart_beat();

  // stderr("   ++++++++ this_object()  : " + object_name(this_object()) + "\n");
  // stderr("   + this_player()  : " + (this_player() ? object_name(this_player()) : "nil") + "\n");
  // stderr("   + this_player(1) : " + (this_player(1) ? object_name(this_player(1)) : "nil") + "\n");
  // stderr("   + this_user()    : " + (this_user() ? object_name(this_user()) : "nil") + "\n");

  if (!_user || (_user->player() != this_object()))
    dest_me();
}

mixed * stats()
{
  return ({
    ({ "Account Name", account_name }),
    ({ "Role Name", role_name }),
          }) + history::stats() +
               ob::stats();
}
