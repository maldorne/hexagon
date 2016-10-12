/*
 * The great and wonderful alias driver by me ;)
 * pinkfish.
 */
 
mapping map_nicknames;
 

void create()
{
  map_nicknames = ([ ]);
}

void init() 
{
  add_action("delete_nickname", "quitarapodo");
  add_action("delete_nickname", "dnick");

  // add_action("nickname", "nickname");
  add_action("nickname", "apodo");
  add_action("nickname", "apodos");
  add_action("nickname", "nick");

  // add_action("flushnicknames","flushnicknames");
}

string *query_nicknames() { return keys(map_nicknames) + ({ }); }

int add_nickname(string str) 
{
  string s1,s2;
 
  if (sscanf(str,"%s %s",s1,s2)!=2) 
  {
    notify_fail("Sintaxis: apodo <apodo> <nombre>\n");
    return 0;
  }

  if (!map_nicknames)
    map_nicknames = ([ ]);

  if (!map_nicknames[s1]) 
  {
    map_nicknames[s1] = s2;
    write("Apodo '"+s1+"' añadido como '"+s2+"'.\n");
    return 1;
  }

  write("Apodo '"+s1+"' cambiado de '"+map_nicknames[s1]+"' a '"+s2+"'.\n");
  map_nicknames[s1] = s2;
  return 1;
}
 
string expand_nickname(string str) 
{
  string *array;
  int i;
 
  if (stringp(str))
    array = explode(lower_case(str)," ");
  else
    return str;

  if (!map_nicknames)
    map_nicknames = ([ ]);

  for (i=0;i<sizeof(array);i++)
    if (map_nicknames[array[i]])
      array[i] = map_nicknames[array[i]];

  return implode(array," ");
}
 
int delete_nickname(string str) 
{
  if (!map_nicknames)
    map_nicknames = ([ ]);

  if (!map_nicknames[str]) 
  {
    notify_fail("Ese apodo no existe.\n");
    return 0;
  }

  map_nicknames = m_delete(map_nicknames, str);
  write("Apodo borrado: "+str+"\n");
  return 1;
}
 
int print_nicknames() 
{
  int i, cols;
  string str, str1, str2, bit, *tmp;
 
  bit = "";
  str1 = "";
  str2 = "";

  if (!map_nicknames)
    map_nicknames = ([ ]);

  tmp = m_indices(map_nicknames);
  cols = (int)this_player()->query_cols();

  tell_object(this_player(), "------------------------------------------------------\n");
  tell_object(this_player(), "  Lista de apodos:\n");
  tell_object(this_player(), "------------------------------------------------------\n\n");

  for (i = 0; i < sizeof(tmp); i++) 
  {
    str = tmp[i]+": "+map_nicknames[tmp[i]]+"  ";

    if (strlen(str)>39)
      printf(tmp[i]+": %-=*s\n", cols - strlen(tmp[i]), map_nicknames[tmp[i]]);
    else if (strlen(str) > 19)
      str1 += str+"\n";
    else
      str2 += str+"\n";
  }

  if (strlen(str1))
    printf("  %-#*s\n", cols, str1);

  if (strlen(str2))
    printf("  %-#*s\n", cols, str2);

  tell_object(this_player(), "\n------------------------------------------------------\n");
  tell_object(this_player(), "  Utiliza 'quitarapodo <nombre>' para borrarlo\n");
  tell_object(this_player(), "------------------------------------------------------\n");

  return 1;
}
 
int nickname(string str) 
{
  string s1,s2;
 
  if ((!strlen(str)) && !map_sizeof(map_nicknames))
  {
    notify_fail("No tienes apodos definidos.\n");
    return 0;
  }
  
  if (!strlen(str))
    return print_nicknames();
 
  if (sscanf(str,"%s %s",s1,s2)!=2) 
  {
    if (!map_nicknames[str]) 
    {
      notify_fail("Ese apodo no existe.\n");
      return 0;
    }

    write("El apodo '"+str+"' equivale a '"+map_nicknames[str]+"'.\n");
    return 1;
  }
  
  return add_nickname(str);
}

/*
int flushnicknames(string str)
{
  write("Esto borrará tu lista de apodos completa. ¿Estás seguro? (s/n)\n");
  input_to("flushnicks",0);
  return 1;
}

int flushnicks(string str)
{
  if (str=="Y"||str=="y"||str=="S"||str="s")
  {
    map_nicknames=0;
    write ("Apodos borrados.\n");
  }
  else write("Quizá otro día.\n");
  return 1;
}
*/

mixed * stats() 
{
  return ({ 
    ({ "Nicknames", map_nicknames, }),
          });
}
