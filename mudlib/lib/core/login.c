
// old new_login.c object, now this is the only way to log in
// added some things from the old login.c
//   neverbot 21/01/03
// changed login system to include user users
//   neverbot 12/2010
// ported to dgd, neverbot 03/2014

inherit "/lib/core/object";

#include <kernel.h>
#include <mud/config.h>
#include <mud/secure.h>
#include <mud/access.h>
#include <mud/version.h>
#include <mud/patch_version.h>
#include <mud/cmd.h>
#include <user/login.h>
#include <user/user.h>
#include <user/player.h>
#include <areas/common.h>
#include <user/user.h>
#include <translations/common.h>
#include <language.h>

static object _user;   // connection object
static object _player; // avatar object

static int validated;
static string password, user_name;
// static int cols;
static int chosen_gender;
static int coder;
// static int app_coder;
static int no_times;
static int invis_wish;
static object new_copy, other_copy;

// static string player_ob;

// prototypes
nomask void logon(object pl);
nomask void show_options();
nomask void logon_with_user_name(string password);
nomask void logon_with_player_name(string password, int flag);
nomask void begin(int is_new_player, varargs int reconnected, object destination);
nomask void try_throw_out(string str);
nomask void time_out();
nomask void disconnect(varargs int silence);

void create()
{
  ::create();

  validated = FALSE;

  no_times = 0;
  coder = 0;
  password = "";
  chosen_gender = 1;
  invis_wish = -1;
  seteuid(ROOT);
}

nomask int test_user(string str)
{
  string tmp;
  object player;
  int result;

  if (!stringp(str) || sscanf(str, "%s ",tmp))
    return 0;

  player = clone_object("/lib/player.c");
  player->set_name(str);
  result = player->restore_me();

  player->dest_me();
  return result;
}

nomask void time_out()
{
  if (validated == TRUE)
    return;

  if (!_user)
    return;

  write(_LANG_TIMED_OUT);
  disconnect(1);
}

// nomask void quit()
// {
//   destruct(_user);
//   destruct(this_object());
// }

nomask void logon(object u)
{
  // int tim;
  _user = u;
  // objects cannot change name once is set, so DONT do this
  // _user->set_name("login");

  // temporary "player" object while not using a character
  // this object will destroy itself when not needed
  _player = clone_object(LINK_OB);

  // before the _player->move or everything will explode
  // will link back player -> user, too
  _user->set_player_ob(_player);

  // masked move functions, will be moved to their proper places
  // this will make the first init call to have commands available
  _user->move("whereever");
  _player->move("whereever");

  write(_LANG_LPMUD_VERSION + version() + "\n" +
        _LANG_MUDLIB_VERSION + MUDLIB_VERSION + " [ " + _LANG_LANGUAGE_VERSION + " ]\n"/* +
        _LANG_PATCH_VERSION + PATCH_VERSION + "\n\n"*/);

  call_out("time_out", LOGIN_TIMEOUT);

  cat(doc(WELCOME_MESSAGE));

  // TODO check shut object to avoid login near a shutdown
  // if (find_object("/obj/shut"))
  // {
  //   tim = (int)"/obj/shut"->query_time_to_crash();
  //   if ((tim > 0) && (tim < 120))
  //   {
  //     write("Queda muy poco para el reinicio de "+mud_name()+", inténtalo dentro de unos minutos.\n");
  //     dest_me();
  //     return;
  //   }
  //   else
  //   {
  //     int mins;
  //     mins = (tim/60);
  //     write("El juego se reiniciará en "+mins+" minuto"+((tim<2)?"":"s")+".\n");
  //   }
  // }

  // added from old login.c, neverbot 21/01/03
  if (uptime() < MIN_UPTIME_LOGIN_TIME)
  {
    write(_LANG_WORLD_STILL_BEING_CREATED);
    disconnect(1);
    return;
  }

  if (sizeof(users()) > MAX_PLAYERS - CODER_SLOTS)
  {
    write(_LANG_TOO_MANY_PLAYERS);
    disconnect(1);
    return;
  }

  show_options();
}

