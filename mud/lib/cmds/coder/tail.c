/** coded by Raskolnikov July 1996 **/
/** Vaeleth... tail algo mas flexible **/

#include <mud/cmd.h>

inherit CMD_BASE;

void setup()
{
  position = 1;
}

static int cmd(string str, object me, string verb) 
{
  string file, *filename, text;
  int range, lineas;
  string content;

  content = "";

  if (!strlen(str)) 
  {
    notify_fail("Uso: tail [-<número de líneas>] <fichero>\n");
    return 0;
  }

  if (sscanf(str, "-%d %s", range, file) == 2) 
  {
    filename = get_files(file);
    
    if (!sizeof(filename)) 
    {
      notify_fail("Fichero no válido.\n");
      return 0;
    }
    
    if (range < 0) 
    {
      notify_fail("Rango no válido: "+range+"\n");
      return 0;
    }
    
    if (!range) 
      range = 20;
    
    content = read_file(filename[0]);
    
    if (!content)
    {
      notify_fail("No se pudo leer el fichero... ¿demasiado largo?\n");
      return 0;
    }
    
    lineas = sizeof(explode(content,"\n"));
    text = read_file_line(filename[0], lineas-range, lineas);
    // printf("%-=*s", this_player()->query_cols(), text);
    tell_object(me, text);
    return 1;
  }
  else 
  {
    sscanf(str, "%s", file);
    filename = get_files(file);
    
    if (!sizeof(filename)) 
    {
      notify_fail("Fichero no válido.\n");
      return 0;
    }
    
    content = read_file(filename[0]);
    
    if (!content)
    {
      notify_fail("No se pudo leer el fichero... ¿demasiado largo?\n");
      return 0;
    }
    
    lineas = sizeof(explode(content,"\n"));
    range = 20;
    text = read_file_line(filename[0], lineas-range, lineas);
    // printf("%-=*s", this_player()->query_cols(), text);
    tell_object(me, text);
    return 1;
  }
} /* head_file() */

string query_short_help() 
{
  return "Muestra las últimas líneas de un archivo.";
  /*
  return "Este comando permite al usuario ver el final de "
    "un fichero, concretamente el rango de líneas especificado "
    "o bien las últimas 20 líneas del fichero que se quiera visualizar.\n";
  */
}

string query_usage() 
{
  return "tail -rango|fichero";
}
