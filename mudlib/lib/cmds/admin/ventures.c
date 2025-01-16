
#include <mud/cmd.h>
#include <room/ventures.h>
#include <room/location.h>
#include <user/terminal.h>

inherit CMD_BASE;

void setup()
{
  set_aliases(({ "ventures" }));
  set_usage("ventures");
  set_help("List info about ventures through the whole system (shops, pubs, etc).");
}

static int cmd(string str, object me, string verb) 
{
  string * ventures;
  string ret;
  int i, j;
  object terminal, where;

  terminal = load_object(TERM_HANDLER);

  ventures = ({ });
  ventures += load_object(VENTURES_HANDLER)->query_shops_array();
  ventures += load_object(VENTURES_HANDLER)->query_pubs_array();

  if (!ventures || !sizeof(ventures))
  {
    write("No ventures stored in the handler.\n");
    return 1;
  }

  ret = "Ventures stored:\n\n";

  for (i = 0; i < sizeof(ventures); i++)
  {
    where = load_object(ventures[i]);

    // loaded first, it is a room
    if (where)
    {
      ret += sprintf(" - %-25s", extract(terminal->clean_string(where->short()), 0, 20));
      ret += sprintf(" %-6s %-10s", (where->query_shop() ? "shop" : "pub"), "room");
      ret += sprintf("%30s.c\n", ventures[i]);

    }
    // check if it is a location
    else if (where = load_object(LOCATION_HANDLER)->load_location(ventures[i]))
    {
      ret += sprintf(" - %-25s", extract(terminal->clean_string(where->short()), 0, 20));
      ret += sprintf(" %-6s %-10s", (where->query_shop() ? "shop" : "pub"), "location");
      ret += sprintf("%30s\n", ventures[i]);
    }
    else
    {
      ret += sprintf(" - %-25s", "not loaded");
      ret += sprintf(" %-12s", "(??)");
      ret += sprintf("%30s\n", ventures[i]);
    }
  }

  write(ret + "\n");
  return 1;
}
