// - Añadido define para el numero maximo de lineas y subido de 
//   900 a 2000 y arreglos varios.
//   neverbot 08/2008

#include <mud/cmd.h>

inherit CMD_BASE;

#define NUMBER_OF_LINES 2000

void setup()
{
  position = 1;
}

static int cmd(mixed str, object me, string verb) 
{
  int i, j, num, ignore_case, found;
  string *files, *bit, *bit2;
  string s1, s2, s3, s4, s5;
  string err;

  if (!strlen(str)) 
  {
    notify_fail("Uso: grep [-in] <pattern> <archivo(s)>\n"); 
    return 0;
  }

  num = 1;
  ignore_case = 0;
  found = 0;

  if (sscanf(str, "-n %s", str) == 1)
  {
    num = 0;
  }

  if (sscanf(str, "-i %s", str) == 1)
  {
    ignore_case = 1;
  }

  if (sscanf(str, "%s -n %s", s1, s2) == 2) 
  {
    num = 0;
    str = s1+" "+s2;
  }

  if (sscanf(str, "%s %s", s1, s2) != 2) 
  {
    notify_fail("Uso: grep [-in] <patrón> <archivo(s)>\n");
    return 0;
  }

  files = get_files(s2);

  if (ignore_case)
  {
    s5 = capitalize(s1);
  }

  if (!sizeof(files)) 
  {
    notify_fail("Fichero(s) " + s2 + " no encontrado(s).\n");
    return 0;
  }

  // Recorremos todos los archivos
  for (i = 0; i < sizeof(files); i++)
  {
    if (file_size(files[i]) > 0)
    {
      catch (str = read_file(files[i], 0, NUMBER_OF_LINES) );

      if (!str)
      {
        tell_object(this_player(), "Fichero vacío o demasiado largo para usar grep ("+files[i]+").\n");
        continue;
      }

      // Si encontramos la palabra a buscar dentro del archivo
      if ( (sscanf(str, "%s" + s1 + "%s", s3, s4) == 2) ||
        (ignore_case && sscanf(str,"%s" + s5 + "%s", s3, s4) == 2) )
      {
        j = 0;
        found = 1;

        // str = read_file(files[i], 0, NUMBER_OF_LINES);
        write("Archivo: "+files[i]+"\n");

        while (str && num) 
        {
          while( (sscanf(str, "%s" + s1 + "%s", s3, s4) == 2 ) ||
              (ignore_case && sscanf(str,"%s" + s5 + "%s", s3, s4) == 2) )
          {
            bit = explode(s3,"\n");
            bit2 = explode(s4,"\n");
            if (sizeof(bit))
              write(sprintf("%4d: %s\n", j + sizeof(bit), bit[sizeof(bit)-1] + s1 + (sizeof(bit2)?bit2[0]:"")));

            j += sizeof(bit);

            str = implode(bit2[1..],"\n");
          }

          j = ((j / NUMBER_OF_LINES) + 1) * NUMBER_OF_LINES;

          err = catch ( str = read_file(files[i], j, NUMBER_OF_LINES) );

          if (strlen(err))
            break;
        }
      }
    }
  }
  
  if (found == 0)
  {
    write("No encontrado '"+s1+"' en ningún archivo.\n");
  }

  return 1;
}

