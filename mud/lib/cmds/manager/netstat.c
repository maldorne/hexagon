#include <standard.h>
#include <cmd.h>
inherit CMD_BASE;
#define PEOPLER "/obj/handlers/peopler"

void setup()
{
position = 1;
}

protected int cmd(string str, object me, string verb) {
  return (int)PEOPLER->do_netstat(str);
} /* do_people() */

