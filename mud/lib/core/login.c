
// old new_login.c object, now this is the only way to log in
// added some things from the old login.c
//   neverbot 21/01/03
// changed login system to include user accounts
//   neverbot 12/2010
// ported to dgd, neverbot 03/2014

inherit "/lib/core/object";

#include <kernel.h>
#include <mud/config.h>
#include <mud/secure.h>
#include <mud/access.h>
#include <mud/version.h>
#include <mud/patch_version.h>
#include <user/login.h>
#include <account/account.h>
#include <areas/common.h>
#include <user/user.h>
#include <language.h>

static object _player;

static int validated;
static object account;
static string password, account_name;
// static int cols;
static int gender;
static int coder;
// static int app_coder;
static int no_times;
static int invis_wish;
static object new_copy, other_copy;

// static string player_ob;

// prototypes
nomask void logon(object pl);
nomask void show_options();
nomask void logon_with_account_name(string password);
nomask void logon_with_player_name(string password, int flag);
nomask void begin(int is_new_player, varargs object destination);
nomask void try_throw_out(string str);
nomask void time_out();
nomask void disconnect(varargs int silence);

void create_account(string str);
void create_account2(string str);
void create_account3(string str);
void create_account4();


void create()
{
  ::create();

  validated = FALSE;

  no_times = 0;
  coder = 0;
  password = "";
  gender = 1;
  invis_wish = -1;
  seteuid(ROOT);
}

nomask int test_user(string str)
{
  string tmp;
  object user;
  int result;

  if (!stringp(str) || sscanf(str, "%s ",tmp))
    return 0;

  user = clone_object("/lib/user.c");
  user->set_name(str);
  result = user->restore_me();

  user->dest_me();
  return result;
}

nomask void time_out()
{
  if (validated == TRUE)
    return;

  if (!_player)
    return;

  write(_LANG_TIMED_OUT);
  destruct(_player);
  destruct(this_object());
}

// nomask void quit()
// {
//   destruct(_player);
//   destruct(this_object());
// }

nomask void logon(object pl)
{
  int tim;

  _player = pl;

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

  destruct(_player);
  destruct(this_object());
}

nomask void restore_account(string account_name)
{
  account = clone_object(ACCOUNT_OB);

  if (!account->restore_me(account_name))
  {
    write(_LANG_NONEXISTANT_ACCOUNT);
    destruct(account);
    show_options();
    return;
  }
}

nomask void restore_character(string character_name)
{
  _player->set_name(character_name);

  // try to restore the character
  if (!_player->restore_me())
  {
    write(_LANG_CHARACTER_RESTORE_ERROR);
    disconnect(1);
    return;
  }
}

nomask void show_finger(string who)
{
  write(_LANG_FINGER_COMMAND + "\n");

  /*
  string ret = "/secure/finger.c"->finger_info(who, this_object());

  if (!ret || (ret == ""))
  {
    write("\nNo hay ningún jugador con el nombre '"+who+"'.\n\n");
    return;
  }
  ret = "\n--------------------------------------------------------------------------\n"+
        ret +
        "--------------------------------------------------------------------------\n\n";

  write( "/global/events.c"->fix_string(ret) );
  */
}

nomask void show_who()
{
  write(_LANG_WHO_COMMAND + "\n");
  /*
  object who = load_object("/cmds/player/who.c");
  string ret = "";

  if (!who)
  {
    write("Ha habido un error con el comando.\n");
    return;
  }

  ret = who->who_string(79, 0, 0) + "\n\n";
  write( "/global/events.c"->fix_string(ret) );
  */
}

nomask void show_options()
{
  write("\n");
  cat(doc(LOGIN_OPTIONS_MESSAGE));
  write(_LANG_ENTER_AN_OPTION);
  input_to("logon_option");
}

