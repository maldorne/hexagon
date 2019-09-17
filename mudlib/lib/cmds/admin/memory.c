
#include <mud/cmd.h>
#include <mud/secure.h>

inherit CMD_BASE;

void setup()
{
  position = 1;
}

static int cmd(string str, object me, string verb) 
{
  write(memory_status() + "\n");
  return 1;
}
