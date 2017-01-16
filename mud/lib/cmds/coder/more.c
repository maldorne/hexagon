
#include <mud/cmd.h>
#include <mud/secure.h>

inherit CMD_BASE;

int ex_spool(string yn,string file,int linum);
// varargs object * wiz_present(string str,object ob,int internal);

void setup()
{
  position = 1;
}

static int cmd(string str, object me, string verb) 
{
  object * things;
  string * filenames;
  int egg;
  string spam;

  if (!strlen(str))
  {
    write("Uso: more <fichero>\n");
    return 1;
  }

  /* dodgy idea, but allows 'ed here' or 'ed strawberry' */    
  if (sizeof(things = me->query_role()->wiz_present(str, me))) 
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
          write("No existe ese archivo.\n");
          return 0;
        }
        else 
        {
          str = filenames[loop];
        } 
        write("Nombre ambiguo, usando: " + str + "\n");
      }
    }
  }

  if (file_size(str) == -2) 
  {
    write("Es un directorio.\n");
    return 1;
  }

  if (file_size(str) == -1) 
  {
    write("No hay archivos coincidentes.\n");
    return 1;
  }
  
  if (!SECURE->valid_read(str, this_player()))
  {
    write("No tienes permisos para ver este fichero.\n");
    return 1;
  }

  ex_spool("y", str, 1);
  return 1;
}

int ex_spool(string yn, string fil, int linum)
{
  string s1;
  int i;
  mixed tnum;

  if (sscanf(yn, "%d", tnum) == 1)
  {
    linum = (int)tnum;
    if (linum < 1 ) 
      linum = 1;
  }

  if ( (yn == "Q") || (yn == "q") ) 
    return 1;

  for (i = 0; i < this_player()->query_rows(); linum++)
  {
    i++;
    s1 = read_file_line(fil, linum, 1);
    if ( !strlen(s1) && (s1 != "\n") )
    {
      write("\n");
      return 1;
    }

    write(sprintf("%4d: %s", linum, s1));
  }

  write(sprintf("Archivo: %s. Q para salir ", fil));
  input_to("ex_spool", 0, fil, linum);
  return 1;
}