static nomask void disconnect(varargs int silence)
{
  if (!silence)
    write(_LANG_COME_AGAIN_SOON);
  if (_player)
    destruct(_player);
  destruct(_user);
  destruct(this_object());
}

nomask void restore_user(string account_name)
{
  if (!_user->restore_me(account_name))
  {
    write(_LANG_NONEXISTANT_ACCOUNT);
    disconnect(1);
    return;
  }

  user_name = account_name;
}

nomask void restore_player(string character_name)
{
  _player = clone_object(PLAYER_OB);
  _player->set_name(character_name);

  // try to restore the character
  if (!_player->restore_me())
  {
    write(_LANG_CHARACTER_RESTORE_ERROR);
    disconnect(1);
    return;
  }

  _user->set_account_name(_player->query_account_name());
  if (!_user->restore_me())
  {
    write(_LANG_ACCOUNT_RESTORE_ERROR);
    disconnect(1);
    return;
  }

  // will link back player -> user, too
  _user->set_player_ob(_player);
}

nomask void show_options()
{
  string ret;

  ret = "\n" + _LANG_LOGIN_CMDS_OPTIONS + "\n";

  if (!validated) {
    ret += "  " + _LANG_OPTIONS_ACCOUNT_LOGIN + "\n";
    ret += "  " + _LANG_OPTIONS_CREATE_ACCOUNT + "\n";
    ret += "  " + _LANG_OPTIONS_GUEST_LOGIN + "\n";
  } else {
    ret += "  " + _LANG_OPTIONS_CHARACTER_LOGIN + "\n";
    ret += "  " + _LANG_OPTIONS_CREATE_CHARACTER + "\n";
    ret += "  " + _LANG_OPTIONS_LIST_CHARACTERS + "\n";
  }

  ret += "  " + _LANG_OPTIONS_CMD_GAMES + "\n";
  ret += "  " + _LANG_OPTIONS_CMD_FINGER + "\n";
  ret += "  " + _LANG_OPTIONS_CMD_WHO + "\n";

  // cat(doc(LOGIN_OPTIONS_MESSAGE));
  write(ret + "\n" + _LANG_ENTER_AN_OPTION);
  input_to("logon_option");
}

