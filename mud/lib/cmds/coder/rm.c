
#include <mud/cmd.h>

inherit CMD_BASE;

void setup()
{
  position = 1;
}

static int cmd(string str, object me, string verb) 
{
    int fsize, loop;
    string *filenames;

    if (this_player(1) != this_player()) return 0;

    if (!strlen(str)) 
    {
      notify_fail("Sintaxis: rm <archivo> [archivo ...]\n");
      return 0;
    }

    filenames = get_files(str);

    if (!sizeof(filenames)) 
    {
      notify_fail("No existe el archivo: '" + str + "'.\n");
      return 0;
    }

    for(loop = 0; loop < sizeof(filenames); loop++) 
    {
      string temp, temp2;

      temp2 = "";
      str = filenames[loop];
      
      if((sscanf(str, "%s/.%s", temp, temp2) && (temp2 == "")) || 
          sscanf(str, "%s/..%s", temp, temp2) && (temp2 == "") ) 
      {
        continue;
      }
      
      fsize = file_size(str);
      
      if (fsize == -1) 
      {
         notify_fail("No existe el archivo o directorio.\n");
         return 0;
      }
      if (fsize == -2) 
      {
        if (!rmdir(str)) 
        {
          notify_fail("No se puede borrar el directorio: '" + str + "'.\n");
          return 0;
        }
      } 
      else if (!rm(str)) 
      {
        notify_fail("No se puede borrar el archivo.\n");
        return 0;
      }
    }
    write("Ok.\n");
    return 1;
} /* rm_files() */
 
