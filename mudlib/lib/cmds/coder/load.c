
#include <mud/cmd.h>

inherit CMD_BASE;

void setup()
{
  set_aliases(({ "load" }));
  set_usage("load <file>");
  set_help("Loads in memory the specified file or files.");
}

static int cmd(string str, object me, string verb)
{
  string *filenames, err;
  int loop;

  if (!strlen(str))
  {
    notify_fail("Load what?\n");
    return 0;
  }

  filenames = get_cfiles(str);

  if (!sizeof(filenames))
  {
    notify_fail("File does not exist.\n");
    return 0;
  }

  for (loop = 0; loop < sizeof(filenames); loop++)
  {
    str = filenames[loop];

    if (file_size(str)<0)
    {
      write(str + ": File does not exist.\n");
      continue;
    }

    if (err = catch(str->load_up_with_yellow()))
      write("Could not load file '"+str+"'.\nError: "+err+"\n");
    else
      write("File '"+str+"' loaded.\n");
  }

  return 1;
}
 