nomask void logon_option(string str)
{
  string * exploded_args;
  mapping available_cmd_info;
  string * available_cmds;
  int tmp;

  if (!strlen(str))
  {
    disconnect();
    return;
  }

  str = lower_case(str);
  exploded_args = explode(str, " ");

  available_cmd_info = CMD_HANDLER->query_hash_by_category(USER_CMD);
  available_cmds = keys(available_cmd_info);

  // new options, neverbot 02/2006
  // execute cmd avaibale to login roles, neverbot 09/2019
  if (member_array(exploded_args[0], available_cmds) != -1)
  {
    string args, err;
    int result;
    args = (sizeof(exploded_args) > 1) ? implode(exploded_args[1..], " ") : "";

    err = catch(result = (int)CMD_HANDLER->cmd(exploded_args[0], args, _user));
    if (err)
      write(_LANG_ERROR_HAPPENED);

    show_options();
    return;
  }

#ifdef PLAYER_LOCKOUT
  // if (member_array(str, ({ "invitado", "guest" }) != -1)
  // {
  //   write(_LANG_PLAYER_ACCESS_FORBIDDEN);
  //   destruct(_user);
  //   destruct(this_object());
  //   return;
  // }

  if (!read_file("/home/" + str + "/workroom.c"))
  {
    write(_LANG_PLAYER_ACCESS_FORBIDDEN);
    disconnect();
    return;
  }
#endif

  if (str == _LANG_CREATE_COMMAND)
  {
    // if not validated yet, we are creating a new user account
    if (!validated)
    {
      string ret;
      ret = _LANG_ACCOUNT_RECOMMEND;
      ret = sprintf("\n%-*s\n", _user->query_cols(), ret);
      write( ret );

      write(_LANG_ENTER_EMAIL);
      input_to("create_user");
      return;
    } 
    // already logged with a user account, let's create a new character
    else
    {
      write(_LANG_TYPE_THE_NEW_CHARACTER_NAME);
      input_to("create_player");
      return;      
    }
  }

  // from here we know we are trying to log in
  // with an existing user/character

  // from the old login.c, neverbot 21/01/03
  // More of Hamlet's cute crap.
  if (str[0] == '#')
  {
    invis_wish = 0;
    str = str[1..];
  }
  else if (str[0] == '*')
  {
    invis_wish = 1;
    str = str[1..];
  }
  else if (str[0] == '@')
  {
    invis_wish = 2;
    str = str[1..];
  }

  /*
  TODO bastards.c
  // query_player_ob() should only be passed 1 as its 2nd argument HERE!
  //   It's a big mistake to pass a second arg at all anywhere else. - Hamlet
  player_ob = (string)"/secure/bastards"->query_player_ob(str, 1);
  // changed next line to be stringp - Hamlet
  if (!stringp(player_ob))
  {
    write("Lo sentimos, pero ese nombre no está permitido.\n");
    destruct(_user);
    destruct(this_object());
    return ;
  }
  */

  // TODO guest login
  // if ((str == "invitado") || (str == "guest"))
  //   return guest_login();

  // now we accept emails as users names
  // if (strlen(str) > MAX_LEN)
  // {
  //   write("El nombre es demasiado largo, el máximo son 11 caracteres.\n");
  //   write(_LANG_ENTER_AN_OPTION);
  //   input_to("logon_option");
  //   return;
  // }

  // added by neverbot 21/01/03
  if (strlen(str) < MIN_LEN)
  {
    write(_LANG_OPTION_STRING_TOO_SHORT);
    show_options();
    return;
  }

  if (((tmp = SECURE->valid_user_name(str)) != -1) && !SECURE->valid_email(str))
  {
    write(_LANG_INVALID_CHARACTER_OR_EMAIL);
    show_options();
    return;
  }

  // the input is a character name
  if (file_size("/save/players/" + str[0..0] + "/" + str + ".o") > 0)
  {
    // if already logged, don't need to input password again
    if (validated) {
      string * list;
      list = _user->query_player_list();

      // our own character, load auto
      if (member_array(str, list) != -1) {
        restore_player(str);
        logon_with_player_name("", 1);
        return;
      }

      write(_LANG_NONEXISTANT_ACCOUNT_OR_CHARACTER);
      show_options();
      return;

    } else {
      restore_player(str);
      write(_LANG_TYPE_CHARACTER_PASSWORD);
      // flag to show it's a character name
      input_to("logon_with_player_name", 1, 0);
      return;
    }
  }
  // the input is an user name
  else if (!validated && file_size("/save/users/" + str[0..0] + "/" + str + ".o") > 0)
  {
    restore_user(str);
    write(_LANG_TYPE_ACCOUNT_PASSWORD);
    input_to("logon_with_user_name", 1);
    return;
  }
  else
  {
    write(_LANG_NONEXISTANT_ACCOUNT_OR_CHARACTER);
    show_options();
    return;
  }
}

