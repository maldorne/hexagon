/*
 * The great and wonderful alias driver by me ;)
 * pinkfish.
 */

mapping map_nicknames;

void create()
{
  map_nicknames = ([ ]);
}

void nickname_commands()
{
  add_private_action("delete_nickname", "quitarapodo");
  add_private_action("delete_nickname", "dnick");

  // add_private_action("nickname", "nickname");
  add_private_action("nickname", "apodo");
  add_private_action("nickname", "apodos");
  add_private_action("nickname", "nick");

  // add_private_action("flushnicknames","flushnicknames");
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

string _print_nicknames()
{
  int i, cols;
  string str, str1, str2, bit, *tmp, ret;

  bit = "";
  str1 = "";
  str2 = "";
  ret = "";

  if (!map_nicknames)
    map_nicknames = ([ ]);

  tmp = m_indices(map_nicknames);
  cols = (int)this_user()->query_cols();

  for (i = 0; i < sizeof(tmp); i++)
  {
    str = tmp[i]+": "+map_nicknames[tmp[i]]+"  ";

    if (strlen(str) > 39)
      ret += sprintf(" " + tmp[i]+": %-=*s\n", cols - strlen(tmp[i]) - 2, map_nicknames[tmp[i]]);
    else if (strlen(str) > 19)
      str1 += str + "\n";
    else
      str2 += str + "\n";
  }

  if (strlen(str1))
    ret += sprintf("%-#*s\n", cols, str1);

  if (strlen(str2))
    ret += sprintf("%-#*s\n", cols, str2);

  return ret;
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
  {
    string line;
    line = sprintf("%p%|*s\n", '-', this_user()->query_cols(), "");

    write(line + " * Lista de apodos:\n" + line);
    write(_print_nicknames());
    write(line + " Utiliza 'quitarapodo <nombre>' para borrarlo\n" + line);
    return 1;
  }

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
