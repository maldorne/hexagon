#include <std.h>
#include <mud/cmd.h>
inherit CMD_BASE;

void setup()
{
position = 1;
}


static int cmd(string str, object me, string verb) {
  tell_object(me,dump_file_descriptors());
  return 1;
} /* do_fds() */
 
