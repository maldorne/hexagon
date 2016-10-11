// old content of /global/path.c 
// neverbot 3/2009

static string home_dir;
static string current_path;

// prototypes
object *wiz_present(string str, varargs object onobj, int nogoout);


void create()
{
  home_dir = "";
  current_path = "";
}

static void role_commands()
{
  add_action("what_dir", "pwd");
  add_action("change_dir", "cd");
}

string query_home_dir() { return home_dir; }
string query_current_path() { return current_path; }
string query_path() { return current_path; }

int what_dir(string str) 
{
  write("Directorio actual: '"+current_path+"'.\n");
  return 1;
}

// Radix - Added wiz_present call to 'cd' allowing 'cd here'
// or any other object, same as 'ed'
// December 15, 1995
int change_dir(string str)
{
  string *filenames;
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
      notify_fail("No tienes homedir. Usa 'homedir <directorio>' para establecerlo.\n");
      return 0;
    }

    str = home_dir;
  }

  filenames = get_files(str);

  if (sizeof(filenames) > 1) 
  {
    notify_fail("Directorio ambiguo.\n");
    return 0;
  }

  if (!sizeof(filenames))
  {
    obs = wiz_present(str, this_player());

    if (!sizeof(obs))
    {
      notify_fail("Directorio inexistente.\n");
      return 0;
    }

    // Added 'cd <immortal>' changes your path to their path
    // Radix
    if (interactive(obs[0]) && obs[0]->query_coder())
      filenames = get_files(obs[0]->query_path());
    // else
    // {
    //   tmp = virtual_file_name(obs[0]);
    //   if (!stringp(tmp))
    //   {
    //     notify_fail("Error: call a wiz_present erróneo, cd imposible.\n");
    //     return 0;
    //   }
    //   arr = explode(tmp,"/");
    //   str = implode(arr[0..sizeof(arr)-2],"/");
    //   filenames = get_files("/"+str);
    // }

    // Unlikely a loaded object won't have a dir, but leave it
    // to a bastard to do such things...
    // Radix
    if (!sizeof(filenames))
    {
      notify_fail("Directorio inexistente.\n");
      return 0;
    }
  }

  str = filenames[0];
  
  if (file_size(str) != -2)
    write("Directorio erróneo: '" + str + "'.\n");
  else
    current_path = str;

  write(current_path+"\n");
  return 1;
}

mixed stats() 
{
  return ({ 
          ({"Home dir", home_dir, }),
          ({"Current Path (nosave)", current_path, }),
          });
}
            
