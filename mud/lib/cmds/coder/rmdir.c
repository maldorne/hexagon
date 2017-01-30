
#include <mud/cmd.h>

inherit CMD_BASE;

void setup()
{
  position = 1;
}

static int cmd(string str, object me, string verb) 
{
  string *filenames;
  int fsize, loop;

  if (!strlen(str)) 
  {
    notify_fail("Sintaxis: rmdir <directorio>\n");
    return 0;
  }

  filenames = get_files(str);

  if (!sizeof(filenames)) 
  {
    notify_fail("No existe el directorio: '" + str + "'.\n");
    return 0;
  }

  for(loop = 0; loop < sizeof(filenames); loop++) 
  {
    str = filenames[loop];
    fsize = file_size(str);

    if (fsize == -1) 
    {
      notify_fail("No existe el directorio: '" + str + "'.\n");
      return 0;
    }

    if (fsize != -2) 
    {
      notify_fail(str + " no es un directorio.\n");
      return 0;
    }

    if (!rmdir(str)) 
    {
      notify_fail("No se puede borrar el directorio: '" + str + "'.\n");
      return 0;
    }
  }

  write("Ok.\n");
  return 1;
} /* removedir() */


