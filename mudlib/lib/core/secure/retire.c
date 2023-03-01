/* The first attempt on a Retire command.
 * It will make the players able to delete their own charachters.
 * Baldrick march '94
 * Put it in /secure/master and add it to master.c (#include).
 * Add an add_action and a call in player.c (test on force!)
 * Moved to /cmds and translated by neverbot 4/2003
 * Here we only do the security checks
 */

#include <files/postal.h>
#include <basic/money.h>
// #include <user/user.h>
#include <user/player.h>
#include <language.h>

// #define BANK_HANDLER "/obj/handlers/bank_handler"

string password;
int no_times;

nomask int put_password(string str);
nomask int do_retirejob(string name);

int try_retire(object who)
{
  if (who->query_coder())
  {
    notify_fail(_LANG_TRY_RETIRE_CODER);
    return 0;
  }

  tell_object(who, _LANG_TRY_RETIRE_DELETE_WARNING + 
    _LANG_TRY_RETIRE_BE_CAREFUL +
    _LANG_TRY_RETIRE_INPUT_PASSWORD);

  input_to("put_password", 1);
  return 1;
}

// int test_password(string name, string pass)
// {
//   string tmp;
//   string account_name;

//   account_name = find_player(name)->query_account_name();

//   if (!stringp(name) || strlen(name) < 2 || sscanf(name, "%s %s", tmp, tmp)
//     || name[0] == '.' || sscanf(name, "%s..%s", tmp,tmp))
//     return 0;

//   // now with accounts, neverbot 12/2010
//   // if (!restore_object("/save/players/"+name[0..0]+"/"+name,1))
//   if (!restore_object("/save/accounts/" + account_name[0..0] + "/" + account_name))
//     return 0;

//   return crypt(pass, password) == password;
// } /* test_password() */

nomask int put_password(string str)
{
  string playername;

  playername = this_player()->query_name();

  if (!strlen(str))
  {
    write(_LANG_RETIRE_NO_PASSWORD);
    return 0;
  }

  // if (!test_password(playername, str))
  if (!initial_object()->valid_password(str))
  {
    write(_LANG_RETIRE_WRONG_PASSWORD);
    return 0;
  }

  do_retirejob(playername);
  return 1;
} /* put_password() */

/* The deleting of the player. */
/* have to get root access to be able to do the RM of the file. */
/* Done by putting the whole thingie in the master. */

nomask int do_retirejob(string name)
{
  object u;

  // the initiating user
  u = initial_object();

  // security
  if (!u->player() || u->player()->query_name() != name)
    return 0;

  // remove exploration data
  if (file_size("/save/explorers/" + name[0..0] + "/" + name + ".o"))
    rm ("/save/explorers/" + name[0..0] + "/" + name + ".o");

  // remove mount
  if (file_size("/save/mounts/" + name[0..0] + "/" + name + ".o"))
    rm ("/save/mounts/" + name[0..0] + "/" + name + ".o");

  // remove the character from the account
  u->remove_player(name);
  u->update_last_connection();

  // last but not least, remove the character
  rm("/save/players/" + name[0..0] + "/" + name + ".o");

  write(_LANG_RETIRE_OK);
  this_player()->add_property(GUEST_PROP, 1);

  // remove the mails
  // May make problem if the player don't have a mail.o file..
  // rm ("/save/post/" + name + ".o");

  // Maybe THIS will work, and actually free the disk space - Radix
  POSTAL_D->retire_user(name);

  // Hmm.. should add a delete of bank accounts, think i have some code.
  // WHY does the BANK_HANDLER thingie work here and not in bank.c ?
  // BANK_HANDLER->refresh_account(name);

  write(_LANG_RETIRE_GUEST_MSG);

  /* Wonerflug1997, adding a log til someone fixes the bug */
  log_file("retire", this_player()->query_cap_name() + " has retired the character, "+
    ctime(time(), 4) + ".\n");

  return 0;
} /* do_retire */

int query_prevent_shadow() { return 1; }

