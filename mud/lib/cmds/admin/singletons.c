
#include <kernel.h>
#include <mud/cmd.h>

inherit CMD_BASE;

void setup()
{
  position = 0;
}

string query_usage()
{
  return "singletons";
}

string query_short_help()
{
  return "Shows a list of the singleton objects handled by the system.";
}

static int cmd(string name, object me, string verb)
{
  mapping obs;
  string * ks;
  int i;

  obs = SINGLETON_HANDLER->handlers();
  ks = keys(obs);

  if (!sizeof(ks))
    write("\n * No handlers loaded yet.\n");
  else
  {
    write("\n * Handlers loaded:\n");

    for (i = 0; i < sizeof(ks); i++)
      write("   - " + ks[i] + " (" + base_name(obs[ks[i]]) + ")\n");
  }

  obs = SINGLETON_HANDLER->tables();
  ks = keys(obs);

  if (!sizeof(ks))
    write("\n * No tables loaded yet.\n");
  else
  {
    write("\n * Tables loaded:\n");

    for (i = 0; i < sizeof(ks); i++)
      write("   - " + ks[i] + " (" + base_name(obs[ks[i]]) + ")\n");
  }

  write("\n");

  return 1;
}