nomask void logon_option(string str)
{
  string * list;
  int tmp;

  if (!strlen(str))
  {
    disconnect();
    return;
  }

  str = lower_case(str);

  // new options, neverbot 02/2006
  list = explode(str, " ");
  if ((sizeof(list) >= 2) && (list[0] == _LANG_FINGER_COMMAND))
  {
    show_finger(list[1]);
    show_options();
    return;
  }

  if (str == _LANG_WHO_COMMAND)
  {
    show_who();
    show_options();
    return;
  }

#ifdef PLAYER_LOCKOUT
  // if (member_array(str, ({ "invitado", "guest" }) != -1)
  // {
  //   write(_LANG_PLAYER_ACCESS_FORBIDDEN);
  //   destruct(_player);
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
    string ret;
    ret = _LANG_ACCOUNT_RECOMMEND;
    ret = sprintf("\n%-*s\n", _player->query_cols(), ret);
    write( ret );

    write(_LANG_ENTER_EMAIL);
    input_to("create_account");
    return;
  }

  // from here we know we are trying to log in
  // with an existing account/character

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
    destruct(_player);
    destruct(this_object());
    return ;
  }
  */

  // TODO guest login
  // if ((str == "invitado") || (str == "guest"))
  //   return guest_login();

  // now we accept emails as accounts names
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
    write(_LANG_INVALID_CHARACTER);
    show_options();
    return;
  }

  // the input is a character name
  if (file_size("/save/players/" + str[0..0] + "/" + str + ".o") > 0)
  {
    restore_character(str);
    write(_LANG_TYPE_CHARACTER_PASSWORD);
    // flag to show it's a character name
    input_to("logon_with_player_name", 1, 0);
  }
  // the input is an account name
  else if (file_size("/save/accounts/" + str[0..0] + "/" + str + ".o") > 0)
  {
    restore_account(str);

    write(_LANG_TYPE_ACCOUNT_PASSWORD);
    input_to("logon_with_account_name", 1);
  }
  else
  {
    write(_LANG_NONEXISTANT_ACCOUNT_OR_CHARACTER);
    show_options();
    return;
  }
}

nomask void logon_with_account_name(string password)
{
  int tmp;
  string * list;

  if (!strlen(password))
  {
    disconnect();
    return;
  }

  if (!account->valid_password(password))
  {
    write(_LANG_WRONG_PASSWORD);

    if (++no_times >= MAX_RETRIES)
    {
      write(_LANG_TOO_MANY_RETRIES);
      destruct(account);
      disconnect(1);
      return;
    }

    write(_LANG_TYPE_ACCOUNT_PASSWORD);
    input_to("logon_with_account_name", 1);
    return;
  }

  // from here the password is ok

  _player->set_account_ob(account);
  account->set_player(_player);

  list = account->query_player_list();

  write(_LANG_AVAILABLE_CHARACTERS_IN_ACCOUNT);

  for (tmp = 0; tmp < sizeof(list); tmp++)
    write("   " + ((string)(tmp + 1)) + ") " + capitalize(list[tmp]) + "\n");

  write(_LANG_CHOOSE_ACCOUNT_CHARACTER);
  input_to("choose_character", 0, list);
} /* logon_with_account_name() */

nomask void choose_character(string str, string * list)
{
  string chosen;
  chosen = "";

  // creating a new player in the account
  if (str == _LANG_CHOOSE_NEW)
  {
    write(_LANG_TYPE_THE_NEW_CHARACTER_NAME);
    input_to("create_player");
    return;
  }

  list = account->query_player_list();

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

  // none of the account characters
  if (chosen == "")
  {
    write(_LANG_UNKNOWN_CHARACTER);
    input_to("choose_character", 0, list);
    return;
  }

  restore_character(chosen);

  // not needed, we already did the password validation
  // write(_LANG_TYPE_CHARACTER_PASSWORD);
  // flag to show we come from the account login
  // input_to("logon_with_player_name", 1, 1);
  logon_with_player_name("", 1);
}

