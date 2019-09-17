
#include <mud/cmd.h>

inherit CMD_BASE;

void setup()
{
  position = 1;
}

static int cmd(string str, object me, string verb)
{
  string *filenames, text, dest, *fnames;
  int loop, fs;

  if (this_player(1) != this_user())
    return 0;

  if (!strlen(str))
  {
    notify_fail("Uso: cp fichero [fichero|dir...]\n");
    return 0;
  }

  fnames = explode(str, " ");
  filenames = get_files(implode(fnames[0..sizeof(fnames) -2], "/"));

  if (!sizeof(filenames))
  {
    notify_fail("Sintaxis: cp fichero [fichero|dir...]\n");
    return 0;
  }

  dest = fnames[sizeof(fnames) - 1];
  dest = get_path(dest);

  if (!dest)
  {
    tell_object(this_player(),"Necesitas especificar un destino.\n");
    return 1;
  }

  for(loop = 0; loop < sizeof(filenames); loop++)
  {
    str = filenames[loop];
    text = read_file(str);
    if (!text)
    {
      tell_object(this_player(),"Fichero inexistente: " + str + "\n");
      continue;
    }

    fs = file_size(dest);

    if (fs == -2)
    {
      string *names;

      names = explode(str, "/");
      fs = file_size(dest + "/" + names[sizeof(names) - 1]);

      if (fs != -1)
      {
        tell_object(this_player(),"Fichero existente: "+dest+"/"+
          names[sizeof(names) - 1] + "\n");
        continue;
      }

      write_file(dest + "/" + names[sizeof(names) - 1], text);
    }
    else
    {
      if (fs != -1)
      {
        tell_object(this_player(),"Fichero existente: " + dest + "\n");
        continue;
      }

      write_file(dest, text);
    }
  }
  tell_object(this_player(),"Ok.\n");
  return 1;
} /* cp_file() */


