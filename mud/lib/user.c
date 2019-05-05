
#include <user/user.h>
#include <user/input.h>
#include <mud/secure.h>
#include <user/hud.h>
#include <language.h>

inherit obj         "/lib/core/object";
inherit commands    "/lib/user/commands.c";
inherit output      "/lib/user/output";
inherit communicate "/lib/user/communicate";
inherit help        "/lib/user/help";
inherit prompt      "/lib/user/prompt";
inherit role        "/lib/user/role";
inherit more_string "/lib/user/more_string";
inherit more_file   "/lib/user/more_file";

// interactive object info
static object redirect_input_ob;       // object that will catch input and
static string redirect_input_function; // function inside that object
static mixed  redirect_input_args;     // optional arguments passed to the function

static int timestamp;     // last input time
static int echo;          // input echo

int start_time;           // time of first connection
int last_connected;
string last_on_from;      // last ip the user connected from

static object _player;    // player avatar

// user account info
string account_name;      // user email
string password;
static string tmp_password;

string real_name;
string birthday;
string location;
string * player_list;

string hud;

static int save_counter;  // each reset counter
static int last_command;  // time of last command

// TMP DEBUG, REMOVE!!!
// string query_name() { return "neverbot"; }
// string short(varargs int dark) { return "neverbot"; }
int query_verbose() { return 1; }
// END TMP DEBUG

#include "/lib/user/start.c"


// Function prototypes
// static void open();
// static void close(varargs mixed dest);
// static void receive_message(string str);
// nomask int set_input_to(object obj, string func, varargs int flag, mixed arg)

void create()
{
  output::create();
  communicate::create();
  more_string::create();
  more_file::create();
  help::create();
  prompt::create();
  commands::create();
  role::create();
  obj::create();

  player_list = ({ });
  account_name = "";
  real_name = "";
  birthday = "";
  location = "";

  seteuid(PLAYER_EUID);

  timestamp      = 0;
  echo           = 1;
  hud            = HUD_DIFFICULTY;

  start_time     = time();
  last_connected = time();
  last_on_from   = "";

  save_counter   = 0;
  last_command   = time();

  redirect_input_ob       = nil;
  redirect_input_function = "";
  redirect_input_args     = ({ });

  // the player object will have the heart_beat
  // if (clonep(this_object()))
  //   set_heart_beat(1);
}

void init()
{
  // ::init();
}

nomask int query_user() { return 1; }
nomask object user() { return this_object(); }
// total time connected
nomask int query_timestamp() { return timestamp; }

void dest_me()
{
  if (_player) {
    catch(_player->dest_me());
  }

  // remove the user from the user handler
  if (clonep(this_object()))
    find_object(USER_HANDLER)->remove_user(this_object());

  role::dest_me();
  obj::dest_me();
}

// void account_commands()
// {
//   commands::account_commands();
// }

nomask int valid_password(string pass)
{
  if (!strlen(pass))
    return 0;

  pass = crypt(pass, password);

  if (pass != password)
    return 0;

  return 1;
}

nomask int query_player() { return 1; }
nomask object player() { return _player; }
nomask int set_player_ob(object ob)
{
  // for safety reasons, we allow set_player_ob only to be called from /lib/core/login
  if (!SECURE->valid_progname("/lib/core/login"))
    return 0;

  _player = ob;
  _player->set_user_ob(this_object());

  return 1;
}

// Called from the input_to efun
nomask int set_input_to(object obj, string func, varargs int flag, mixed args...)
{
  if (redirect_input_ob == nil)
  {
    redirect_input_ob = obj;
    redirect_input_function = func;
    redirect_input_args = args;

    if (flag)
    {
      echo = 0;
      ::send_message(0);
    }

    return TRUE;
  }

  return FALSE;
}

// called from the driver
static void open()
{
  timestamp = time();

  MUDOS->set_initiator_user(this_object());
  MUDOS->set_initiator_object(this_object());

  LOGIN->logon(this_object());

  MUDOS->set_initiator_user(nil);
  MUDOS->set_initiator_object(nil);
}

// called from the driver
static void close(mixed arg)
{
  // use tell_object insted of write, the reason of the disconnection
  // could be in a different user (ie: a reconnection)
  tell_object(this_object(), _LANG_DISCONNECTED);
  // write(_LANG_DISCONNECTED);
}

nomask void save_me()
{
  if (!account_name || (account_name == ""))
    return;

  save_object(USERS_SAVE_DIR + account_name[0..0] + "/" + account_name + ".o", 1);
}

nomask int restore_me(varargs string account)
{
  if (!SECURE->valid_progname("/lib/core/login") &&
      !SECURE->valid_progname("/lib/core/secure/finger"))
    return 0;

  if (account)
    account_name = account;

  if (!account_name || (account_name == ""))
    return 0;

  return restore_object(USERS_SAVE_DIR + account_name[0..0] + "/"  + account_name + ".o", 1);
}

void send_message(string str)
{
  if (str == nil)
    return;

  ::send_message(str);
}

// called from the driver

// When the output buffer has emptied, message_done() will be called in
// the user object.
// void message_done()
// {
// }

