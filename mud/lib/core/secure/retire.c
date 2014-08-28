/* The first attempt on a Retire command.
 * It will make the players able to delete their own charachters. 
 * Baldrick march '94 
 * Put it in /secure/master and add it to master.c (#include).
 * Add an add_action and a call in player.c (test on force!)
 * Moved to /cmds and translated by neverbot 4/2003
 * Here we only do the security checks
 */

#include <post.h>
#include <money.h>
#include <player.h>

#define MAX_RETRYS 1

// #define BANK_HANDLER "/obj/handlers/bank_handler"
// #define POSTAL_D "/obj/handlers/postal_d"

string password;
int no_times;

static int put_password(string str);
static int do_retirejob(string name);

int try_retire(object who) 
{
  if(who->query_coder())
  {
    notify_fail("Eres un programador, no puedes retirarte de este "+
      "modo. Pregúntale a un administrador.\n");
    return 0;
  }

  tell_player(who, "Esto borrará el personaje con el que estás jugando.\n");
  tell_object(who, "Asegúrate de saber lo que estás haciendo.\n" +
    "No introduzcas la contraseña si te arrepientes.\n\n");
  tell_object(who, "Introduce la contraseña de tu cuenta > ");

  input_to("put_password", 1);
  return 1;
} 

int test_password(string name, string pass) 
{
  string tmp;
  string account_name;

  account_name = find_player(name)->query_account_name();

  if (!stringp(name) || strlen(name) < 2 || sscanf(name, "%s %s", tmp, tmp)
    || name[0] == '.' || sscanf(name, "%s..%s", tmp,tmp))
    return 0;
  
  // now with accounts, neverbot 12/2010  
  // if (!restore_object("/save/players/"+name[0..0]+"/"+name,1))
  if (!restore_object("/save/accounts/"+account_name[0..0]+"/"+account_name))
    return 0;
    
  return crypt(pass, password) == password;
} /* test_password() */

static int put_password(string str) 
{
    // string bing;
  string playername;

  playername = this_player()->query_name();
  
  if (!str || str == "") 
  {
    write("Sin contraseña no puedes retirarte.\n");
    return 0;
  } 
  
  if (!test_password(playername, str))
  {
    write("Contraseña equivocada, no puedes retirarte.\n");
    return 0;
  } 
  
  do_retirejob(playername);
  return 1;
} /* put_password() */

/* The deleting of the player. */
/* have to get root access to be able to do the RM of the file. */
/* Done by putting the whole thingie in the master. */

static int do_retirejob(string name)
{
  object acc;
  
  // remove exploration data
  if (file_size("/save/explorers/" + name[0..0] + "/" + name + ".o"))
    rm ("/save/explorers/" + name[0..0] + "/" + name + ".o");

  // remove mount
  if (file_size("/save/mounts/" + name[0..0] + "/" + name + ".o"))
    rm ("/save/mounts/" + name[0..0] + "/" + name + ".o");

  // remove the character from the account
  acc = clone_object("/global/account.c");
  acc->restore_me(this_player()->query_account_name());
  acc->remove_player(name);
  acc->update_last_connection();
  
  // last but not least, remove the character
  rm ("/save/players/" + name[0..0] + "/" + name + ".o");

  write ("Ok, has borrado tu ficha, " + name + ".\n");
  this_player()->add_property(GUEST_PROP, 1);

  /* remove the mails */
  /* May make problem if the player don't have a mail.o file.. */
  // rm ("/save/post/" + name + ".o");

  // Maybe THIS will work, and actually free the disk space - Radix
  POSTAL_D->retire_user(name);

  /* Hmm.. should add a delete of bank accounts, think i have some code. */
  /* WHY does the BANK_HANDLER thingie work here and not in bank.c ? */
  // BANK_HANDLER->refresh_account(name);

  write("Ahora eres un invitado.\n"+
    "Sal del juego para eliminar definitivamente el personaje.\n");
    
  /* Wonerflug1997, adding a log til someone fixes the bug */
  log_file("retirar", this_player()->query_cap_name()+" se ha retirado, "+
    ctime(time(), 4)+".\n");
    
  return 0;
} /* do_retire */

int query_prevent_shadow() { return 1; }

