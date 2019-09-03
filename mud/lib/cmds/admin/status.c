#include <standard.h>
#include <cmd.h>
inherit CMD_BASE;

void setup()
{
position = 1;
}

protected int cmd(string str, object me, string verb) {
  if (str)
  write(mud_status(1));
  else
   write(mud_status());
  return 1;
} /* do_status() */
 
