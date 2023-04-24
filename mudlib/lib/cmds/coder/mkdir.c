
#include <mud/cmd.h>

inherit CMD_BASE;

void setup()
{
  set_aliases(({ "mkdir" }));
  set_usage("mkdir <directory>");
  set_help("Creates a directory.");
}

static int cmd(string str, object me, string verb) 
{
  if (!strlen(str)) 
  {
    notify_fail("Make what directory?\n");
    return 0;
  }
  
  str = get_path(str);

  if (!str)
     return 1;

  if (file_size(str) != -1) 
  {
    notify_fail(str + " already exists.\n");
    return 0;
  }

  if (!mkdir(str)) 
  {
    notify_fail("Couldn't make dir.\n");
    return 0;
  }
  
  write("Ok, directory created.\n");
  return 1;
}
 