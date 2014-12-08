
#include <user/alias.h>

int convert();

mapping aliases, map_aliases;
static mapping doing_alias;

void create()
{
  aliases = ([ ]);
  map_aliases = ([ ]);
  doing_alias = ([ ]);
}

void init() 
{
  add_action("alias", "alias");
  add_action("unalias", "unalias");
  add_action("flushalias","flushalias");
}

mixed *compile_alias(string str) 
{
    mixed *ret;
    int i, space;
    string *frog, s1;
    int tmp, gumby, nodollar;
    mixed *ifargs;

    str = implode(explode(str," ")," ");
    /* Quick hack.. Baldrick. */
    //if (!this_player()->query_coder())
    if(0)
    {
      frog = explode(str,";");
      if (sizeof(frog) > 1)
      {
        tell_object(this_player(),"\";\" no permitido en los alias\n");
        return ({ });
      }
    }
    else
      str = implode(explode(str,";"), "$new_line$");

    str = "&"+str+"&";
    frog = explode(str, "$");
    
    if (frog[0] == "&")
      frog = delete(frog, 0, 1);
    else
      frog[0] = frog[0][1..100];
    
    s1 = frog[sizeof(frog)-1];
    
    if (s1 == "&")
      frog = delete(frog, sizeof(frog)-1, 1);
    else
      frog[sizeof(frog)-1] = s1[0..strlen(s1)-2];

    ret = ({ });
    ifargs = ({ });
    nodollar = 1;
    
    for (i = 0; i < sizeof(frog); i++)
      if (frog[i] == "new_line") 
      {
        ret += ({ NEW_LINE });
        nodollar = 1;
      } 
      else if (frog[i] == "*") 
      {
        ret += ({ ALL_ARGS });
        gumby = 1;
        nodollar = 1;
      } 
      else if (frog[i][0..4] == "ifarg") 
      {
        if (sscanf(frog[i], "ifarg%d:%s", tmp, s1) == 2) 
        {
          if (tmp < 0)
            tmp = 0;
          if (tmp > ALIAS_MASK)
            tmp = ALIAS_MASK;

          ret += ({ IFARG_THING+ tmp, 0, "" });
          frog[i--] = s1;
          nodollar = 1;
          ifargs += ({ sizeof(ret)-2 });
          space = 0;
          gumby = 1;
        } 
        else if (frog[i][5] == ':') 
        {
          ret += ({ ALL_IFARG, 0, "" });
          // Needed to be separated.. not sure why. Flode - 040398
          // frog[i] = frog[i--][6..100];
          frog[i] = frog[i][6..100];
          i--;
          nodollar = 1;
          ifargs += ({ sizeof(ret)-2 });
          space = 0;
          gumby = 1;
        } 
        else if (sizeof(ret) && stringp(ret[sizeof(ret)-1]) && !space)
          ret[sizeof(ret)-1] += "$"+frog[i];
        else if (nodollar) 
        {
          ret += ({ frog[i] });
          nodollar = 0;
        } 
        else
          ret += ({ "$"+frog[i] });   
      } 
      else if (frog[i][0..2] == "arg") 
      {
        if (sscanf(frog[i], "arg%d:%s", tmp, s1) == 2) 
        {
          if (tmp < 0)
            tmp = 0;
          if (tmp > ALIAS_MASK)
            tmp = ALIAS_MASK;
          ret += ({ ARG_THING+ tmp, s1 });
          nodollar = 1;
        } 
        else if (frog[i][3] == ':') 
        {
          ret += ({ ALL_ARG, frog[i][4..100] });
          nodollar = 1;
        }
        else if (sizeof(ret) && stringp(ret[sizeof(ret)-1]) && !space)
          ret[sizeof(ret)-1] += "$"+frog[i];
        else if (nodollar) 
        {
          ret += ({ frog[i] });
          nodollar = 0;
        }
        else
          ret += ({ "$"+frog[i] });

        gumby = 1;
        space = 0;
      } 
      else if (frog[i] == "else" && sizeof(ifargs)) 
      {
        ret[ifargs[sizeof(ifargs)-1]] = sizeof(ret)-ifargs[sizeof(ifargs)-1]+1;
        ret += ({ ELSE_THING, 0, "" });
        ifargs[sizeof(ifargs)-1] = sizeof(ret)-2;
        nodollar = 1;
      } 
      else if (strlen(frog[i]) && frog[i][strlen(frog[i])-1] == '*' &&
             sscanf(frog[i], "%d", tmp) == 1) 
      {
        if (tmp < 0)
          tmp = 0;
        if (tmp > ALIAS_MASK)
          tmp = ALIAS_MASK;
        ret += ({ FROM_ARG + tmp });
        gumby = 1;
        nodollar = 1;
      } 
      else if (strlen(frog[i]) && frog[i][0] == '*' &&
             sscanf(frog[i][1..1000], "%d", tmp) == 1) 
      {
        if (tmp < 0)
          tmp = 0;
        if (tmp > ALIAS_MASK)
          tmp = ALIAS_MASK;
        ret += ({ TO_ARG + tmp });
        gumby = 1;
        nodollar = 1;
      } 
      else if (sscanf(frog[i], "%d", tmp) == 1) 
      {
        if (tmp < 0)
          tmp = 0;
        if (tmp > ALIAS_MASK)
          tmp = ALIAS_MASK;
        ret += ({ ONE_ARG + tmp });
        gumby = 1;
        nodollar = 1;
      } 
      else 
      {
        if (!nodollar)
          frog[i] = "$"+frog[i];
        nodollar = 0;
        space = 0;
        if (strlen(frog[i]) && frog[i][strlen(frog[i])-1] == '~')
          if (sizeof(ifargs)) 
          {
            if (strlen(frog[i]) == 1)
              frog[i] = "";
            else
              frog[i] = frog[i][0..strlen(frog[i])-2];
            /* create an offset */
            ret[ifargs[sizeof(ifargs)-1]] = sizeof(ret)-
                   ifargs[sizeof(ifargs)-1];
            ifargs = delete(ifargs, sizeof(ifargs)-1, 1);
            nodollar = 1;
            space = 1;
          }

        if (sizeof(ret) && stringp(ret[sizeof(ret)-1]) && space != 2)
          ret[sizeof(ret)-1] += frog[i];
        else  
          ret += ({ frog[i] });

        if (space)
          space = 2;
      }
        
    if (!gumby) 
    {
      if (sizeof(ret) && !stringp(ret[sizeof(ret)-1]) || space)
        ret += ({ " ", ALL_ARGS });
      else 
      {
        ret[sizeof(ret)-1] += " ";
        ret += ({ ALL_ARGS });
      }
    }
    return ret;
}