nomask void logon_with_user_name(string password)
{
  int tmp;
  string * list;

  if (!strlen(password))
  {
    disconnect();
    return;
  }

  if (!_user->valid_password(password))
  {
    write(_LANG_WRONG_PASSWORD);

    if (++no_times >= MAX_RETRIES)
    {
      write(_LANG_TOO_MANY_RETRIES);
      disconnect(1);
      return;
    }

    write(_LANG_TYPE_ACCOUNT_PASSWORD);
    input_to("logon_with_user_name", 1);
    return;
  }

  // from here the password is ok
  validated = TRUE;

  // set the name of the player object (which is of type /lib/link.c)
  // to "mark" it as already logged
  // important: once a name is set, cannot be undone
  _player->set_name("logged");

  show_options();

  // list = _user->query_player_list();

  // write(_LANG_AVAILABLE_CHARACTERS_IN_ACCOUNT);

  // for (tmp = 0; tmp < sizeof(list); tmp++)
  //   write("   " + ((string)(tmp + 1)) + ") " + capitalize(list[tmp]) + "\n");

  // write(_LANG_CHOOSE_ACCOUNT_CHARACTER);
  // input_to("choose_character", 0, list);
}

nomask void choose_character(string str, string * list)
{
  string chosen;
  chosen = "";

  // creating a new player in the user
  if (str == _LANG_CHOOSE_NEW)
  {
    write(_LANG_TYPE_THE_NEW_CHARACTER_NAME);
    input_to("create_player");
    return;
  }

  list = _user->query_player_list();

  // existant character name
  if ((value = member_array(str, list)) != -1)
    chosen = list[value];
  else
  {
    value = atoi(str);
    // list goes from 1 to n, the array from 0 to n-1
    value--;

    if ((value >= 0) && (value < sizeof(list)))
      chosen = list[value];
  }

  // none of the user characters
  if (chosen == "")
  {
    write(_LANG_UNKNOWN_CHARACTER);
    input_to("choose_character", 0, list);
    return;
  }

  restore_player(chosen);

  // not needed, we already did the password validation
  // write(_LANG_TYPE_CHARACTER_PASSWORD);
  // flag to show we come from the user login
  // input_to("logon_with_player_name", 1, 1);
  logon_with_player_name("", 1);
}

// if flag == 0, login with the player name
// if flag == 1, we come from the user login, already authenticated
nomask void logon_with_player_name(string password, int flag)
{
  // login with the character name
  if (flag == 0)
  {
    // we have to check the password, wasn't check with the user
    if (!_user->valid_password(password))
    {
      write(_LANG_WRONG_PASSWORD);

      if (++no_times >= MAX_RETRIES)
      {
        write(_LANG_TOO_MANY_RETRIES);
        destruct(_user);
        disconnect(1);
        return;
      }

      write(_LANG_TYPE_CHARACTER_PASSWORD);
      input_to("logon_with_player_name", 1, flag);
      return;
    }
  }

  // from here, we have the user, the character and the password is valid

  other_copy = find_player(_player->query_name());

  if (other_copy == this_player())
  {
    begin(0);
    return;
  }

  if (other_copy)
  {
    write(_LANG_ALREADY_PLAYING);
    input_to("try_throw_out");
    return;
  }

  begin(0);
}

nomask void begin(int is_new_player, varargs int reconnected, object destination)
{
  // string player_ob;
  object room;

  // TODO: move query_player_ob checks here somehow
  // player_ob = SECURE->query_player_ob(name);

// only if we are using multiple servers and this is not the development one
#ifdef MULTIMUD
#ifndef CCMUD_DEVEL
  if (coder && !SECURE->query_admin(name))
  {
    write(_LANG_CODERS_FORBIDDEN);
    disconnect(1);
    return;
  }
#endif
#endif

  write(_LANG_WAIT_LOADING);

  // every user object will have the same name
  // if name != "object", that will mean the user has finished the login process
  _user->set_name("user");

  _player->set_living_name(_player->query_name()); // this is done both for players and npcs

  // store the user name in the user handler
  find_object(USER_HANDLER)->update_user(_user, _player);

  write(_LANG_CONNECTED_WELCOME);
  // true -> silent flag, just store as notification but do not show popup
  _user->add_notification("login", _LANG_CONNECTED_WELCOME, true);

  // CHECK
  if (query_property(GUEST_PROP))
    _player->add_property(GUEST_PROP, 1);

  // set user privileges
  _user->set_role(_player->query_role_name());

  // Hamlet stuff
  if (!coder)
    invis_wish = -1;
  else if ( (invis_wish == 2) && !SECURE->query_admin(_player->query_name()) )
    invis_wish = 1;

  _user->start(invis_wish, is_new_player);
  _player->start(invis_wish, is_new_player, reconnected, destination);

  // added by neverbot, this should be here somewhere
  // removed again by neverbot, we don't have news for now
  // if (!reconnected)
  //   cat(doc("news.txt"));

  // do not show the first prompt, a command will be issued
  // in move_player_to_start and after that the prompt will be
  // shown
  _user->set_no_prompt();

  destruct(this_object());
}

