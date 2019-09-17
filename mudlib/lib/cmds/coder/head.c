/** coded by Raskolnikov July 1996 **/

#include <mud/cmd.h>

inherit CMD_BASE;

void setup()
{
  position = 1;
}

static int cmd(string str, object me, string verb)
{
  string file, *filename, text;
  int range;

  if (!strlen(str))
  {
    notify_fail("Usage : head -range|file\n");
    return 0;
  }

  if (sscanf(str, "-%d %s", range, file) == 2)
  {
    filename = get_files(file);

    if (!sizeof(filename))
    {
      notify_fail("Invalid file\n");
      return 0;
    }

    if (range < 0)
    {
      notify_fail("Invalid range : "+range+"\n");
      return 0;
    }

    if (!range)
      range = 10;

    text = read_file_line(filename[0], 0, range);
    // printf("%-=*s", this_user()->query_cols(), text);
    tell_object(me, text + "\n");
    return 1;
  }
  else
  {
    sscanf(str, "%s", file);
    filename = get_files(file);

    if (!sizeof(filename))
    {
      notify_fail("Fichero inválido.\n");
      return 0;
    }

    text = read_file_line(filename[0], 0, 10);
    // printf("%-=*s", this_user()->query_cols(), text);
    tell_object(me, text + "\n");
    return 1;
  }
} /* head_file() */

string query_short_help()
{
  return "Devuelve las primeras líneas de un archivo.";
  /*
  return "This command allows the user to see the contents "
  "of file starting at the first line and ending at line range. "
  " A default of 10 lines is returned if range is not specified.\n\n"
  "Ex. head -20 /std/room.c.\n"
  "will return the first 20 lines of the file room.c.\n\n"
  "Note: Thextension must be specified.";
  */
}

string query_usage() {
  return "head -range|file";
}