string alias_string(mixed *al) 
{
  int i, num, *add_thing;
  string str;

  str = "";
  add_thing = ({ });
  
  if (!pointerp(al))
    return "";
    
  for (i=0;i<sizeof(al);i++) 
  {
    if (stringp(al[i]))
      str += al[i];
    else 
    {      
      num = al[i] & ALIAS_MASK;
      
      switch (al[i] - num) 
      {
        case NEW_LINE  : str += ";";
                         break;
        case ALL_ARGS  : str += "$*$";
                         break;
        case ONE_ARG   : str += "$"+num+"$";
                         break;
        case TO_ARG    : str += "$*"+num+"$";
                         break;
        case FROM_ARG  : str += "$"+num+"*$";
                         break;
        case ALL_ARG   : str += "$arg:"+al[++i]+"$";
                         break;
        case ARG_THING : str += "$arg"+num+":"+al[++i]+"$";
                         break;
        case ELSE_THING :
                         str += "$else$";
                         add_thing[sizeof(add_thing)-1] = i+1+al[++i];
                         break;
        case ALL_IFARG : str += "$ifarg:";
                         add_thing += ({ i+1+al[++i] });
                         break;
        case IFARG_THING :
                         str += "$ifarg"+num+":";
                         add_thing += ({ i+1+al[++i] });
                         break;
      }
    }
    
    if (member_array(i, add_thing) != -1)
      str += "~$";
  }
  return str;
}