nomask void try_throw_out(string str)
{
  // object tmp, ob;
  object env;

  if (member_array(str[0], _LANG_NO_OPTIONS_ARRAY) != -1)
  {
    disconnect();
    return;
  }

  if (!other_copy)
  {
    disconnect();
    return;
  }

  env = environment(other_copy);

  if (catch(other_copy->quit()))
    if (catch(other_copy->dest_me()))
      destruct(other_copy);

  tell_room(env, _LANG_HAS_RECONNECTED, ({ _user, other_copy }) );
  event(users() - ({ other_copy, 0 }), "inform",
    _user->query_cap_name() + " reconnected", "link-death");

  begin(0, TRUE, env);
}

// *******************************************************
//  create user account
// *******************************************************

void create_user(string str);
void create_user2(string str);
void create_user3(string str);
void create_user4();

void create_user(string str)
{
  if (!strlen(str))
  {
    disconnect();
    return;
  }

  if (!SECURE->valid_email(str))
  {
    write(_LANG_ENTER_VALID_EMAIL);
    input_to("create_user");
    return;
  }

  if (file_size("/save/users/" + str[0..0] + "/" + str + ".o") > 0)
  {
    write(_LANG_USED_EMAIL);
    input_to("create_user");
    return ;
  }

  user_name = str;

  write(_LANG_ACCOUNT_NAME_CHOSEN);
  input_to("check_user_name");
}

int check_user_name(string str)
{
  str = implode(explode(str, " "), "");

  if (strlen(str))
  {
    if (member_array(str[0], _LANG_YES_OPTIONS_ARRAY) != -1)
    {
      write(_LANG_DEFINE_ACCOUNT_PASSWORD);
      input_to("create_user2", 1);
      return 1;
    }
    else if (member_array(str[0], _LANG_NO_OPTIONS_ARRAY) != -1)
    {
      write(_LANG_TRY_AGAIN);
      write(_LANG_ENTER_VALID_EMAIL);
      input_to("create_user");
      return 1;
    }
  }

  write(_LANG_ANSWER_YES_NO);
  input_to("check_user_name");
  return 1;
}

void create_user2(string str)
{
  if (!strlen(str))
  {
    disconnect();
    return;
  }

  password = crypt(str, password);

  write(_LANG_DEFINE_ACCOUNT_PASSWORD_REPEAT);
  input_to("create_user3", 1);
}

void create_user3(string str)
{
  string tmppasswd;

  if (!strlen(str))
  {
    write(_LANG_ACCOUNT_PASSWORD_REMOVED + " " + _LANG_ACCOUNT_REPEAT + "\n"+
      _LANG_DEFINE_ACCOUNT_PASSWORD);
    input_to("create_user2", 1);
    return;
  }

  tmppasswd = crypt(str, password);

  if (password != tmppasswd)
  {
    write(_LANG_ACCOUNT_DIFFERENT_PASSWORDS + " " + _LANG_ACCOUNT_REPEAT + "\n"+
      _LANG_DEFINE_ACCOUNT_PASSWORD);
    input_to("create_user2", 1);
    return;
  }

  create_user4();
}

