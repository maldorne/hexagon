
static nomask string resolve_path(string path) 
{
  int i,j;
  string *dirs;

  if (!path) 
    return "";
  
  dirs = explode(path,"/");
  
  // First, remove any . from the array. 
  dirs -= ({ "." });

  for (i = 0; i < sizeof(dirs); i++) 
  {
    if (!dirs[i] || dirs[i]=="") 
      continue;
    
    if (dirs[i][0]=='~') 
    {
      if (!this_user())
        dirs[i] = "/home/admin";
      else if (strlen(dirs[i])==1) 
        dirs[i] = "/home/"+this_user()->query_name();
      else 
        dirs[i] = "/home/"+dirs[i][1..];
    }
  }

  // Now remove any .. and the preceding element. 
  i = member_array("..",dirs);
  while (i>-1) 
  {
    // Can't start with a ..  
    if (i==0) 
      return "";

    j = sizeof(dirs);

    // Piece it back together, depending on if we remove the first two
    //   elements, the last two, or two from the middle. 
    if (i==1) 
    {
      if (j==2) 
        return "";
      dirs = dirs[i+1 .. j-1];
    } 
    else 
    {
      if (i==j-1) 
        dirs = dirs[0 .. i-2];
      else 
        dirs = dirs[0 .. i-2] + dirs[i+1 .. j-1];
    }

    // Look for another .. in the path. 
    i = member_array("..",dirs);
  }
  path = implode(dirs,"/");
  return path;
}

/* absolute_path() returns 1 if the string is an absolute path (ie, it
   begins with ~ or /) and 0 if not. This is done in so many different
   commands that it seemed wisest to just put it in the auto object so
   you didn't have to change it in 30 places. The null string returns 0. */

static nomask int is_absolute_path(string str) 
{
    if (!str || str=="") 
      return FALSE;
    if (str[0]=='/' || str[0]=='~') 
      return TRUE;
    return FALSE;
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

// Antiguo /global/path.c, ahora extraido a simul_efuns, Folken 03/2009                       

string get_path(string str) 
{
  string *array, *array1, temp;
  int i;

  if (!str) 
  {
    str = this_player()->query_home_dir();
  }

  if (str == "~") 
  {
    str = this_player()->query_home_dir();
  }
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

string *get_files(string str) 
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