// if flag == 0, login with the player name
// if flag == 1, we come from the account login
nomask void logon_with_player_name(string password, int flag)
{
  if (!account)
  {
    _player->set_account(_player->query_account_name());
    account = _player->query_account();
  }

  // login with the character name
  if (flag == 0)
  {
    // we have to check the password, wasn't check with the account
    if (!account->valid_password(password))
    {
      write(_LANG_WRONG_PASSWORD);

      if (++no_times >= MAX_RETRIES)
      {
        write(_LANG_TOO_MANY_RETRIES);
        destruct(account);
        disconnect(1);
        return;
      }

      write(_LANG_TYPE_CHARACTER_PASSWORD);
      input_to("logon_with_player_name", 1, flag);
      return;
    }
  }

  // from here, we have the account, the character and the password is valid

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
} /* logon_with_player_name() */

nomask void begin(int is_new_player, varargs object destination)
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

  _player->set_name(name);
  _player->set_living_name(name); // both for players and npcs

  // store the user name in the user handler
  find_object(USER_HANDLER)->update_user(_player);

  validated = TRUE;
  write(_LANG_CONNECTED_WELCOME);

  if (query_property(GUEST_PROP))
    _player->add_property(GUEST_PROP, 1);

  // set user privileges
  _player->set_role(_player->query_role_name());

  /* Hamlet stuff */
  if (!coder)
    invis_wish = -1;
  else if ( (invis_wish == 2) && !SECURE->query_admin(name) )
    invis_wish = 1;

  if (!destination)
  {
    // room = load_object(CODER_COMMON_ROOM);
    // _player->move(room);
    _player->move_player_to_start(invis_wish, is_new_player);

    // added by neverbot, this should be here somewhere
    cat(doc("news.txt"));
  }
  else
  {
    _player->move(destination);
    _player->do_look();
  }

  // do not show the first prompt, a command will be issued
  // in move_player_to_start and after that the prompt will be
  // shown
  _player->set_no_prompt();

  destruct(this_object());
} /* begin() */

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

  tell_room(env, _LANG_HAS_RECONNECTED, ({ _player, other_copy }) );
  event(users() - ({ other_copy, 0 }), "inform",
    _player->query_cap_name() + " reconnected", "link-death");

  begin(0, env);
} /* try_throw_out() */









void create_account(string str)
{
  if (!strlen(str))
  {
    // write(_LANG_COME_AGAIN_SOON);
    destruct(_player);
    destruct(this_object());
    return;
  }

  if (!SECURE->valid_email(str))
  {
    write(_LANG_ENTER_VALID_EMAIL);
    input_to("create_account");
    return;
  }

  if (file_size("/save/accounts/" + str[0..0] + "/" + str + ".o") > 0)
  {
    write(_LANG_USED_EMAIL);
    input_to("create_account");
    return ;
  }

  account_name = str;

  write(_LANG_ACCOUNT_NAME_CHOSEN);
  input_to("check_account_name");
}

int check_account_name(string str)
{
  str = implode(explode(str, " "), "");

  if (strlen(str))
  {
    if (member_array(str[0], _LANG_YES_OPTIONS_ARRAY) != -1)
    {
      write(_LANG_DEFINE_ACCOUNT_PASSWORD);
      input_to("create_account2", 1);
      return 1;
    }
    else if (member_array(str[0], _LANG_NO_OPTIONS_ARRAY) != -1)
    {
      write(_LANG_TRY_AGAIN);
      write(_LANG_ENTER_VALID_EMAIL);
      input_to("create_account");
      return 1;
    }
  }

  write(_LANG_ANSWER_YES_NO);
  input_to("check_account_name");
  return 1;
} /* check_account_name() */

void create_account2(string str)
{
  if (!strlen(str))
  {
    // write(_LANG_COME_AGAIN_SOON);
    destruct(_player);
    destruct(this_object());
    return ;
  }

  password = crypt(str, password);

  write(_LANG_DEFINE_ACCOUNT_PASSWORD_REPEAT);
  input_to("create_account3", 1);
}

