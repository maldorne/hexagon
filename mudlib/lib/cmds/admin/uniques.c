
// Where every unique object of the game is. A unique object is one that may
// only exist once: the handler that tracks it is not built yet, so this only
// says so.

#include <std.h>
#include <mud/cmd.h>

inherit CMD_BASE;

void setup()
{
  set_aliases(({ "uniques" }));
  set_usage("uniques");
  set_help(
    "List every unique object of the game, and where each one is.\n" +
    "\n" +
    "A unique object exists once: whoever holds it holds the only one, and " +
    "it can only be cloned again once the one in play is gone. The handler " +
    "that keeps track of them has not been written yet, so there is nothing " +
    "to list.");
}

static int cmd(string str, object me, string verb)
{
  write("There is no unique-object handler in this mudlib yet.\n");
  return 1;
}
