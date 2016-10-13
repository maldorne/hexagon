
#include <mud/secure.h>

nomask mixed creator_file(string file, varargs int author) 
{
  string *str;

  if (!file || !stringp(file)) 
    return NOBODY_EUID;
  
  str = explode(file, "/") - ({ "" });
  if (sizeof(str) < 2) 
    return ROOT;
  
  switch (str[0]) 
  {
    case "lib" :
      return ROOT;

    case "net" :
      // return "Network stuff";
      return NETWORK_EUID;

    case "game" :
      if (sizeof(str) <= 3)
        return ROOT;

      //if (!author)
        // return capitalize(str[2]);
      return str[2];
     
      // return ("/d/"+str[1]+"/master")->author_file(str);

    case "tmp" :
     return TMP_EUID;

    case "home" :
      if (sizeof(str) < 3)
        return ROOT;
      return str[1];

    default:
      return ROOT;    
  }
}

nomask mixed author_file(mixed bing) 
{
  return creator_file(bing, 1);
}

nomask mixed domain_file(mixed bing) 
{
  string str;

  str = creator_file(bing);
  
  if (!str) 
    return str;
  
  if ((str[0] >= 'A' && str[0] <= 'Z') ||
      (str[0] >= 'a' && str[0] <= 'z'))
    return str;
  
  return "mud";
}