void create_account3(string str)
{
  string tmppasswd;

  if (!strlen(str))
  {
    write(_LANG_ACCOUNT_PASSWORD_REMOVED + " " + _LANG_ACCOUNT_REPEAT + "\n"+
      _LANG_DEFINE_ACCOUNT_PASSWORD);
    input_to("create_account2", 1);
    return;
  }

  tmppasswd = crypt(str, password);

  if (password != tmppasswd)
  {
    write(_LANG_ACCOUNT_DIFFERENT_PASSWORDS + " " + _LANG_ACCOUNT_REPEAT + "\n"+
      _LANG_DEFINE_ACCOUNT_PASSWORD);
    input_to("create_account2", 1);
    return;
  }

  create_account4();
}

void create_account4()
{
  // create the account object
  account = clone_object(ACCOUNT_OB);
  account->set_account_name(account_name);
  account->set_password(password);
  // account->add_player(name);
  account->update_last_connection();
  account->save_me();

  write(_LANG_NEW_ACCOUNT_CREATED);

  _player->set_account_ob(account);

  // from here the account object will take control of inputs
  account->logon(_player, TRUE); // flag = new account
  return;
}

void create_player(string str)
{
  int tmp;

  if ((tmp = SECURE->valid_user_name(str)) != -1)
  {
    write("Caracter inválido '"+str[tmp..tmp]+"' en ("+str+").\n\n");
    write("Introduce el nombre de tu nuevo personaje: ");
    input_to("create_player");
    return;
  }

  if (strlen(str) > MAX_LEN)
  {
    write("El nombre es demasiado largo, el máximo son 11 caracteres.\n");
    write("Introduce el nombre de tu nuevo personaje: ");
    input_to("create_player");
    return;
  }

  if (strlen(str) < MIN_LEN)
  {
    write("El nombre es demasiado corto, el mínimo son 3 caracteres.\n");
    write("Introduce el nombre de tu nuevo personaje: ");
    input_to("create_player");
    return;
  }

  if ((file_size("/save/players/"+str[0..0]+"/"+str+".o") > 0) ||
      (file_size("/save/accounts/"+str[0..0]+"/"+str+".o") > 0))
  {
    write("Lo sentimos, pero ese nombre ya está utilizado.\n");
    write("Por favor, introduce otro nombre: ");
    input_to("create_player");
    return ;
  }

  name = str;

  write("Has escogido como nombre: '"+name+"', ¿estás seguro? (s/n): ");
  input_to("check_player_name");
}

int check_player_name(string str)
{
  str = implode(explode(str," "),"");

  if ((str[0] == 's') || (str[0] == 'S'))
  {
    write("¿Tu personaje es hombre o mujer? (h/m): ");
    input_to("get_sex");
    return 1;
  }
  else if ((str[0] == 'n') || (str[0] == 'N'))
  {
    write(_LANG_TRY_AGAIN);
    write("Introduce el nombre de tu personaje: ");
    input_to("create_player");
    return 1;
  }

  write(_LANG_ANSWER_YES_NO);
  input_to("check_player_name");
  return 1;

} /* check_player_name() */

void get_sex(string str)
{
  int i;
  str = lower_case(str);

  if (!strlen(str))
  {
    write("Inténtalo con hombre o mujer (h/m): ");
    input_to("get_sex");
    return;
  }

  i = str[0];

  if ((i == 'h') || (i == 'H'))
  {
    _player->set_gender(1);
    gender = 1;
  }
  else if ((i== 'm')  || (i == 'M'))
  {
    _player->set_gender(2);
    gender = 2;
  }
  else
  {
    write("Inténtalo con hombre o mujer (h/m): ");
    input_to("get_sex");
    return;
  }

  if (!query_property(GUEST_PROP))
  {
    _player->save_me();
    account->add_player(name);
  }

  begin(!query_property(GUEST_PROP));
}

mixed * stats()
{
  return ({
    ({ "Login validated", validated, }),
          });
}


