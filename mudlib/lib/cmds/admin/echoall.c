
// A line to everybody connected, with nothing to say who wrote it.

#include <std.h>
#include <mud/cmd.h>

inherit CMD_BASE;

void setup()
{
  set_aliases(({ "echoall" }));
  set_usage("echoall <message>");
  set_help(
    "Send a line to everybody connected.\n" +
    "\n" +
    "The message goes out exactly as you type it, on a line of its own, and " +
    "nobody is told it came from you -- so write enough for it to stand on " +
    "its own, punctuation included. To sign it, use 'emoteall'. Every use " +
    "is logged.");
}

static int cmd(string str, object me, string verb)
{
  object * connections;
  int i;

  if (!str || !strlen(str))
  {
    notify_fail("Say what to send: echoall <message>\n");
    return 0;
  }

  connections = users();

  for (i = 0; i < sizeof(connections); i++)
    tell_object(connections[i], "\n" + str + "\n");

  log_file("echoes", "[" + ctime(time(), 4) + "] " + me->query_cap_name() +
           ": " + str + "\n");

  write("Sent to " + sizeof(connections) + " connection" +
        (sizeof(connections) == 1 ? "" : "s") + ".\n");
  return 1;
}
