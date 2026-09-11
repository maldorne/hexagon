
// Keep a name out of the game for good. The list lives on the secure object
// and the login prompt is what enforces it.

#include <std.h>
#include <mud/cmd.h>
#include <mud/secure.h>

inherit CMD_BASE;

void setup()
{
  set_aliases(({ "ban" }));
  set_usage("ban [ <name> <reason> | list | remove <name> ]");
  set_help(
    "Ban a name from the game.\n" +
    "\n" +
    "  ban <name> <reason>  ban it, saying why\n" +
    "  ban list             every name banned, and why\n" +
    "  ban remove <name>    let it back in\n" +
    "\n" +
    "The name is either an account or a character: whichever is typed at " +
    "the login prompt is checked, so banning an account keeps its " +
    "characters out and banning one character leaves the rest of the " +
    "account alone. The reason is shown to whoever tries to get in.\n" +
    "\n" +
    "For a spell rather than for good, use 'suspend'.");
}

static int cmd(string str, object me, string verb)
{
  mapping banned;
  string name, reason;
  string * names;
  string out;
  int i;

  if (!str || !strlen(str))
  {
    notify_fail("Usage: ban [ <name> <reason> | list | remove <name> ]\n");
    return 0;
  }

  if (str == "list")
  {
    banned = (mapping)SECURE->query_banished();
    if (!mappingp(banned) || !map_sizeof(banned))
    {
      write("Nobody is banned.\n");
      return 1;
    }

    names = map_indices(banned);
    out = "Banned names:\n";
    for (i = 0; i < sizeof(names); i++)
      out += sprintf("  %-20s %s\n", names[i], banned[names[i]]);

    write(out);
    return 1;
  }

  if (sscanf(str, "remove %s", name) == 1)
  {
    name = lower_case(name);
    banned = (mapping)SECURE->query_banished();

    if (!mappingp(banned) || undefinedp(banned[name]))
    {
      notify_fail("'" + name + "' is not banned.\n");
      return 0;
    }

    if (!SECURE->unbanish_playername(name))
    {
      notify_fail("The secure object refused to lift it.\n");
      return 0;
    }

    write("'" + name + "' may log in again.\n");
    return 1;
  }

  if (sscanf(str, "%s %s", name, reason) != 2)
  {
    notify_fail("Say why: ban <name> <reason>\n");
    return 0;
  }

  name = lower_case(name);
  if (!SECURE->banish_playername(name, reason))
  {
    notify_fail("The secure object refused to ban that name.\n");
    return 0;
  }

  write("'" + name + "' banned: " + reason + "\n");
  return 1;
}
