
#include <mud/cmd.h>
#include <translations/common.h>

inherit CMD_BASE;

void setup()
{
  set_aliases(({ "cp" }));
  set_usage("cp <file> <destination>");
  set_help("Copies a file to another location.");
}

static int cmd(string str, object me, string verb)
{
  string *filenames, text, dest, *fnames;
  int loop, fs;

  if (this_player(1) != this_user())
    return 0;

  if (!strlen(str))
  {
    notify_fail(_LANG_SYNTAX + ": " + query_usage() + "\n");
    return 0;
  }

  fnames = explode(str, " ");
  filenames = get_files(implode(fnames[0..sizeof(fnames) -2], "/"));

  if (!sizeof(filenames))
  {
    notify_fail(_LANG_SYNTAX + ": " + query_usage() + "\n");
    return 0;
  }

  dest = fnames[sizeof(fnames) - 1];
  dest = get_path(dest);

  if (!dest)
  {
    write("You need to specify a destination.\n");
    return 1;
  }

  for (loop = 0; loop < sizeof(filenames); loop++)
  {
    str = filenames[loop];
    text = read_file(str);
    if (!text)
    {
      write("File does not exist: " + str + "\n");
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
        write("File already exists: " + dest + "/" +
          names[sizeof(names) - 1] + "\n");
        continue;
      }

      write_file(dest + "/" + names[sizeof(names) - 1], text);
    }
    else
    {
      if (fs != -1)
      {
        write("File already exists: " + dest + "\n");
        continue;
      }

      write_file(dest, text);
    }
  }

  write("File copied.\n");
  return 1;
}


