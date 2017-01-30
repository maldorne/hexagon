
#include <mud/cmd.h>

inherit CMD_BASE;

void setup()
{
  position = 1;
}

static int cmd(string str, object me, string verb) 
{
  string *filenames;
  int loop;

  if (!str) 
  {
    notify_fail("¿Cat de qué fichero?\n");
    return 0;
  }

  if (str == "*") 
  {
    notify_fail("¿cat *? ¡Olvídalo!\n");
    return 0;
  }

  filenames = get_files(str);

  if (!sizeof(filenames)) 
  {
    notify_fail(str + ": No existe ese fichero.\n");
    return 0;
  }

  for(loop = 0; loop < sizeof(filenames); loop++) 
  {
    int result;
    str = filenames[loop];

    if(sizeof(filenames) > 1) 
     write("Archivo : " + str + "\n");

    if (file_size(str) < 8196)
    {
      result = cat(str);
      if (!result)
        write("No se ha podido leer el archivo "+str+"\n");
    }
    else 
    {
      write(read_bytes(str,0,8195));
      write("\n ----- TRUNCADO ----- \n");
    }
  }
  return 1;
} /* exa_file() */
 
