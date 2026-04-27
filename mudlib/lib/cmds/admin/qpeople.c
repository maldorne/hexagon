#include <std.h>
#include <mud/cmd.h>
inherit CMD_BASE;
#define PEOPLER "/obj/handlers/peopler"

void setup()
{
position = 1;
}

static int cmd(string str, object me, string verb) {
  return (int)PEOPLER->do_qpeople(str);
} /* do_people() */


