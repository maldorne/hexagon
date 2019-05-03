
#include <kernel.h>

#include "paths/resolve_path.c"
#include "paths/get_path.c"
#include "paths/path.c"

// idea taken from melville mudlib
static nomask int is_absolute_path(string str)
{
  if (!str || (str == ""))
    return -1;

  if ((str[0] == '/') || (str[0] == '~'))
    return 1;

  return -1;
}

string get_path_file_name(string path)
{
  string * words;
  words = explode(path, "/");

  if (sizeof(words) == 0)
    return "";

  if (sizeof(words) == 1)
    return words[0];

  return words[sizeof(words)-1];
}

string * get_files(string str)
{
  int loop, count;
  string *filenames, rmpath, temp1, temp2, *names, *files;
  string oldeiud;

  names = explode(str, " ") - ({ "" });

  if (sizeof(names) == 0)
    return ({ });

  filenames = ({ });

  for (count = 0; count < sizeof(names); count++)
  {
    str = names[count];
    str = rmpath = get_path(str);

    if (rmpath == "/")
    {
      filenames += ({ "/" });
      continue;
    }

    if (!strlen(rmpath))
      rmpath = "/";
    if (!strlen(str))
      str = "/";

    if (sscanf(rmpath, "%s/%s", temp1, temp2) == 2)
    {
      string * path_elements;

      path_elements = explode(rmpath, "/") - ({ "" });
      rmpath = implode(path_elements[0..sizeof(path_elements) - 2], "/");
    }

    oldeiud = geteuid();

    if (this_user() != nil)
      seteuid(geteuid(this_user()));
    else
      seteuid((string)MUDOS->get_root_uid());

    files = get_dir(str);

    // seteuid("");
    seteuid(oldeiud);

    if (files)
    {
      int loop2;
      for (loop2 = 0; loop2 < sizeof(files); loop2++)
      {
        filenames += ({ rmpath + "/" + files[loop2] });
      }
    }
  }

  for (loop = 0; loop < sizeof(filenames); loop++)
  {
    if (filenames[loop][0] != '/') {
      filenames[loop] = "/" + filenames[loop];
    }
  }

  return filenames;
}

// Changed to be *.* so as to handle virtual wombles as well
string *get_cfiles(string str)
{
  int loop;
  string temp, *names, temp2;

  names = explode(str, " ") - ({ "" });

  for (loop = 0; loop < sizeof(names); loop++)
  {
    if (sscanf(names[loop], "%s.%s", temp, temp2) != 2)
    {
      names[loop] += ".c";
    } else if (names[loop] == ".") {
      names[loop] = "*.c";
    } else if (names[loop] == "..") {
      names[loop] = "";
    }
  }

  names -= ({ "" });

  str = implode(names, " ");
  return get_files(str);
}
