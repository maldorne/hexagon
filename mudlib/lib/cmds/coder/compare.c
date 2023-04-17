
/** coded by Raskolnikov July 1996 **/

#include <mud/cmd.h>
#include <translations/common.h>
#include <language.h>

inherit CMD_BASE;

void setup()
{
  set_aliases(({ "compare" }));
  set_usage("compare -<which_file> file1 file2");
  set_help("Allows the user to compare two files.\n" +
      "The command returns the lines of file1 that were " +
      "not found in file2 if which_file is equal to 1 and " +
      "vice-versa if which_file is equal to 2.\n\n" +
      "Note: both file1 and file2 differences are returned if " +
      "which_file is not entered.");
}

static int cmd(string str, object me, string verb)
{
  object user;
  int i, which_file, check;
  mixed check1, check2;
  string file1, file2, *filename1, *filename2,
  *text1, *text2, *diffs1, *diffs2, output1, output2;

  if (!strlen(str))
  {
    notify_fail(_LANG_SYNTAX + ": " + query_usage() + "\n");
    return 0;
  }

  check = sscanf(str, "-%d %s %s", which_file, file1, file2);

  if (check != 3)
  {
    which_file = 3;
    check = sscanf(str, "%s %s", file1, file2);
    if (!check || check != 2)
    {
      notify_fail(_LANG_SYNTAX + ": " + query_usage() + "\n");
      return 0;
    }
  }

  if ((which_file < 1) || (which_file > 3))
  {
    notify_fail("Invalid -<which_file> value.\n\nOptions:\n" +
      "-1 for file1\n-2 for file2\n-3 or nothing for both\n");
    return 0;
  }

  user = me->user();

  filename1 = get_files(file1);
  filename2 = get_files(file2);

  if (!sizeof(filename1) || !sizeof(filename2))
  {
    notify_fail("That file does not exist.\n");
    return 0;
  }

  check1 = read_file(filename1[0]);
  check2 = read_file(filename2[0]);

  if (!check1)
  {
    notify_fail("Not a file: " + filename1[0]+"\n");
    return 0;
  }

  if (!check2)
  {
    notify_fail("Not a file: " + filename2[0]+"\n");
    return 0;
  }

  diffs1 = ({ });
  diffs2 = ({ });
  text1 = explode(check1, "\n");
  text2 = explode(check2, "\n");

  if ((which_file == 1) || (which_file == 3))
    for (i = 0; i < sizeof(text1); i++)
      if (member_array(text1[i], text2) == -1)
        diffs1 += ({ (i+1)+"  "+text1[i] });

  if ((which_file == 2) || (which_file == 3))
    for (i = 0; i < sizeof(text2); i++)
      if (member_array(text2[i], text1) == -1)
        diffs2 += ({ (i+1)+"  "+text2[i] });

  output1 = implode(diffs1, "\n");
  output2 = implode(diffs2, "\n");

  if (which_file == 1)
  {
    user->more_string("Lines in "+filename1[0]+" not "+
      "in "+filename2[0]+"\n\n"+output1+"\n\n");
    return 1;
  }

  if (which_file == 2)
  {
    user->more_string("Lines in "+filename2[0]+" not "+
      "in "+filename1[0]+"\n\n"+output2+"\n\n");
    return 1;
  }

  user->more_string("Lines in "+filename1[0]+" not "+
    "in "+filename2[0]+"\n\n"+output1+"\n\nLines "+
    "in "+filename2[0]+" not in "+filename1[0]+"\n\n"+
    ""+output2+"\n");
  return 1;
} /* compare_file() */
