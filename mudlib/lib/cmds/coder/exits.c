// Antiguo chkexits por Radiks
// Cambiado a exits por neverbot 2001
// Añadido parametros para poder comprobar que las salidas son correctas
//  en una serie de archivos (por ejemplo exits *), neverbot 04/2009

#include <mud/cmd.h>

inherit CMD_BASE;

#define R   "%^BOLD%^RED%^"
#define RE  "%^RESET%^"
#define C   "%^BOLD%^CYAN%^"
#define G   "%^BOLD%^GREEN%^"

string short_file_name(string file_name)
{
  string * pieces;
  pieces = explode(file_name, "/");
  return pieces[sizeof(pieces)-1];
}

static int cmd (string str, object me, string verb)
{
  mixed *dirs,dirs2;
  string aqui;     // nombre de la room environment
  int a;
  string * files;
  string ret;
  mapping contrarios;
  
  ret = "";
  files = ({ });
  
  contrarios = 
       (["norte":"sur","sur":"norte","oeste":"este",
       "este":"oeste","noroeste":"sudeste",
       "sudeste":"noroeste","noreste":"sudoeste",
       "sudoeste":"noreste","arriba":"abajo","abajo":"arriba",
       "dentro":"fuera","fuera":"dentro", "escaleras":"escaleras"]);

  if (!environment(me)) 
  {
    write("¡Sin entorno (environment) no puedes hacer eso!\n");
    return 1;
  }
  
  
  if (str && str != "")
  {
    files = get_files(str);
    if (sizeof(files))
    {
      int i;
      object ob;
      string err;
      
      for (i = 0; i < sizeof(files); i++)
      {
        if (err = catch(ob = load_object(files[i])) ) 
        {
          ret += "Archivo ("+short_file_name(files[i])+") no carga.\n";
          continue;
        }

        if (!ob) 
        {
          ret += "Archivo ("+short_file_name(files[i])+") no carga.\n";
          continue;
        }
        
        if (!ob->query_room())
        {
          ret += "Archivo ("+short_file_name(files[i])+") no es una room.\n";
          continue;
        }
        
        dirs = ob->query_dest_dir();
        
        if (!sizeof(dirs)) 
        {
          ret += "Archivo ("+short_file_name(files[i])+") no tiene salidas.\n";
            continue;
        } 
        else 
        {
        for(a = 0; a < sizeof(dirs); a+=2) 
        {
          // printf("%-10s -> ", dirs[a][0..8]);
          sscanf(dirs[a+1],"%s.c", dirs[a+1]);
            // dirs[a+1] = dirs[a+1]+".c";
          // write( dirs[a+1]+"\n");
        }

        for(a = 0; a < sizeof(dirs); a+=2) 
        {
          string ppp, match;
          int j;
          // printf("%-10s: ",dirs[a][0..8]);

          if (file_size(dirs[a+1]+".c") == -1) 
            ret += "Archivo ("+short_file_name(files[i])+") salida ("+dirs[a][0..8]+") lleva a archivo inexistente ("+short_file_name(dirs[a+1])+").\n";
            // write(dirs[a+1]+G+" no existe"+RE+".\n");
          else 
          {
            if (ppp = catch(dirs2 = dirs[a+1]->query_dest_dir())) 
              ret += "Archivo ("+short_file_name(files[i])+") salida ("+dirs[a][0..8]+") lleva a archivo que no carga ("+short_file_name(dirs[a+1])+").\n";
              // write(dirs[a+1]+G+" no carga"+RE+".\n");
            else 
            {
              match = "";
              if (sizeof(dirs2))
                for(j=0;j<sizeof(dirs2);j=j+2) 
                {
                  sscanf(dirs2[j+1],"%s.c",dirs2[j+1]); 
                  if (file_name(ob)==dirs2[j+1])
                    match = dirs2[j];
                }

              if (!match || match == "")
                ret += "Archivo ("+short_file_name(files[i])+") salida ("+dirs[a][0..8]+") lleva a archivo que no "+
                  "tiene salida de vuelta ("+short_file_name(dirs[a+1])+").\n";
                // write(dirs[a+1]+ R+" no tiene salida aquí"+RE+".\n");
              else 
              {
              if (contrarios[dirs[a]] != match)
                  ret += "Archivo ("+short_file_name(files[i])+") salida ("+dirs[a][0..8]+") lleva a archivo "+
                    "("+short_file_name(dirs[a+1])+") cuya salida de vuelta no corresponde ("+match+").\n";
                //write(dirs[a+1]+ " -> "+C+match[0..8]+""+RE+".\n");
              }
              }
          }
        }
        }          
      }
      if (ret == "")
        write( "Las salidas de todas las rooms son correctas.\n");
      else
        write( ret);
      return 1;
    }
    else // !sizeof(files)
    {
      write( "Sintaxis: exits (sin parámetros para comprobar tu room actual), o\n"+
        "      exits <archivos> (para comprobar una serie de ficheros)\n");
      return 1;      
    }
  }
  
  aqui = file_name(environment(me));

  write("----------------------------------------------------\n");
  write(" Nombre:  "+aqui+"\t");
  write(" Tamaño: "+file_size(aqui+".c")+" bytes\n");
  write("-- Salidas: ----------------------------------------\n");
  
  dirs = environment(me)->query_dest_dir();
  
  if (!sizeof(dirs)) 
  {
    write("No hay salidas en esta habitación.\n");
  } 
  else 
  {
    for(a = 0; a < sizeof(dirs) ;a += 2) 
    {
      write(sprintf("%-10s -> ", dirs[a][0..]));
      sscanf(dirs[a+1],"%s.c",dirs[a+1]);
            // dirs[a+1] = dirs[a+1]+".c";
      write(dirs[a+1]+"\n");
    }
  
    write("-- Estado de las salidas: --------------------------\n");
  
  for(a = 0;a < sizeof(dirs);a += 2) 
  {
    string ppp,match;
    int j;
    write(sprintf("%-10s: ", dirs[a][0..]));
      
    if (file_size(dirs[a+1]+".c") == -1) 
    {
      write(dirs[a+1]+G+" no existe"+RE+".\n");
    } 
    else 
    {
      if (ppp = catch(dirs2 = dirs[a+1]->query_dest_dir())) 
      {
        write(dirs[a+1]+G+" no carga"+RE+".\n");
        //write(ppp);
      } 
      else 
      {
        match = "";
        if (sizeof(dirs2))
          for(j=0;j<sizeof(dirs2);j=j+2) 
          {
            sscanf(dirs2[j+1],"%s.c",dirs2[j+1]); 
            if (file_name(environment(me))==dirs2[j+1])
              match = dirs2[j];
          }

        if (!match || match == "")
          write(dirs[a+1]+ R+" no tiene salida aquí"+RE+".\n");
        else 
        {
        if (contrarios[dirs[a]] == match)
          write("Ok.\n");
          else
          write(dirs[a+1]+ " -> "+C+match[0..]+""+RE+".\n");
        }
        }
    }
  }
  }
  
  write("----------------------------------------------------\n");

  return 1;
}
