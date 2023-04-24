
#include <mud/cmd.h>
#include <translations/common.h>

inherit CMD_BASE;

void setup()
{
  set_aliases(({ "mv" }));
  set_usage("mv <file> <file|dir>");
  set_help("Moves a file to a different file (renaming) or directory (moving).");
}

static int cmd(string str, object me, string verb)
{
  string *filenames, dest, *fnames;
  int loop, fs;

  if (this_player(1) != this_user())
    return 0;

  if (!strlen(str))
  {
    notify_fail(_LANG_SYNTAX + ": " + query_usage() + "\n");
    return 0;
  }

  fnames = explode(str, " ");
  filenames = get_files(implode(fnames[0..sizeof(fnames)-2], " "));

  if (!sizeof(filenames))
  {
    notify_fail(_LANG_SYNTAX + ": " + query_usage() + "\n");
    return 0;
  }

  dest = fnames[sizeof(fnames) - 1];
  dest = get_path(dest);

  if (!dest)
  {
     write("Destination do not exist.\n");
     return 1;
  }

  for(loop = 0; loop < sizeof(filenames); loop++)
  {
    str = filenames[loop];

    if (file_size(str) == -1)
    {
      write("No such file: '" + str + "'.\n");
      continue;
    }

    fs = file_size(dest);

    if (fs == -2)
    {
      string *names;

      names = explode(str, "/");
      fs = file_size(dest + "/" + names[sizeof(names) - 1]);

      if (fs != -1)
      {
        write("File already exists: " + dest + "/" + names[sizeof(names) - 1] + "\n");
        continue;
      }

      rename(str, dest + "/" + names[sizeof(names) - 1]);
    }

    else
    {
      if (fs != -1)
      {
        write("File already exists: " + dest + "\n");
        continue;
      }

      rename(str, dest);
    }
  }

  write("Ok, file moved.\n");
  return 1;
}
