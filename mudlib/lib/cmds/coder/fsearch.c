
// List the functions defined in an LPC file, with the line each one starts on.
// Originally by Kimmuriel, 2000, as an LPC parser; rewritten as a scan of the
// lines outside any block.

#include <std.h>
#include <mud/cmd.h>

inherit CMD_BASE;

private string strip_comments(string line, int * in_block);
private int opens_definition(string line);
private int depth_change(string line);

// The words an LPC definition can start with
#define OPENERS ({ "int", "float", "string", "object", "mapping", "mixed", \
                   "void", "function", "private", "static", "public", \
                   "nomask", "varargs", "atomic" })

void setup()
{
  set_aliases(({ "fsearch" }));
  set_usage("fsearch [ <file> | here ]");
  set_help(
    "List the functions a file defines, and where each one starts.\n" +
    "\n" +
    "  fsearch /lib/user.c   the file you name\n" +
    "  fsearch guard.c       a file in the directory you are working in\n" +
    "  fsearch here          the file behind the room you are standing in\n" +
    "\n" +
    "Only definitions are listed, not prototypes, and a signature written " +
    "over several lines is shown joined. Reading only: nothing is loaded " +
    "or compiled, the file is read as text.");
}

// The line with anything commented out removed. `in_block` carries whether a
// /* */ comment was left open by the previous line.
private string strip_comments(string line, int * in_block)
{
  int i;

  if (in_block[0])
  {
    i = strsrch(line, "*/");
    if (i == -1)
      return "";
    in_block[0] = 0;
    line = line[i + 2 ..];
  }

  i = strsrch(line, "//");
  if (i != -1)
    line = line[.. i - 1];

  i = strsrch(line, "/*");
  if (i != -1)
  {
    int j;

    j = strsrch(line[i ..], "*/");
    if (j == -1)
    {
      in_block[0] = 1;
      return line[.. i - 1];
    }

    line = line[.. i - 1] + line[i + j + 2 ..];
  }

  return line;
}

// Whether a line outside any block starts a function definition: a known
// opening word, a name, and an argument list.
private int opens_definition(string line)
{
  string first;
  int i;

  line = trim(line);
  if (!strlen(line) || line[0] == '#')
    return FALSE;

  i = strsrch(line, "(");
  if (i == -1)
    return FALSE;

  first = explode(line, " ")[0];
  if (strlen(first) && first[strlen(first) - 1] == '*')
    first = first[.. strlen(first) - 2];

  return member_array(first, OPENERS) != -1;
}

// How many blocks the line opens, minus the ones it closes
private int depth_change(string line)
{
  int i, change;

  for (i = 0; i < strlen(line); i++)
  {
    if (line[i] == '{')
      change++;
    if (line[i] == '}')
      change--;
  }

  return change;
}

static int cmd(string str, object me, string verb)
{
  string file, name, out;
  string * lines;
  int * in_block;
  int i, depth, count;

  if (!str || !strlen(str))
  {
    notify_fail("Usage: fsearch [ <file> | here ]\n");
    return 0;
  }

  if (str == "here")
  {
    if (!environment(me))
    {
      notify_fail("You are nowhere.\n");
      return 0;
    }
    name = file_name(environment(me)) + ".c";
  }
  else
    name = get_path(str);

  file = read_file(name);
  if (!file)
  {
    notify_fail("There is no file called " + name + ".\n");
    return 0;
  }

  lines = explode(file, "\n");
  in_block = ({ 0 });
  out = "";

  for (i = 0; i < sizeof(lines); i++)
  {
    string line;

    line = strip_comments(lines[i], in_block);

    // a definition is written outside every block; anything deeper is the
    // body of the function above it
    if (!depth && opens_definition(line))
    {
      int j;

      line = trim(line);

      // a signature may be spread over several lines: read on until the
      // argument list closes
      for (j = 1; strsrch(line, ")") == -1 && i + j < sizeof(lines); j++)
        line += " " + trim(strip_comments(lines[i + j], in_block));

      // a prototype closes its argument list with a semicolon
      if (strsrch(line, ");") == -1)
      {
        out += sprintf("  %5d  %s\n", i + 1, line);
        count++;
      }
    }

    depth += depth_change(line);
    if (depth < 0)
      depth = 0;
  }

  if (!count)
  {
    notify_fail(name + " defines no functions.\n");
    return 0;
  }

  write(name + ", " + count + " function" + (count == 1 ? "" : "s") + ":\n" +
        out);
  return 1;
}
