/** coded by Raskolnikov Spetember 1996 **/

#include <mud/cmd.h>
#include <translations/common.h>

inherit CMD_BASE;

void setup()
{
  set_aliases(({ "cplines" }));
  set_usage("cplines <file> <start> <end> <destination>");
  set_help("Copies the specified lines from a file to the end of another file.");
}

static int cmd(string str, object me, string verb)
{
  string *sfile, sfname, *dfile, dfname, lines;
  int sstart, send, slength, check;

  if (this_player(1) != this_user()) 
    return 0;

  if (!strlen(str))
  {
    notify_fail(_LANG_SYNTAX + ": " + query_usage() + "\n");
    return 0;
  }

  check = sscanf(str, "%s %d %d %s", sfname, sstart, send, dfname);

  if (check != 4)
  {
    notify_fail(_LANG_SYNTAX + ": " + query_usage() + "\n");
    return 0;
  }

  sfile = get_files(sfname);
  dfile = get_files(dfname);

  if (!sizeof(sfile))
  {
    notify_fail("Not a file: " + sfname + "\n");
    return 0;
  }

  if (!sizeof(dfile))
  {
    notify_fail("Not a file: " + dfname + "\n");
    return 0;
  }

  if (sstart < 1)
  {
    notify_fail("Invalid start number.\n");
    return 0;
  }

  slength = file_length(sfile[0]);

  if (send > slength)
  {
    notify_fail("End number too large.\n");
    return 0;
  }

  lines = read_file_line(sfile[0], sstart, send - sstart);

  if (!strlen(lines))
  {
    notify_fail("Content empty. Invalid lines specified?\n");
    return 0;
  }

  write_file(dfile[0], lines, 0);
  write("Lines copied.\n");
  
  return 1;
}
