
// Who is watching whom right now. The links live on the user objects, so
// this only walks the connections and reports the ones that have an end.

#include <std.h>
#include <mud/cmd.h>

inherit CMD_BASE;

void setup()
{
  set_aliases(({ "snoopers" }));
  set_usage("snoopers");
  set_help("List every session being watched, and who is watching it.");
}

static int cmd(string str, object me, string verb)
{
  object * connections;
  string out;
  int i, found;

  connections = users();
  out = "";
  found = 0;

  for (i = 0; i < sizeof(connections); i++)
  {
    object target, watcher, body;

    target = connections[i]->query_snooping();
    if (!target)
      continue;

    watcher = connections[i]->player();
    body = target->player();
    out += "  " + (watcher ? watcher->query_cap_name() : "?") +
           " is watching " + (body ? body->query_cap_name() : "?") + "\n";
    found++;
  }

  if (!found)
  {
    notify_fail("Nobody is being watched.\n");
    return 0;
  }

  write("" + found + " session" + (found == 1 ? "" : "s") + " watched:\n" +
        out);
  return 1;
}
