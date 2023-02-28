// causes command handler to rehash

#include <mud/cmd.h>

inherit CMD_BASE;

string query_usage()
{
  return "rehash [-v]";
}

string query_help()
{
  return "Causes command handler to rehash.\n" +
         "   -v  to use verbose mode.\n";
}

static int cmd(string tail, object me, string verb)
{
  int count, verbose;

  if (!stringp(tail) || !tail)
    tail = "";

  verbose = 0;

  if (member_array("-v", explode(tail, " ")) != -1)
    verbose = 1;

  write("Rehashing command handler...\n");

  count = (int)CMD_HANDLER->cmd_make_hash(verbose);

  write("Command handler hashed "+count+" commands.\n");
  return 1;
}