int flushalias(string str)
{
  write("Esto borrará todos tus alias, ¿estás seguro? (s/n)\n");
  input_to("flushem",0);
  return 1;
}

static int flushem(string str)
{
  if(str == "S" || str == "s")
  {
    aliases = ([ ]);
    map_aliases = ([ ]);
    write("Alias borrados.\n");
  }
  else 
    write("En otra ocasión entonces.\n");

  return 1;
}

int print_aliases() 
{
  int i;
  string str,str1,str2, *tmp, bing;
 
  /* ahh well here goes the clean. you dont want to know what used to
   * be here ;)
   */
  if (!m_sizeof(aliases)) 
  {
    notify_fail("Ningún alias definido.\n");
    return 0;
  }
 
  str1 = "";
  str2 = "";
  tmp = m_indices(aliases);
  tmp = sort_array(tmp);

  for (i=0;i<sizeof(tmp);i++) 
  {
    bing = alias_string(aliases[tmp[i]]);
    str = tmp[i]+": "+bing;
    if (strlen(str)>39)
      sprintf(tmp[i]+": %-=*s\n", (int)this_player()->query_cols()-
                                      strlen(tmp[i])-2, bing);
    else if (strlen(str)>19)
      str1 += str+"\n";
    else
      str2 += str+"\n";
  }
  
  if (strlen(str1))
    sprintf("%-#*s\n", this_player()->query_cols(), str1);

  if (strlen(str2))
    sprintf("%-#*s\n", this_player()->query_cols(), str2);

  return 1;
}

static int alias(string str, int bing) 
{
  string s1, s2;
  mixed *boos;

  if (!mappingp(aliases))
    aliases = ([ ]);

  if (map_aliases && !bing)
    convert();

  if (!str)
    return print_aliases();

  if(sscanf(str, "%s %s", s1, s2) != 2)
  {
    if (!aliases[str])
    {
      notify_fail("No hay ningún alias '"+str+"' definido.\n");
      return 0;
    }
    sprintf("%s: %-=*s\n", str, (int)this_player()->query_cols() -
                          strlen(str) -2, alias_string(aliases[str]));
    return 1;
  }
  
  /* Bishop - somehow this can happen */
  if (!s1) 
    return 1;

  /* Add by Baldrick, we don't want long aliases.. 
   * May '95 
   * Taniwha 09/95 checked for too many as well.
   */
  if (!this_player()->query_coder())
  {
    if(m_sizeof(aliases) > MAXALIAS )
    {
       write("Demasiados alias.\n");
       return 1;
    }
    if(strlen(s2) > MAXALIASLEN)
    {
      tell_object(this_player(),"Alias demasiado largo.\n");
      return 1;
    }
  }
  
  if (!aliases[s1]) 
  {
    if (!sizeof(boos = compile_alias(s2)))
    {
      notify_fail("Eso ya no está permitido.\n");
      return 0;
    }
    
    aliases[s1] = boos;
    write("Incluido el alias '"+s1+"'.\n");
  } 
  else 
  {
    if (!sizeof(boos = compile_alias(s2)))
    {
      notify_fail("Eso ya no está permitido.\n");
      return 0;
    }
    
    aliases[s1] = boos;
    write("Cambiado el alias '"+s1+"'.\n");
  }
  return 1;
}

