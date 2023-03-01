// /adm/daemon/letter_d.c
// from the Nightmare Mudlib
// handles the management of postal letters
// created by Descartes of Borg 04 august 1993
// 93-08-12: euid bug fixed by Pallando for Darkone.

#include <mud/secure.h>
#include <files/postal.h>

string * undeleted;
string letter;
static string * mail_ls, * pl_ls;
static int mail_ptr, pl_ptr;

void manage_mail();
void manage_users();
nomask static int valid_access();

void create()
{
  seteuid(ROOT);
  letter = "";
  undeleted = ({});
  mail_ls = get_dir(DIR_LETTERS + "/");

  if (pointerp(pl_ls = get_dir(DIR_POSTAL + "/")))
    pl_ls = pl_ls - ({ "mail-queue.o" });
  else 
    pl_ls = ({});
  
  mail_ptr = 0;
  pl_ptr = 0;
  manage_mail();
  call_out("manage_users", 60);
}

string create_message(string str, string * who)
{
  int tmp;
  string id;

  if (!valid_access()) 
    return "";

  id = (tmp = time()) + "";

  while (file_size(DIR_LETTERS + "/" + id + ".o") != -1)
    id = (--tmp) + "";

  letter = str;
  undeleted = who;
  save_object(DIR_LETTERS + "/" + id);
  return id;
}

string get_message(string id)
{
  if (!valid_access()) 
    return "";

  undeleted = ({});
  letter = "";

  if (file_size(DIR_LETTERS + "/" + id + ".o") < 0) 
    return "Invalid message.\n";

  restore_object(DIR_LETTERS + "/" + id);
  return letter;
}

void delete_message(string id, string who)
{
  if (!valid_access()) 
    return;

  undeleted = ({});
  letter = "";

  if (file_size(DIR_LETTERS + "/" + id + ".o") < 0) 
    return;

  restore_object(DIR_LETTERS + "/" + id);
  undeleted -= ({ who });

  if (!sizeof(undeleted))
    rm(DIR_LETTERS + "/" + id + ".o");
  else 
    save_object(DIR_LETTERS + "/" + id);
}

void manage_mail()
{
  string str, ext;
  string * tmp;
  int x;

  if (!sizeof(mail_ls)) 
    return;

  if (mail_ptr >= sizeof(mail_ls)) 
    mail_ptr = 0;

  if (!file_exists(DIR_LETTERS + "/" + mail_ls[mail_ptr]))
  {
    if (++mail_ptr >= sizeof(mail_ls)) mail_ptr = 0;
    return;
  }

  sscanf(mail_ls[mail_ptr], "%s.%s", str, ext);

  if (!sscanf(str, "%d", x))
  {
    if (++mail_ptr >= sizeof(mail_ls)) 
      mail_ptr = 0;
    return;
  }

  restore_object(DIR_LETTERS + "/" + str);
  x = sizeof(undeleted);
  tmp = ({});

  while (x--)
    if (user_exists(undeleted[x])) 
      tmp += ({ undeleted[x] });

  if (!sizeof(undeleted = tmp))
  {
    rm(DIR_LETTERS + "/" + mail_ls[mail_ptr]);
    return;
  } else 
    save_object(DIR_LETTERS + "/" + str);

  if (++mail_ptr >= sizeof(mail_ls)) 
    mail_ptr = 0;

  call_out("manage_mail", 300);
}

void manage_users()
{
  string pl, ext;

  if (!sizeof(pl_ls)) 
    return;

  if (pl_ptr >= sizeof(pl_ls)) 
    pl_ptr = 0;

  if (!file_exists(DIR_POSTAL + "/" + pl_ls[pl_ptr]))
  {
    if (++pl_ptr >= sizeof(pl_ls)) 
      pl_ptr = 0;
    return;
  }

  sscanf(pl_ls[pl_ptr], "%s.%s", pl, ext);

  if (!user_exists(pl)) 
    rm(DIR_POSTAL + "/" + pl + ".o");

  if (++pl_ptr >= sizeof(pl_ls)) 
    pl_ptr = 0;

  call_out("manage_users", 300);
}

nomask static int valid_access()
{
  if (member_array(base_name(previous_object()), TRUSTED_MAILERS) == -1 &&
    geteuid(previous_object()) != ROOT) return 0;
  return 1;
}