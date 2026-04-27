#include <std.h>
#include <mud/cmd.h>
inherit CMD_BASE;

void setup()
{
position = 1;
}

static int cmd(string str, object me, string verb) {
  write("status: not yet reimplemented (depends on missing mud_status() kfun).\n");
  return 1;

/* Original body kept for reference:
  if (str)
  write(mud_status(1));
  else
   write(mud_status());
  return 1;
*/
} /* do_status() */
 
