
#include <mud/cmd.h>

inherit CMD_BASE;

void setup()
{
  set_aliases(({ "rm" }));
  set_usage("rm <file>");
  set_help("Removes given file or files.");
}

static int cmd(string str, object me, string verb)
{
  int fsize, loop;
  string *filenames;

  if (this_player(1) != this_user())
    return 0;

  if (!strlen(str))
  {
    write("Syntax: " + query_usage() + "\n");
    return 1;
  }

  filenames = get_files(str);

  if (!sizeof(filenames))
  {
    notify_fail("The file '" + str + "' does not exist.\n");
    return 0;
  }

  for(loop = 0; loop < sizeof(filenames); loop++)
  {
    string temp, temp2;

    temp2 = "";
    str = filenames[loop];

    if((sscanf(str, "%s/.%s", temp, temp2) && (temp2 == "")) ||
        sscanf(str, "%s/..%s", temp, temp2) && (temp2 == "") )
    {
      continue;
    }

    fsize = file_size(str);

    if (fsize == -1)
    {
       notify_fail("File or directory does not exist.\n");
       return 0;
    }

    if (fsize == -2)
    {
      if (!rmdir(str))
      {
        notify_fail("Unable to remove directory: '" + str + "'.\n");
        return 0;
      }
    }
    else if (!rm(str))
    {
      notify_fail("Unable to remove the file.\n");
      return 0;
    }
  }

  write("Ok.\n");
  return 1;
} /* rm_files() */

