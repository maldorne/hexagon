/** coded by Raskolnikov July 1996 **/
/** Vaeleth... tail more flexible **/

#include <mud/cmd.h>

inherit CMD_BASE;

string query_usage()
{
  return "tail [-<number of lines>] <files>";
}

string query_help()
{
  return "Shows the last lines of a file.\n" +
    "A default of 10 lines is returned if range is not specified.\n\n" +
    "Ex. tail -20 /lib/room.c\n" +
    "will return the last 20 lines of the file room.c.";
}

static int cmd(string str, object me, string verb)
{
  string file, *filename, text;
  int range, offset, lines;
  string content, err;

  content = "";
  text = "";
  offset = -1;
  lines = 0;

  if (!strlen(str))
  {
    write("Syntax: " + query_usage() + "\n\n");
    return 1;
  }

  if (sscanf(str, "-%d %s", range, file) != 2)
  {
    file = str;
  }

  filename = get_files(file);

  if (!sizeof(filename))
  {
    notify_fail("That file does not exist.\n");
    return 0;
  }

  if (range < 0)
  {
    notify_fail("Invalid range: " + range + ".\n");
    return 0;
  }

  if (!range)
    range = 10;

  while (!err && lines <= range)
  {
    text = content + text;
    err = catch(content = read_file(filename[0], offset, 1));
    offset--;
    if (content == "\n")
      lines++;
  }

  write(text);
  return 1;
}
