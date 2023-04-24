
#include <mud/cmd.h>
#include <status.h>

inherit CMD_BASE;

void setup()
{
  set_aliases(({ "malloc" }));
  set_usage("malloc");
  set_help("Shows info about used memory.");
}

static int cmd(string str, object me, string verb) 
{
  // write(malloc_status());
  mixed * info;

  info = status();

  write(sprintf("static memory allocated: %d\n" + 
                "static memory in use: %d\n" + 
                "dynamic memory allocated: %d\n" +
                "dynamic memory in use: %d\n", 
                info[ST_SMEMSIZE], info[ST_SMEMUSED], info[ST_DMEMSIZE], info[ST_DMEMUSED]
                ));

  return 1;
}


