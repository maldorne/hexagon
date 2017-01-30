
#include <mud/cmd.h>

inherit CMD_BASE;

void setup()
{
  position = 1;
}

static int cmd(string str, object me, string verb) 
{
  string *filenames, dest, *fnames;
  int loop, fs;

  if (this_player(1) != this_player()) 
    return 0;

  if (!strlen(str)) 
  {
    notify_fail("Sintaxis: mv archivo [archivo|dir...]\n");
    return 0;
  }

  fnames = explode(str, " ");
  filenames = get_files(implode(fnames[0..sizeof(fnames)-2], " "));

  if (!sizeof(filenames)) 
  {
    notify_fail("Sintaxis: mv archivo [archivo|dir...]\n");
    return 0;
  }

  dest = fnames[sizeof(fnames) - 1];
  dest = get_path(dest);

  if (!dest) 
  {
     // write("No destination\n");
     write("El destino no existe.\n");
     return 1;
  }

  for(loop = 0; loop < sizeof(filenames); loop++) 
  {
    str = filenames[loop];

    if (file_size(str) == -1) 
    {
      // write("No such file : " + str + "\n");
      write("No existe el archivo: '" + str + "'.\n");
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
        // write("file exists " + dest + "/" + names[sizeof(names) - 1] + "\n");
        write("El archivo ya existe: " + dest + "/" + names[sizeof(names) - 1]+"\n");
        continue;
      }

      rename(str, dest + "/" + names[sizeof(names) - 1]);
    }

    else 
    {
      if (fs != -1) 
      {
        // write("File exists : " + dest + "\n");
        write("El archivo ya existe: " + dest + "\n");
        continue;
      }
      
      rename(str, dest);
    }
  }

  write("Ok.\n");
  return 1;

} /* mv_file() */
 
