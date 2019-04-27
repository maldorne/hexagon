// old content of /global/path.c
// neverbot 3/2009

#include <language.h>

static string home_dir;
static string current_path;

// prototypes
object *wiz_present(string str, object onobj, varargs int nogoout);


void create()
{
  home_dir = "";
  current_path = "";
}

static void start_role(object player)
{
  if (player)
    home_dir = "/home/" + player->query_name();
  else
    home_dir = "/";

  current_path = home_dir;
}

static void role_commands()
{
  add_action("what_dir", "pwd");
  add_action("change_dir", "cd");
  add_action("set_home_dir", "homedir");
}

string query_home_dir() { return home_dir; }
string query_current_path() { return current_path; }
string query_path() { return current_path; }

int what_dir(string str)
{
  write(_LANG_CURRENT_DIR + ": '"+current_path+"'.\n");
  return 1;
}

// Radix - Added wiz_present call to 'cd' allowing 'cd here'
// or any other object, same as 'ed'
// December 15, 1995
int change_dir(string str)
{
  string *dirnames, *filenames;
  object *obs;
  string tmp;
  string *arr;

  obs = ({ });
  arr = ({ });
  tmp = "";

  if (this_player(1) != this_player())
    return 0;

  if (!strlen(str))
  {
    if (!strlen(home_dir))
    {
      notify_fail(_LANG_NO_HOMEDIR);
      return 0;
    }

    str = home_dir;
  }

  // no ambiguities in 'cd ..', ever
  if (str == "..") {
    dirnames = ({ get_path(str) });
  } else {
    dirnames = get_files(str);
    // fix to avoid "ambiguous directory" messages
    filenames = get_cfiles(str);

    if (sizeof(filenames)) {
      dirnames = dirnames - filenames;
    }
  }

  if (sizeof(dirnames) > 1)
  {
    notify_fail(_LANG_AMBIGUOUS_DIRECTORY);
    return 0;
  }

  if (!sizeof(dirnames))
  {
    obs = wiz_present(str, this_player());

    if (!sizeof(obs))
    {
      notify_fail(_LANG_NONEXISTANT_DIRECTORY);
      return 0;
    }

    // Added 'cd <immortal>' changes your path to their path
    // Radix
    if (interactive(obs[0]) && obs[0]->query_coder())
      dirnames = get_files(obs[0]->query_path());
    else
    {
      tmp = base_name(obs[0]);

      if (!stringp(tmp))
      {
        notify_fail(_LANG_CD_ERROR_WIZ);
        return 0;
      }

      arr = explode(tmp, "/");
      str = implode(arr[0..sizeof(arr)-2], "/");
      dirnames = get_files("/" + str);
    }

    // Unlikely a loaded object won't have a dir, but leave it
    // to a bastard to do such things...
    // Radix
    if (!sizeof(dirnames))
    {
      notify_fail(_LANG_NONEXISTANT_DIRECTORY);
      return 0;
    }
  }

  str = dirnames[0];

  if (file_size(str) != -2)
    write(_LANG_WRONG_DIR + ": '" + str + "'.\n");
  else
    current_path = str;

  write(current_path+"\n");
  return 1;
}

int set_home_dir(string str)
{
  if (this_player(1) != this_object()->query_player())
    return 0;

  if (str)
    home_dir = get_path(str);

  write(_LANG_HOMEDIR_SET);
  return 1;
}

mixed stats()
{
  return ({
          ({"(role) Home Dir", home_dir, }),
          ({"(role) Current Path", current_path, }),
          });
}

