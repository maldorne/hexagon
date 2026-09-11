
// Keep a name out of the game for a while. The same list and the same
// enforcement as 'ban', with an hour at which it lapses on its own.

#include <std.h>
#include <mud/cmd.h>
#include <mud/secure.h>

inherit CMD_BASE;

void setup()
{
  set_aliases(({ "suspend" }));
  set_usage("suspend [ <name> <hours> | list | remove <name> ]");
  set_help(
    "Keep a name out of the game for a number of hours.\n" +
    "\n" +
    "  suspend <name> <hours>  keep it out for that long\n" +
    "  suspend list            everybody serving one, and until when\n" +
    "  suspend remove <name>   let them back in now\n" +
    "\n" +
    "The suspension lapses by itself: the first login attempt after its " +
    "hour has passed clears it and goes through. Only a name with a " +
    "character behind it can be suspended -- to keep out a name that has " +
    "never played, 'ban' it.");
}

static int cmd(string str, object me, string verb)
{
  mapping suspended;
  string name;
  string * names;
  string out;
  int hours, i;

  if (!str || !strlen(str))
  {
    notify_fail("Usage: suspend [ <name> <hours> | list | remove <name> ]\n");
    return 0;
  }

  if (str == "list")
  {
    suspended = (mapping)SECURE->query_suspended();
    if (!mappingp(suspended) || !map_sizeof(suspended))
    {
      write("Nobody is suspended.\n");
      return 1;
    }

    names = map_indices(suspended);
    out = "Suspended:\n";
    for (i = 0; i < sizeof(names); i++)
      out += sprintf("  %-20s until %s%s\n", names[i],
                     ctime(suspended[names[i]]),
                     suspended[names[i]] > time() ? "" : "  (lapsed)");

    write(out);
    return 1;
  }

  if (sscanf(str, "remove %s", name) == 1)
  {
    name = lower_case(name);
    suspended = (mapping)SECURE->query_suspended();

    if (!mappingp(suspended) || undefinedp(suspended[name]))
    {
      notify_fail("'" + name + "' is not suspended.\n");
      return 0;
    }

    if (!SECURE->unsuspend_person(name))
    {
      notify_fail("The secure object refused to lift it.\n");
      return 0;
    }

    write("'" + name + "' may log in again.\n");
    return 1;
  }

  if (sscanf(str, "%s %d", name, hours) != 2 || hours < 1)
  {
    notify_fail("Say for how long, in hours: suspend <name> <hours>\n");
    return 0;
  }

  name = lower_case(name);
  if (!SECURE->suspend_person(name, hours * 3600))
  {
    notify_fail("The secure object refused: '" + name + "' has no " +
                "character of that name, or you may not suspend.\n");
    return 0;
  }

  write("'" + name + "' is out until " + ctime(time() + hours * 3600) +
        ".\n");
  return 1;
}