// the skeleton of this function started taking the melville received_message
// as the general idea, but I think it will change a lot with time
static void receive_message(string str)
{
  string tmp_redirect_func;
  object tmp_redirect_obj;
  mixed  tmp_redirect_args;
  string verb;
  string params;
  string * pieces;
  int i;

  rlimits (MAX_USER_DEPTH; MAX_USER_TICKS)
  {
    timestamp = time();

    // restore the original echo value
    if (echo == 0)
    {
      echo = 1;
      ::send_message(1);
    }

    // input_to redirection
    if (redirect_input_ob)
    {
      // to have this_player() inside an input_to redirection
      MUDOS->set_initiator_user(this_object());
      MUDOS->set_initiator_object(this_object());

      tmp_redirect_obj  = redirect_input_ob;
      tmp_redirect_func = redirect_input_function;
      tmp_redirect_args = redirect_input_args;

      redirect_input_ob       = nil;
      redirect_input_function = "";
      redirect_input_args     = ({ });

      call_other(tmp_redirect_obj, tmp_redirect_func, str, tmp_redirect_args...);

      if (!redirect_input_ob &&
          this_object() && this_object()->query_show_prompt())
      {
        show_prompt();
      }

      // the object destructed itself
      if (!this_object()) {
        return;
      }

      MUDOS->set_initiator_user(nil);
      MUDOS->set_initiator_object(nil);

      return;
    }

    // old ccmudlib process_input content

    // Taniwha crash workround
    // if ( !strsrch(str,"%^") )
    //   return;

    // while ( str[<1..<1] == " " )
    //    str = str[0..<2];
    str = trim(str);

    if ( !strlen(str) || str == "\n" )
    {
      // \n added before the prompt
      show_prompt("\n");
      return;
    }

    // the new line has content, so we have a new this_player()
    MUDOS->set_initiator_user(this_object());
    MUDOS->set_initiator_object(this_object());

    if ( strlen(str) > INPUT_MAX_STRLEN )
    {
      str = str[ 0..INPUT_MAX_STRLEN ];
      write("Comando demasiado largo - procesando de todas formas.\n");
    }

    // Ok, my stuff: (Baldrick)

    if (_player)
    {
      // Might as well move this here too
      if (str[0] == '.')
      {
        this_object()->set_trivial_action();
        str = _player->expand_history(str[1..]);
      }
      else
      {
        // Bishop - adding history
        _player->add_history( str );
      }
    }

    sscanf(str, "%s %s", verb, params);

    if (!verb)
      verb = str;

    // First the aliases
    if ( _player && !_player->exec_alias(verb, params) )
    {
      // if no alias found, continue
      _player->action_check( str );
      _player->lower_check( str );
    }

    // the object destructed itself
    if (!this_object())
      return;

    MUDOS->set_initiator_user(nil);
    MUDOS->set_initiator_object(nil);

  } // rlimits
}

// void heart_beat()
// {
//   if (this_object() && !interactive(this_object()) )
//   {
//     if (name == "guest" || name == "root")
//     {
//       say(query_cap_name()+" es engullid"+G_CHAR+" por una nube de lógica.\n");
//       quit();
//     }
//     else if (!net_dead)
//     {
//       say(query_cap_name()+" se vuelve blanc"+G_CHAR+" y sólid"+G_CHAR+" al tiempo que se "+
//         "convierte en una estatua.\n");
//       event(users(), "inform", query_cap_name() + " ha perdido " +
//         query_possessive() + " conexión", "conexiones");
//       save_me();
//       quit();
//       net_dead = 1;
//     }
//     // last_command = time() - this_object()->query_idle();
//   }

//   // query_idle no se puede ejecutar sobre objetos no interactive,
//   // asi que saco esta linea del if anterior, neverbot 7/05
//   if (!net_dead)
//     last_command = time() - this_object()->query_idle();

//   living::heart_beat();
// }

void set_account_name(string str)
{
  // for safety reasons, we allow set_account_name only to be called from /lib/core/login
  if (!SECURE->valid_progname("/lib/core/login"))
    return;

  if (account_name != "")
    return; // changing account name is not allowed

  account_name = str;
}

string query_account_name() { return account_name; }
void set_password(string str)
{
  password = str;
  save_me();
}

string query_password() { return password; }
string query_email() { return account_name; }
void set_real_name(string str) { real_name = str; }
string query_real_name() { return real_name; }
void set_birthday(string str) { birthday = str; }
string query_birthday() { return birthday; }
void set_location(string str) { location = str; }
string query_location() { return location; }

string * query_player_list() { return player_list; }

void add_player(string name)
{
  if (member_array(name, player_list) != -1)
    return;
  player_list += ({ name });
  save_me();
}

void remove_player(string name)
{
  player_list -= ({ name });
  save_me();
}

string query_last_on_from() { return last_on_from; }
int query_start_time() { return start_time; }
int query_last_connection() { return last_connected; }
void update_last_connection()
{
  last_connected = time();
  save_me();
}

// query_idle - determine how many seconds an interactive player has been idle
// int query_idle( object ob );
// Query how many seconds a player object (ob) has been idling.

nomask int query_idle()
{
  return time() - timestamp;
}

mixed * stats()
{
  return ({
    ({ "Account Name", account_name }),
    ({ "Timestamp", timestamp, }),
    ({ "Last On From", last_on_from }),
    ({ "Hud", hud }),
          }) + help::stats() +
               output::stats() +
               communicate::stats() +
               prompt::stats() +
               role::stats() +
               commands::stats() +
               obj::stats();
}
