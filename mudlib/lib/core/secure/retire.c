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
#include <living/living.h>
#include <item/board.h>

string password;
int no_times;

nomask int put_password(string str);
nomask int do_retirejob(string name);
static int remove_player_dir(string dir);

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
//   // if (!restore_object(player_save_dir(name) + "player.o",1))
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

/**
 * Removes a directory with everything inside it. DGD's remove_dir only
 * accepts empty directories, so files and subfolders go first.
 *
 * @param dir directory path, ending in a slash
 * @return 1 when the directory is gone, 0 otherwise
 */
static int remove_player_dir(string dir)
{
  mixed * entries;
  int i;

  entries = get_dir(dir, -1);

  for (i = 0; i < sizeof(entries); i++)
  {
    if (entries[i][1] == -2)
      remove_player_dir(dir + entries[i][0] + "/");
    else
      rm(dir + entries[i][0]);
  }

  return rmdir(dir);
}

/**
 * The deleting of the player. Root access is needed to remove the files, so
 * it lives in the master.
 *
 * @param name name of the character to delete
 * @return 0 always
 */
nomask int do_retirejob(string name)
{
  object u, * games;
  string dir;
  int i;

  // the initiating user
  u = initial_object();

  // security
  if (!u->player() || u->player()->query_name() != name)
    return 0;

  // remove the character from the account
  u->remove_player(name);
  u->update_last_connection();

  // last but not least, remove the whole player folder (player.o, exploration,
  // mounts and anything else saved there)
  // the folder must be this character's own, never a parent of other folders
  dir = player_save_dir(name);
  if (strlen(name) && strlen(dir) > strlen(name) + 1 &&
      dir[strlen(dir) - strlen(name) - 2 ..] == "/" + name + "/")
    remove_player_dir(dir);

  write(_LANG_RETIRE_OK);
  this_player()->add_property(GUEST_PROP, 1);

  // remove the mails
  // May make problem if the player don't have a mail.o file..
  // rm ("/save/post/" + name + ".o");

  // Maybe THIS will work, and actually free the disk space - Radix
  POSTAL_D->retire_user(name);

  // and the player killing records
  "/lib/handlers/pk"->retire_user(name);

  // the mud-wide records kept outside the player folder
  catch(RANKING_OB->retire_user(name));
  catch(BOARD_HAND->retire_user(name));

  // the records of each game: a game's own handler keeps its own file, and
  // one that has none answers with the shared handler of the mudlib
  games = (object *)handler("games")->query_game_objects();
  for (i = 0; i < sizeof(games); i++)
  {
    catch(handler("crimes", games[i])->retire_user(name));
    catch(handler("deities", games[i])->retire_user(name));
    catch(handler("families", games[i])->retire_user(name));
  }

  write(_LANG_RETIRE_GUEST_MSG);

  /* Wonerflug1997, adding a log til someone fixes the bug */
  log_file("retire", this_player()->query_cap_name() + " has retired the character, "+
    ctime(time(), 4) + ".\n");

  return 0;
} /* do_retire */

int query_prevent_shadow() { return 1; }

