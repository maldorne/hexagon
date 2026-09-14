
// Find files by name, walking a directory and everything under it.

#include <std.h>
#include <mud/cmd.h>

inherit CMD_BASE;

#define MAX_MATCHES 200

private string * walk(string dir, string pattern, int * found);

void setup()
{
  set_aliases(({ "findfile" }));
  set_usage("findfile [<directory>] <name>");
  set_help(
    "Find every file whose name contains <name>.\n" +
    "\n" +
    "  findfile guard.c          look through the whole mudlib\n" +
    "  findfile /lib/cmds ban    look through one directory and below\n" +
    "\n" +
    "The match is a substring, not a pattern: 'guard' answers with " +
    "guard.c, guardian.c and a guards/ directory alike. A relative " +
    "directory is taken from where you are working. At most " +
    "200 matches are listed.");
}

// Every path under `dir` whose last element contains `pattern`, directories
// included. `found` carries the running count so the walk can stop once the
// listing is long enough to be useless anyway.
private string * walk(string dir, string pattern, int * found)
{
  string * names, * matches;
  int i;

  matches = ({ });
  names = get_dir(dir);

  for (i = 0; i < sizeof(names); i++)
  {
    string path;

    if (found[0] >= MAX_MATCHES)
      break;

    path = dir + names[i];

    if (strsrch(names[i], pattern) != -1)
    {
      matches += ({ path });
      found[0]++;
    }

    if (file_size(path) == -2)
      matches += walk(path + "/", pattern, found);
  }

  return matches;
}

static int cmd(string str, object me, string verb)
{
  string * args, * matches;
  string dir, pattern;
  int * found;

  args = (str && strlen(str)) ? explode(str, " ") - ({ "" }) : ({ });

  if (!sizeof(args) || sizeof(args) > 2)
  {
    notify_fail("Usage: findfile [<directory>] <name>\n");
    return 0;
  }

  if (sizeof(args) == 2)
  {
    dir = get_path(args[0]);
    pattern = args[1];
  }
  else
  {
    dir = "/";
    pattern = args[0];
  }

  if (file_size(dir) != -2)
  {
    notify_fail("'" + dir + "' is not a directory.\n");
    return 0;
  }

  if (dir[strlen(dir) - 1] != '/')
    dir += "/";

  found = ({ 0 });
  matches = walk(dir, pattern, found);

  if (!sizeof(matches))
  {
    notify_fail("Nothing under " + dir + " is called " + pattern +
                "-something.\n");
    return 0;
  }

  write(implode(matches, "\n") + "\n" +
        (found[0] >= MAX_MATCHES
           ? "Stopped at " + MAX_MATCHES + " matches.\n"
           : "" + sizeof(matches) + " match" +
             (sizeof(matches) == 1 ? "" : "es") + ".\n"));
  return 1;
}
