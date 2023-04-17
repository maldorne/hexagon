// backup.c -- Automagically creates a backup file
// Code originally by Mordred, around 1 May 96
// 06 May 96  Agamemnon  Added file informs
//            Also added EDIT logging
//            Thirdly, added support for multiple files
//            Finally cleaned it up and added more descriptive fails
// 06 June 96 Agamemnon  Changed to use the "cp" efun instead of read
//            and write_file... (faster, and less chance
//            to barf and die)
// 25 Abril 03 neverbot translated to spanish and compatible with new ctime()

#include <mud/cmd.h>
#include <translations/common.h>

#define COL ""

inherit CMD_BASE;

void setup()
{
  set_aliases(({ "backup" }));
  set_usage("backup <file>");
  set_help("Creates a backup of a file.");
}

static int cmd(string str, object me, string verb)
{
  string *files, orig, backup, date;
  int i, ver;

  if (!strlen(str))
  {
    notify_fail(_LANG_SYNTAX + ": " + query_usage() + "\n");
    return 0;
  }

  date = ctime(time(), 2);
  files = get_files(str);

  if (!sizeof(files)) 
  {
    write("Cannot find file(s): " + str + ".\n");
    return 1;
  }

  for (i = 0; i < sizeof(files); i++) 
  {
    orig = files[i];

    if (file_size(orig) == -1) 
    {
      write("Cannot back up file " + orig + ": No such file or directory.\n");
      continue;
    }

    if (file_size(orig) == -2) 
    {
      write("Cannot backup file " + orig + ": Is a directory.\n");
      continue;
    }

    backup = orig + "." +  date + "." + this_player()->query_name();

    if (file_size(backup) == -2) 
    {
      write("Cannot backup to " + orig + ": It already exists as a directory.\n");
      continue;
    }
    
    if (file_size(backup) != -1) 
    {
      // fix error with versioning, neverbot 4/03
      ver = 2;
      while (file_size(backup + ".v" +  ver) > 0) 
        ver++;
      backup += ".v" +  ver;
    }

    if (cp(orig, backup)) 
    {
      write(COL + "File " + orig + " backed up as " +  backup + "\n");
    } 
    else 
    {
      write("Unable to write file : " + backup + "\n");
    }
  }

  me->set_trivial_action();
  return 1;
}
