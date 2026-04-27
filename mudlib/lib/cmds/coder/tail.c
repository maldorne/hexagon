/** coded by Raskolnikov July 1996 **/
/** Vaeleth... tail more flexible **/
/** rewrite to fixed-size tail read, neverbot 04/2026 **/

#include <mud/cmd.h>

inherit CMD_BASE;

#define DEFAULT_LINES  10
#define TAIL_BYTES     16384  /* upper bound on bytes read from the file end */

void setup()
{
  set_aliases(({ "tail" }));
  set_usage("tail [-<number of lines>] <files>");
  set_help("Shows the last lines of a file.\n" +
    "A default of " + DEFAULT_LINES + " lines is returned if range is not specified.\n\n" +
    "Ex. tail -20 /lib/room.c\n" +
    "will return the last 20 lines of the file room.c.");
}

static int cmd(string str, object me, string verb)
{
  string file, *filename, *lines, content, err;
  int range, fsize, want, start;

  if (!strlen(str))
  {
    write("Syntax: " + query_usage() + "\n");
    return 1;
  }

  if (sscanf(str, "-%d %s", range, file) != 2)
    file = str;

  if (range < 0)
  {
    notify_fail("Invalid range: " + range + ".\n");
    return 0;
  }
  if (!range)
    range = DEFAULT_LINES;

  filename = get_files(file);
  if (!sizeof(filename))
  {
    notify_fail("That file does not exist.\n");
    return 0;
  }

  fsize = file_size(filename[0]);
  if (fsize < 0)
  {
    notify_fail("Cannot read " + filename[0] + ".\n");
    return 0;
  }
  if (fsize == 0)
  {
    write("");
    return 1;
  }

  /* read at most TAIL_BYTES from the end of the file */
  want  = (fsize < TAIL_BYTES) ? fsize : TAIL_BYTES;
  start = fsize - want;

  err = catch(content = read_file(filename[0], start, want));
  if (err || !content)
  {
    notify_fail("Could not read " + filename[0] + ".\n");
    return 0;
  }

  lines = explode(content, "\n");

  /* if we didn't start at byte 0, the first element is a partial line; drop it */
  if (start != 0 && sizeof(lines) > 1)
    lines = lines[1..];

  /* keep only the last `range` lines */
  if (sizeof(lines) > range)
    lines = lines[sizeof(lines) - range ..];

  write(implode(lines, "\n") + "\n");
  return 1;
}
