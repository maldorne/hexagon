// Old chkexits by Radiks
// Moved to exits por neverbot 2001
// Parameters added to check specific files (i.e. exits *), neverbot 04/2009

#include <mud/cmd.h>
#include <translations/exits.h>

inherit CMD_BASE;

#define R   "%^BOLD%^RED%^"
#define RE  "%^RESET%^"
#define C   "%^BOLD%^CYAN%^"
#define G   "%^BOLD%^GREEN%^"

string query_usage()
{
  return "exits [files]";
}

string query_help()
{
  return "Checks the exits of a room or multiple rooms.\n" +
         "Input no files to check your current environment.";
}

string short_file_name(string file_name)
{
  string * pieces;
  pieces = explode(file_name, "/");
  return pieces[sizeof(pieces)-1];
}

static int cmd(string str, object me, string verb)
{
  mixed *dirs,dirs2;
  string here; // environment name
  int a;
  string * files;
  string ret;
  object user;
  
  ret = "";
  files = ({ });
  
  if (!environment(me)) 
  {
    write("You can't do that without an environment\n");
    return 1;
  }
  
  if (strlen(str))
  {
    files = get_files(str);
    if (sizeof(files))
    {
      int i;
      object ob;
      string err;
      
      for (i = 0; i < sizeof(files); i++)
      {
        if ((err = catch(ob = load_object(files[i])) ) || (!ob)) 
        {
          ret += "File ("+short_file_name(files[i])+") cannot be loaded.\n";
          continue;
        }

        if (!ob->query_room())
        {
          ret += "File ("+short_file_name(files[i])+") is not a room.\n";
          continue;
        }
        
        dirs = ob->query_dest_dir();
        
        if (!sizeof(dirs)) 
        {
          ret += "File ("+short_file_name(files[i])+") has no exits.\n";
          continue;
        } 
        else 
        {
          for (a = 0; a < sizeof(dirs); a+=2) 
          {
            // printf("%-10s -> ", extract(dirs[a],0,8));
            sscanf(dirs[a+1], "%s.c", dirs[a+1]);
            // dirs[a+1] = dirs[a+1] + ".c";
            // write(dirs[a+1]+"\n");
          }

          for (a = 0; a < sizeof(dirs); a+=2) 
          {
            string error, match;
            int j;
            // printf("%-10s: ",extract(dirs[a],0,8));

            if (file_size(dirs[a+1] + ".c") == -1) 
              ret += "File (" + short_file_name(files[i]) + ") exit (" + extract(dirs[a], 0, 8) + 
                ") goes to non existant file (" + short_file_name(dirs[a+1]) + ").\n";
            else 
            {
              if (error = catch(dirs2 = dirs[a+1]->query_dest_dir())) 
                ret += "File (" + short_file_name(files[i]) + ") exit (" + extract(dirs[a], 0, 8) + 
                  ") goes to file that cannot be loaded (" + short_file_name(dirs[a+1]) + ").\n";
              else 
              {
                match = "";
                if (sizeof(dirs2))
                  for (j = 0; j < sizeof(dirs2); j = j+2) 
                  {
                    sscanf(dirs2[j+1], "%s.c", dirs2[j+1]); 
                    if (file_name(ob) == dirs2[j+1])
                      match = dirs2[j];
                  }

                if (!match || match == "")
                  ret += "File (" + short_file_name(files[i]) + ") exit (" + extract(dirs[a], 0, 8) + 
                    ") goes to a file that does not come back (" + short_file_name(dirs[a+1]) + ").\n";
                else 
                {
                  if (OPPOSITES[dirs[a]] != match)
                    ret += "File (" + short_file_name(files[i]) + ") exit (" + extract(dirs[a], 0, 8) + 
                      ") goes to file (" + short_file_name(dirs[a+1]) + ") with wrong backwards exit (" + match + ").\n";
                }
                }
            }
          }
        }          
      }

      if (!strlen(ret))
        write("All exits in every room are right.\n");
      else
        write(ret);
      return 1;
    }
    else // !sizeof(files)
    {
      write(query_help());
      return 1;      
    }
  }

  // from here we are checking only our current environment
  
  here = file_name(environment(me));
  user = me->user();


  ret += sprintf("%p%|*s\n", '-', user->query_cols(), "");
  ret += " File name:  "+here+"\t";
  ret += " Size: "+file_size(here+".c")+" bytes\n";
  ret += sprintf("%p%|*s\n", '-', user->query_cols(), "");
  
  dirs = environment(me)->query_dest_dir();
  
  if (!sizeof(dirs)) 
  {
    ret += "This room has no exits.\n";

  } 
  else 
  {
    for (a = 0; a < sizeof(dirs); a += 2) 
    {
      string error, match;
      int j;
      ret += sprintf("  %-9s -> ", dirs[a][0..]);
      sscanf(dirs[a+1], "%s.c", dirs[a+1]); // remove the extension

      ret += dirs[a+1] + " ";

      if (file_size(dirs[a+1] + ".c") == -1) 
      {
        ret += "" + R + "does not exist" + RE + ".\n";
      } 
      else if (error = catch(dirs2 = dirs[a+1]->query_dest_dir()))
      {
        ret += "" + R + "does not load" + RE + ".\n";
      }
      else
      {
        match = "";
        if (sizeof(dirs2))
        {
          for(j = 0; j < sizeof(dirs2); j = j+2) 
          {
            sscanf(dirs2[j+1], "%s.c", dirs2[j+1]); // remove the extension
            if (file_name(environment(me)) == dirs2[j+1])
              match = dirs2[j];
          }
        }

        if (!strlen(match))
          ret += dirs[a+1] + R + " has not backwards exit" + RE + ".\n";
        else 
        {
          if (OPPOSITES[dirs[a]] == match)
            ret += "" + G + "is ok" + RE + " (" + match + ").\n";
          else
            ret += " -> " + C + match[0..] + "" + RE + ".\n";
        }
      }
    }
    
    /*
    for (a = 0; a < sizeof(dirs); a += 2) 
    {
      string error, match;
      int j;
      write(sprintf("%-10s: ", dirs[a][0..]));
        
      if (file_size(dirs[a+1] + ".c") == -1) 
      {
        write(dirs[a+1]+G+" no existe"+RE+".\n");
      } 
      else 
      {
        if (error = catch(dirs2 = dirs[a+1]->query_dest_dir())) 
        {
          write(dirs[a+1]+G+" no carga"+RE+".\n");
          //write(error);
        } 
        else 
        {
          match = "";
          if (sizeof(dirs2))
            for (j=0;j<sizeof(dirs2);j=j+2) 
            {
              sscanf(dirs2[j+1],"%s.c",dirs2[j+1]); 
              if (file_name(environment(me))==dirs2[j+1])
                match = dirs2[j];
            }

          if (!match || match == "")
            write(dirs[a+1]+ R+" no tiene salida aquí"+RE+".\n");
          else 
          {
          if (OPPOSITES[dirs[a]] == match)
            write("Ok.\n");
            else
            write(dirs[a+1]+ " -> "+C+match[0..]+""+RE+".\n");
          }
        }
      }
    }
    */
  }

  ret += sprintf("%p%|*s\n", '-', user->query_cols(), "");
  write(ret);

  return 1;
}
