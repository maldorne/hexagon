
#include <mud/cmd.h>
#include <mud/secure.h>

inherit CMD_BASE;

int ex_spool(string yn,string file,int linum);
// varargs object * wiz_present(string str,object ob,int internal);

void setup()
{
  set_aliases(({ "more" }));
  set_usage("more <file>");
  set_help("Shows the contents of given file, paginated.");
}

static int cmd(string str, object me, string verb)
{
  object * things, user;
  string * filenames;
  int egg;
  string spam;

  if (!strlen(str))
  {
    write("Syntax: " + query_usage() + "\n");
    return 1;
  }

  user = me->user();

  /* dodgy idea, but allows 'ed here' or 'ed strawberry' */
  if (sizeof(things = user->query_role()->wiz_present(str, me)))
  {
    spam = object_name(things[0]);
    sscanf(spam, "%s#%d", spam, egg);
    if (file_size(spam) < 0)
      filenames = ({ spam + ".c" });
    else
      filenames = ({ spam });
  }
  else
    filenames = get_files(str);

  if (!sizeof(filenames))
  {
    str = get_path(str);
  }
  else
  {
    if (sizeof(filenames) > 0)
    {
      str = filenames[0];
      if (sizeof(filenames) > 1)
      {
        int loop;
        loop = 0;
        while (loop < sizeof(filenames) && file_size(filenames[loop]) < 0)
          loop++;
        if (loop >= sizeof(filenames))
        {
          write("That file does not exist.\n");
          return 0;
        }
        else
        {
          str = filenames[loop];
        }
        write("Ambiguous name, using: " + str + "\n");
      }
    }
  }

  if (file_size(str) == -2)
  {
    write("Is a directory.\n");
    return 1;
  }

  if (file_size(str) == -1)
  {
    write("There are no matching files.\n");
    return 1;
  }

  if (!SECURE->valid_read(str, this_player()))
  {
    write("You have no permissions to see that file.\n");
    return 1;
  }

  ex_spool("y", str, 1);
  return 1;
}

int ex_spool(string yn, string fil, int linum)
{
  string s1;
  string * lines;
  int i;
  mixed tnum;

  if (sscanf(yn, "%d", tnum) == 1)
  {
    linum = (int)tnum;
    if (linum < 1 )
      linum = 1;
  }
  // exiting command, from input_to
  else if ((yn == "Q") || (yn == "q"))
  {
    write("Exiting...\n");
    return 1;
  }
  // from input_to, something not recognize has been written
  else if (strlen(yn) && (yn != "Y") && (yn != "y")) 
  {
    write("Exiting...\n");
    return 1;
  }

  // for (i = 0; i < this_player()->query_rows(); linum++)
  // {
  //   i++;
  //   s1 = read_file_line(fil, linum, 1);
  //   if ( !strlen(s1) && (s1 != "\n") )
  //   {
  //     write("\n");
  //     return 1;
  //   }

  //   write(sprintf("%4d: %s", linum, s1));
  // }

  s1 = read_file_line(fil, linum, this_user()->query_rows());

  if (!strlen(s1))
  {
    write("\n");
    return 1;
  }

  lines = explode(s1, "\n");

  for (i = 0; i < sizeof(lines); i++, linum++)
    write(sprintf("%4d: %s\n", linum, lines[i]));

  write(sprintf("File: %s. Q to quit", fil));
  input_to("ex_spool", 0, fil, linum);
  return 1;
}