void create_user4()
{
  // create the user object
  _user->set_user_name(user_name);
  _user->set_password(password);
  // _user->add_player(name);
  _user->update_last_connection();
  _user->save_me();

  // from here the account is ok
  validated = TRUE;

  write(_LANG_NEW_ACCOUNT_CREATED);

  // now we have to choose a new command, 
  // probably create a new character
  show_options();
}

// *******************************************************
//  create player character
// *******************************************************

void create_player2();

void create_player(string str)
{
  int tmp;

  if ((tmp = SECURE->valid_user_name(str)) != -1)
  {
    write(_LANG_INVALID_USER_NAME);
    write(_LANG_TYPE_THE_NEW_CHARACTER_NAME);
    input_to("create_player");
    return;
  }

  if (strlen(str) > MAX_LEN)
  {
    write(_LANG_CHARACTER_NAME_TOO_LONG);
    write(_LANG_TYPE_THE_NEW_CHARACTER_NAME);
    input_to("create_player");
    return;
  }

  if (strlen(str) < MIN_LEN)
  {
    write(_LANG_CHARACTER_NAME_TOO_SHORT);
    write(_LANG_TYPE_THE_NEW_CHARACTER_NAME);
    input_to("create_player");
    return;
  }

  if (file_size("/save/players/"+str[0..0]+"/"+str+".o") > 0)
  {
    write(_LANG_USED_CHARACTER_NAME);
    input_to("create_player");
    return ;
  }

  name = str;

  write(_LANG_CHARACTER_NAME_CHOSEN);
  input_to("check_player_name");
}

int check_player_name(string str)
{
  str = implode(explode(str, " "), "");

  if (member_array(str[0], _LANG_YES_OPTIONS_ARRAY) != -1)
  {
    write(_LANG_CHOOSE_CHARACTER_GENDER);
    input_to("get_sex");
    return 1;
  }
  else if (member_array(str[0], _LANG_NO_OPTIONS_ARRAY) != -1)
  {
    write(_LANG_TRY_AGAIN);
    write(_LANG_TYPE_THE_NEW_CHARACTER_NAME);
    input_to("create_player");
    return 1;
  }

  write(_LANG_ANSWER_YES_NO);
  input_to("check_player_name");
  return 1;
}

void get_sex(string str)
{
  int i;
  str = implode(explode(str, " "), "");

  if (!strlen(str))
  {
    write(_LANG_ANSWER_CHOOSE_AN_OPTION + "\n" + _LANG_CHOOSE_CHARACTER_GENDER);
    input_to("get_sex");
    return;
  }

  i = str[0];

  if (member_array(i, _LANG_GENDER_MAN_OPTIONS_ARRAY) != -1)
  {
    chosen_gender = 1;
  }
  else if (member_array(i, _LANG_GENDER_WOMAN_OPTIONS_ARRAY) != -1)
  {
    chosen_gender = 2;
  }
  else
  {
    write(_LANG_ANSWER_CHOOSE_AN_OPTION + "\n" + _LANG_CHOOSE_CHARACTER_GENDER);
    input_to("get_sex");
    return;
  }

  create_player2();
}

void create_player2()
{
  object old_player;
  old_player = _player;

  // end of the process, we create a new player object
  _player = clone_object(PLAYER_OB);
  _player->set_name(name);
  _player->set_gender(chosen_gender);
  _player->set_account_name(user_name);

  // destruct the link object
  destruct(old_player);

  _user->set_account_name(user_name);
  // will link back player -> user, too
  _user->set_player_ob(_player);
  _user->add_player(name);
  _user->save_me();

  // save the _user first, and then the _player, 
  // any message printed without a user will prompt an error, and 
  // even this save_me will print messages
  _player->save_me();

  begin(!query_property(GUEST_PROP));
}

mixed * stats()
{
  return ({
    ({ "Login validated", validated, }),
          });
}
