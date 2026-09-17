
#include <mud/cmd.h>
#include <mud/secure.h>

inherit CMD_BASE;

void setup()
{
  set_aliases(({ "memory" }));
  set_usage("memory");
  set_help("How much memory the driver has taken from the system and how much of\n" +
           "it is in use, both in the static and the dynamic pools.");
  position = 1;
}

static int cmd(string str, object me, string verb) 
{
  write(memory_status() + "\n");
  return 1;
}
