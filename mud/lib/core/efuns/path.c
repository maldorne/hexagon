
#include "/lib/core/efuns/paths/resolve_path.c"

// idea taken from melville mudlib
static nomask int is_absolute_path(string str) 
{
    if (!str || (str == "")) 
      return -1;

    if ((str[0] == '/') || (str[0] == '~')) 
      return 1;

    return -1;
}

string get_path_file_name(string path)
{
  string * words; 
  words = explode(path, "/");

  if (sizeof(words) == 0)
    return "";

  if (sizeof(words) == 1)
    return words[0];

  return words[sizeof(words)-1];
}

string get_path_only(string path)
{
  string * words; 
  words = explode(path, "/");
  
  if (sizeof(words) == 0)
    return "";

  if (sizeof(words) == 1)
    return "/" + words[0] + "/";

  return "/" + implode(words[0..sizeof(words)-2], "/") + "/";
}

// old /global/path.c, now moved to simul_efuns, neverbot 03/2009                       
string get_path(string str) 
{
  string *array, *array1, temp;
  int i;

  if (!str) 
    str = this_player()->query_home_dir();

  if (str == "~") 
    str = this_player()->query_home_dir();
  else if(str[0] == '~') 
  {
    if(str[1] == '/') 
    {
      sscanf(str, "~/%s", temp);
      str = this_player()->query_home_dir() + temp;
    }
    else 
    {
      string name;
      if (sscanf(str, "~%s/%s", name, str) != 2) 
      {
        name = extract(str,1);
        str = "w/" + name;
      } 
      else
      {
        /* cheat at this point and just assume they are a wizard. sigh i know i know */
        str = "w/" + name + "/" + str;
      }
    }
  } 
  else if (str[0] != '/')
    str = this_player()->query_current_path() + "/" + str + "/";

  if (str == "/")
    return "/"; 
 
  array = explode(str, "/") - ({ "" });

  for (i = 0; i < sizeof(array); i++)
  {
    if (array[i] == "..") 
    {
      if (i < 1)
      {
          write("Path inválido.\n");
          return "";
      }
      
      if (i == 1)
        array1 = ({ "." });
      else
        array1 = array[0..i-2];
        
      if (i + 1 <= sizeof(array)-1) 
        array1 += array[i+1..sizeof(array)-1];
      
      array = array1;
      i -= 2;
    } 
    else if (array[i] == ".")
      array[i] = "";
 }
 
 if (array)
   str = implode(array, "/");
 else
   str = "";

  return "/"+str;
}  

string * get_files(string str) 
{
  mixed * info;

  info = get_dir(str);

  return info[0];

  /*

    int loop, count;
    string *filenames, rmpath, temp1, temp2, *names, *files;

    names = explode(str, " ") - ({ "" });

    if(sizeof(names) == 0) 
    {
      return ({ });
    }
    
    filenames = ({ });
    
    for(count = 0; count < sizeof(names); count++) 
    {
      str = names[count];
      str = rmpath = get_path(str);
    
      if(rmpath == "/") 
      {
        filenames += ({ "/" });
        continue;
      }

      if(!rmpath)
        rmpath = "/";
      if(!str) 
        str = "/";

      if(sscanf(rmpath, "%s/%s", temp1, temp2) == 2) 
      {
          string *path_elements;
 
          path_elements = explode(rmpath, "/") - ({ "" });
          rmpath = implode(path_elements[0..sizeof(path_elements) - 2], "/");
      }
    
      // if (this_player() != nil)
      //   seteuid(geteuid(this_player(1)));
      // else
      //   seteuid((string)master()->get_root_uid());
      
      files = get_dir(str);
      
      seteuid(0);

      if(files) 
      {
        int loop2;
        for (loop2 = 0; loop2 < sizeof(files); loop2++) 
        {
          filenames += ({ rmpath + "/" + files[loop2] });
        }
      }
    }

    for (loop = 0; loop < sizeof(filenames); loop++) 
    {
      if(filenames[loop][0] != '/') {
        filenames[loop] = "/" + filenames[loop];
      }
    }
    return filenames;
    */
}

// Changed to be *.* so as to handle virtual wombles as well
// string *get_cfiles(string str) 
// {
//   int loop;
//   string temp, *names, temp2;

//   names = explode(str, " ") - ({ "" });

//   for(loop = 0; loop < sizeof(names); loop++) 
//   {
//     if(sscanf(names[loop], "%s.%s", temp, temp2) != 2) 
//     {
//       names[loop] += ".c";
//     }
//   }
  
//   str = implode(names, " ");
//   return get_files(str);
// }

