
#include <mud/cmd.h>
#include <language.h>

inherit CMD_BASE;

void setup()
{
  set_aliases(({ "cat" }));
  set_usage("cat <file>");
  set_help("Shows the contents of given file.");
}

static int cmd(string str, object me, string verb)
{
  string *filenames;
  int loop;

  if (!strlen(str))
  {
    write("Syntax: " + query_usage() + "\n");
    return 1;
  }

  if (str == "*")
  {
    notify_fail("cat *? Forget about it.\n");
    return 0;
  }

  filenames = get_files(str);

  if (!sizeof(filenames))
  {
    notify_fail(str + ": That file does not exist.\n");
    return 0;
  }

  for (loop = 0; loop < sizeof(filenames); loop++)
  {
    int result;
    str = filenames[loop];

    if (sizeof(filenames) > 1)
     write("File: " + str + "\n\n");

    if (file_size(str) < 8196)
    {
      result = cat(str);
      if (!result)
        write("Cannot read that file.\n");
    }
    else
    {
      write(read_bytes(str, 0, 8195));
      write("\n ----- TRUNCATED ----- \n\n");
    }
  }

  return 1;
}
