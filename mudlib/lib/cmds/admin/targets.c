
#include <mud/cmd.h>
#include <maps/maps.h>

inherit CMD_BASE;

void setup()
{
  set_aliases(({ "targets" }));
  set_usage("targets");
  set_help("List your targets to search for actions.");
}

static int cmd(string str, object me, string verb) 
{
  object * targs;
  string ret;
  int i, max_length;

  max_length = 0;
  targs = targets(me);

  if (!targs || !sizeof(targs))
  {
    write("No targets... this should not happen.\n");
    return 1;
  }

  ret = "Available targets:\n\n";

  for (i = 0; i < sizeof(targs); i++)
    if (strlen(to_string(targs[i])) > max_length)
      max_length = strlen(to_string(targs[i]));

  for (i = 0; i < sizeof(targs); i++)
  {
    string id;
    // remove carriage return from id
    id = replace_string(to_string(targs[i]), "\n", "");

    ret += " -" + sprintf("%-" + max_length + "s", id);

    if (targs[i]->query_name())
      ret += " (" + targs[i]->query_name() + ")";
    ret += "\n";
  }

  write(ret + "\n");
  return 1;
}
