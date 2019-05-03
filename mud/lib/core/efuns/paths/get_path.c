
// old /global/path.c, now moved to simul_efuns, neverbot 03/2009
string get_path(varargs string str)
{
  string *array, *array1, temp;
  int i;

  if (!str)
    str = this_user()->query_role()->query_home_dir();

  // if (!strlen(str))
  //   return "/";

  if (str == "~")
  {
    str = this_user()->query_role()->query_home_dir();
  }
  else if (strlen(str) && (str[0] == '~'))
  {
    if (str[1] == '/')
    {
      sscanf(str, "~/%s", temp);
      str = this_user()->query_role()->query_home_dir() + temp;
    }
    else
    {
      string name;
      if (sscanf(str, "~%s/%s", name, str) != 2)
      {
        name = extract(str,1);
        str = "home/" + name;
      }
      else
      {
        // cheat at this point and just assume they are a wizard. sigh i know i know
        str = "home/" + name + "/" + str;
      }
    }
  }
  else if (!strlen(str) || (str[0] != '/'))
    str = this_user()->query_role()->query_current_path() + "/" + str + "/";

  if (str == "/")
    return "/";

  array = explode(str, "/") - ({ "" });

  for (i = 0; i < sizeof(array); i++)
  {
    if (array[i] == "..")
    {
      if (i < 1)
      {
          notify_fail("Invalid path.\n");
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
