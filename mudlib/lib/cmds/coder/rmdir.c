
#include <mud/cmd.h>
#include <translations/common.h>

inherit CMD_BASE;

void setup()
{
  set_aliases(({ "rmdir" }));
  set_usage("rmdir <directory>");
  set_help("Removes a directory.");
}

static int cmd(string str, object me, string verb) 
{
  string *filenames;
  int fsize, loop;

  if (!strlen(str)) 
  {
    notify_fail(_LANG_SYNTAX + ": " + query_usage() + "\n");
    return 0;
  }

  filenames = get_files(str);

  if (!sizeof(filenames)) 
  {
    notify_fail("Directory does not exist: '" + str + "'.\n");
    return 0;
  }

  for (loop = 0; loop < sizeof(filenames); loop++) 
  {
    str = filenames[loop];
    fsize = file_size(str);

    if (fsize == -1) 
    {
      notify_fail("Directory does not exist: '" + str + "'.\n");
      return 0;
    }

    if (fsize != -2) 
    {
      notify_fail(str + " is not a directory.\n");
      return 0;
    }

    if (!rmdir(str)) 
    {
      notify_fail("Cannot remove directory: '" + str + "'.\n");
      return 0;
    }
  }

  write("Ok, directory removed.\n");
  return 1;
}