static int unalias(string str) 
{
  if (!mappingp(aliases))
    aliases = ([ ]);
  if (map_aliases)
    convert();

  if (!str) 
  {
    notify_fail("Sintaxis: "+query_verb()+" <alias>\n");
    return 0;
  }

  if (!aliases[str]) 
  {
    notify_fail("El alias '"+str+"' no existe, no puedes eliminarlo.\n");
    return 0;
  }
  
  aliases = m_delete(aliases, str);
  write("Borrado alias '"+str+"'.\n");
  return 1;
}

void test_remove_alias(string str)
{
  aliases = m_delete(aliases,str);
}

mapping query_aliases() { return aliases; }

static int exec_alias(string verb, string args) 
{
  int i, num;
  string *bits, line;
  mixed stuff;

  /* Moved to process input
  if (verb[0] == '.')
  {
    this_object()->set_trivial_action();
    if (args)
      this_object()->action_check(expand_history(verb[1..100])+args);
    else
      this_object()->action_check(expand_history(verb[1..100]));
    return 1;
  }
  */
  
  if (!mappingp(aliases))
      aliases = ([ ]);

  if (!aliases[verb])
      return 0;

  if (!doing_alias)
      doing_alias = ([ ]);

  if (doing_alias[verb]) 
  {
    notify_fail("¿Alias recursivos? Eso no está permitido.\n");
    return 0;
  }
  
  doing_alias[verb] = 1;
  // in_alias_command = 1;
  this_object()->set_in_alias_command(1);

  call_out("remove_alias_thing", 0, verb);
  stuff = aliases[verb];
  line = "";
  if (!args)
    args = "";
  bits = explode(verb+" "+args, " ");

  for (i=0;i<sizeof(stuff);i++)
    if (stringp(stuff[i]))
      line += stuff[i];
    else 
    {
      num = stuff[i] & ALIAS_MASK;
      switch (stuff[i] - num) 
      {
        case NEW_LINE :
          /* command queue don't like trailing spaces */
          line = trim(line);
          // while (line != "" && line[<1] == ' ')
          //   line = line[0..<2];
          this_object()->action_check(line);
          line = "";
          break;
        case ALL_ARGS : 
          line += args;
          break;
        case ONE_ARG  : 
          if (num < sizeof(bits))
            line += bits[num];
          break;
        case TO_ARG   : 
          line += implode(bits[1..num], " ");
          break;
        case FROM_ARG : 
          line += implode(bits[num..100], " ");
          break;
        case ALL_ARG  : 
          i++;
          if (args == "")
            line += stuff[i];
          else
            line += args;
          break;
        case ARG_THING : 
          i++;
          if (num < sizeof(bits))
            line += bits[num];
          else
            line += stuff[i];
          break;
        case ALL_IFARG : 
          i++;
          if (args == "")
            i += stuff[i];
          break;
        case IFARG_THING :
          i++;
          if (num >= sizeof(bits))
            i += stuff[i];
          break;
        case ELSE_THING :
          i++;
          i += stuff[i];
          break;
      }
    }
  // command queue don't like trailing spaces 
  // while (line != "" && line[<1] == ' ')
  //   line = line[0..<2];
  line = trim(line);

  ///  if (line != "") check(line);

  if (line != "") 
    this_object()->action_check(line);

  doing_alias = m_delete(doing_alias, verb);
  // in_alias_command = 0;
  this_object()->set_in_alias_command(0);
  this_object()->set_trivial_action();
  return 1;
}

void remove_alias_thing(string verb) 
{
  // in_alias_command = 0;
  this_object()->set_in_alias_command(0);
  doing_alias = m_delete(doing_alias, verb);
}

int convert() 
{
  int i;
  string *str;

  str = m_indices(map_aliases);
  
  for (i=0;i<sizeof(str);i++)
    alias(str[i]+" "+map_aliases[str[i]], 1);
    
  map_aliases = ([ ]);
  return 1;
}

mixed * stats() 
{
  return ({ 
    ({ "Command Aliases", aliases, }),
          });
}
