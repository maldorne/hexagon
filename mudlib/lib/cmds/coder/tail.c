
/** coded by Raskolnikov July 1996 **/
/** Vaeleth... tail more flexible **/

#include <mud/cmd.h>

inherit CMD_BASE;

void setup()
{
  position = 1;
}

string query_usage()
{
  return "tail [-<number of lines>] <files>\n";
}

string query_short_help()
{
  return "Shows the last lines of a file.";
}

static int cmd(string str, object me, string verb)
{
  string file, *filename, text;
  int range, lines;
  string content, err;

  content = "";

  if (!strlen(str))
  {
    notify_fail("Syntax: tail [-<number of lines>] <files>\n");
    return 0;
  }

  if (sscanf(str, "-%d %s", range, file) == 2)
  {
    filename = get_files(file);

    if (!sizeof(filename))
    {
      notify_fail("That file does not exist.\n");
      return 0;
    }

    if (range < 0)
    {
      notify_fail("Invalid range: " + range + "\n");
      return 0;
    }

    if (!range)
      range = 20;

    err = catch(content = read_file(filename[0]));

    if (err || !content)
    {
      notify_fail("Unable to read the file... maybe too long?\n");
      return 0;
    }

    lines = sizeof(explode(content, "\n"));
    text = read_file_line(filename[0], lines - range, lines);

    write(text);
    return 1;
  }
  else
  {
    sscanf(str, "%s", file);
    filename = get_files(file);

    if (!sizeof(filename))
    {
      notify_fail("That file does not exist.\n");
      return 0;
    }

    err = catch(content = read_file(filename[0]));

    if (err || !content)
    {
      notify_fail("Unable to read the file... maybe too long?\n");
      return 0;
    }

    lines = sizeof(explode(content, "\n"));
    range = 20;
    text = read_file_line(filename[0], (lines - range < 0 ? 0 : lines - range), lines);

    write(text);
    return 1;
  }
}
