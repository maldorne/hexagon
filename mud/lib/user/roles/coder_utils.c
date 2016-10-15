

// prototypes
object *wiz_present(string str, object onobj, varargs int nogoout);

static string desc_object(mixed o)
{
  string str;

  if (!o) 
    return "** Espacio-Vacio **";
  
  if (!catch(str = (string)o->short()) && strlen(str)) 
    return str;
  
  if (!catch(str = (string)o->query_name()) && strlen(str)) 
    return str;
  
  return file_name(o);
} 

static string desc_f_object(object o)
{
  string str; //, tmp;

  str = desc_object(o);

  if (o && (str != file_name(o))) 
  {
    // if (tmp)
    //   str += " (" + tmp + ")";
    // else
      str += " (" + file_name(o) + ")";
  }

  return str;
}

void inform_of_call(object ob, mixed *argv) 
{
  // Arggghhh!  This is annoying me.
  string str;
  int i;
  
  str = this_player()->query_cap_name() + " calls " + argv[0] + "(";
  for (i = 1; i < sizeof(argv)-1; i++) 
  {
    // str += replace(sprintf("%O", argv[i]), "\n", " ");
    str += replace(replace(to_string(argv[i]), " ", ""), "\n", " ");
    if (i < sizeof(argv)-2)
      str += ",";
    //else
    //  str += " ";
  }

  if (ob)
    event(coders(), "inform", str + ") on " + desc_object(ob), 
                    "calls", this_object());

} /* inform_of_call() */

static mixed *parse_args(string str, string close) 
{
  mixed *args, *m, *m2;
  object *obs;
  string s1, s2, s3, s4, s5, s6, s7;
  int i;
  mapping map;

  args = ({ });

  while (strlen(str)) 
  {
    while (strlen(str) && str[0] == ' ') 
      str = str[1..];

    if (!strlen(str) || str[0..0] == close) 
      return ({ args, str[1..] });

    switch (str[0]) 
    {
      case '\'' :
        if (sscanf(str, "'%s'%s", s1, s2) != 2) 
        {
          write("Cadena no terminada (Unterminated String).\n");
          return ({ });
        }
        args += ({ replace(s1, "\\n", "\n") });
        str = s2;
        break;

      case '`' :
        if (sscanf(str, "`%s`%s", s1, s2) != 2) 
        {
          write("Cadena no terminada (Unterminated String).\n");
          return ({ });
        }
        args += ({ replace(s1, "\\n", "\n") });
        str = s2;
        break;

      case '"' :
        if (sscanf(str, "\"%s\"%s", s1, s2) != 2) 
        {
          write("Cadena no terminada (Unterminated String).\n");
          return ({ });
        }
        args += ({ replace(s1, "\\n", "\n") });
        str = s2;
        break;

      case '{' :
        m = parse_args(str[1..], "}");
        if (!m)
          return ({ });
        args += ({ m[0] });
        str = m[1];
        break;

      case '[' :
        str = str[1..];
        map = ([ ]);
        while (1) 
        {
          m = parse_args(str, ":");
          /* Ok...  if we cannot find another : maybe we are at the end? */
          if (!m) {
            while (strlen(str) && str[0] == ' ') 
              str = str[1..];
            if (str[0] == ']')
              break;
          }
          if (!(m2 = parse_args(str, ","))) 
          {
            if (!(m2 = parse_args(str, "]")))
              return ({ });
            if (sizeof(m[0]))
              map[m[0][0]] = (sizeof(m2[0])?m2[0][0]:0);
            break;
          }
          if (sizeof(m[0]))
            map[m[0][0]] = (sizeof(m2[0])?m2[0][0]:0);
        }
        args += ({ map });
        break;

      case '|' :
        if (sscanf(str, "|%s|%s", s1, s2) != 2) 
        {
          write("No se encontró la pareja de | (Unmatched)\n");
          return ({ });
        }

        obs = wiz_present(str, this_player());

        if (!sizeof(obs))
          args += ({ this_player() });
        else if (sizeof(obs) == 1)
          args += ({ obs[0] });
        else
          args += ({ obs });

        str = s2;
        break;

      case '0'..'9' :
      case '-' :
        if (sscanf(str, "%d%s", i, str) != 2) 
        {
          write("Se esperaba un número (Number expected).\n");
          return ({ });
        }

        args += ({ i });
        break;

      default :
        s2 = s3 = "";
        sscanf(str, "%s,%s", s4, s2);
        sscanf(str, "%s"+close+"%s", s5, s3);

        if (sscanf(str, "%s->%s", s6, s7) == 2 &&
           (!strlen(s3) || strlen(s5) > strlen(s6)) &&
           (!strlen(s2) || strlen(s4) > strlen(s6))) 
        {
          // Now we do something trully revolting.... 
          while (s7[0] == ' ') 
            s7 = s7[1..];
          if (sscanf(s7, "%s(%s", s1, s7) != 2) 
          {
            write("Falta un '(' (expected).\nResto de la línea sin procesar '"+s7+"'.\n");
            return ({ });
          }
          
          obs = wiz_present(s6, this_player());
          
          if (!sizeof(obs)) 
          {
            write("El objeto "+s6+" debe existir.\n");
            return ({ });
          }

          m = parse_args(s7, ")");
          if (!m) 
            return ({ });
          if (sizeof(m[0]) < 6) 
            m[0] += allocate(6-sizeof(m[0]));

          obs = map_array(obs, "mapped_call", this_object(), ({ s1 })+m[0]);

          if (sizeof(obs) == 1)
            args += obs;
          else
            args += ({ obs });

          str = m[1];
          break;
        } 
        else if (s2 && s3)
        {
          if (strlen(s4) < strlen(s5)) 
          {
            s1 = ",";
            str = s4;
          } 
          else 
          {
            s1 = close;
            s2 = s3;
            str = s5;
          }
        } 
        else if (s2) 
        {
          s1 = ",";
          str = s4;
        } 
        else if (s3) 
        {
          s1 = close;
          s2 = s3;
          str = s5;
        } 
        else 
        {
          s1 = "";
          s2 = "";
        }
        
        obs = wiz_present(str, this_player());

        if (!sizeof(obs)) 
        {
          if (str[0] >= '0' && str[0] <= '9' || str[0] == '-') 
          {
            sscanf(str, "%d%s", i, str);
            args += ({ i });
          } 
          else
            args += ({ replace(str, "\\n", "\n") });
        } 
        else if (sizeof(obs) == 1)
          args += ({ obs[0] });
        else
          args += ({ obs });

        str = s1+s2;
        break;
    }

    // Skip rubbish and if we dont have a comma we have finished.
    while (strlen(str) && str[0] == ' ') 
      str = str[1..];
    
    if (!strlen(str))
      return ({ args, str });
    if (str[0..0] == close)
      return ({ args, str[1..] });
    
    if (str[0] != ',') 
    {
      write("Parse error leyendo argumentos, falta un ',' o '"+close+"' (expected).\n");
      write("Resto de la línea no procesada '"+str+"'.\n");
      return ({ });
    }

    str = str[1..];
  }
  return ({ args, str });
} /* parse_args() */

object * wzpresent2(string str, mixed onobj) 
{
  int i;
  object *obs, obj, *obs2;
  string s1, s2;

  if (pointerp(onobj)) 
  {
    obs = ({ });
    for(i = 0; i < sizeof(onobj); i++)
      obs += wzpresent2(str, onobj[i]);
    return obs;
  }

  // if ((str == "all") || (str == "todo"))
  //   return all_inventory(onobj);

  /* every fish */

  if ((sscanf(str, "cada %s", s1) == 1) || 
      (sscanf(str, "each %s", s1) == 1) )
  {
    obs2 = all_inventory(onobj);
    obs = ({ });
    for (i = 0; i < sizeof(obs2); i++)
      if (obs2[i]->id(s1)) 
        obs += ({ obs2[i] });

    return obs;
  }

  obs2 = find_match(str, onobj);
  
  if (sizeof(obs2)) 
    return obs2;

  if (obj = present(str, onobj)) 
    return ({ obj });

  for (obj = first_inventory(onobj); obj; obj = next_inventory(obj)) 
  {
    s2 = file_name(obj);
    if (sscanf(s2, "%s"+str+"#%d", s1, i)
    || sscanf(s2, "%s#"+str, s1)) {
      return ({ obj });
    }
  }
  return ({ });
} /* wiz_present2() */

object * wiz_present(string str, object onobj, varargs int nogoout) 
{
  // nogoout is so that it WON'T check the environment of onobj
  int i,j;
  object ob, *obs, *obs2;
  string s1, s2, *sts;

  if (!strlen(str) || !onobj) 
    return ({ });

  // all the simple ones first 
  if (str[0] == '@') 
  {
    if (ob = find_living((string)this_player()->expand_nickname(extract(str, 1))))
      return ({ ob });
    
    notify_fail("Imposible encontrar el objeto living: "+extract(str,1)+".\n");
    return ({ });
  }

  if ((str == "me") || (str == "yo")) 
    return ({ this_player() });

  if ((str == "here") || (str == "aqui") || (str == "aquí")) 
    return ({ environment(this_player()) });

  if ((str == "everyone") || (str == "todos"))  
    return users();

  if (str[0] == '/') 
  {
    if (ob = find_object(str)) 
      return ({ ob });

    if (sizeof((sts = (string *)get_cfiles(str))))  
    {
      obs = ({ });
      for (i=0;i<sizeof(obs);i++)
        if ((ob = find_object(sts[i])))
          obs += ({ ob });
      return obs;
    }

    notify_fail("Objeto no estaba cargado en memoria: "+str+".\n");
    return ({ });
  }

  // (fish) == environment of fish

  if (str[0] == '(' && str[strlen(str) - 1] == ')') 
  {
    obs = wiz_present(extract(str,1,strlen(str) - 2),onobj);
  
    if (!sizeof(obs)) 
      return obs;

    for (i = 0; i < sizeof(obs); i++) 
      obs[i] = environment(obs[i]);
    return obs;
  }

  /* fish except fish2 */

  if ((sscanf(str,"%s except %s",s1,s2) == 2) ||
      (sscanf(str,"%s but %s",s1,s2) == 2) ||
      (sscanf(str,"%s excepto %s",s1,s2) == 2)) 
  {
    obs = wiz_present(s1, onobj);
    obs2= wiz_present(s2, onobj);

    for (i=0;i<sizeof(obs2);i++)
    {
      while ((j = member_array(obs2[i], obs)) > -1)
      {
        obs = delete(obs, j--, 1);
      }
    }

    return obs;
  }

  /* handle "fish on fish2" */
  /* Hmm....sounds fishy to me.  Timion 97 */

  if ((sscanf(str,"%s on %s",s1,s2) == 2) ||
      (sscanf(str,"%s in %s",s1,s2) == 2) ||
      (sscanf(str,"%s en %s",s1,s2) == 2))  
  {
    obs = wiz_present(s2, onobj);

    if (!sizeof(obs)) 
      return obs;

    obs2 = ({ });

    for (i = 0; i < sizeof(obs); i++)
      obs2 += wiz_present(s1, obs[i], 1);

    return obs2;
  }

  /* fish and fish2 */

  if ((sscanf(str,"%s and %s",s1,s2) == 2) ||
      (sscanf(str,"%s y %s",s1,s2) == 2)) 
  {
    obs  = wiz_present(s1, onobj);
    obs2 = wiz_present(s2, onobj);

    for (i = 0; i < sizeof(obs); i++)  /* remove duplicates ... */
      if (member_array(obs[i], obs2) < 0) 
        obs2 += ({ obs[i] });

    return obs2;
  }

  if ((sscanf(str, "player %s", s1)) || 
      (sscanf(str, "jugador %s", s1)))
    return ({ find_player(s1) });

  if (!sizeof(obs2 = wzpresent2(str, onobj)) && 
      !nogoout && 
      environment(onobj))
  { 
    obs2 = wzpresent2(str, environment(onobj));
  }

  if (sizeof(obs2)) 
    return obs2;

  /* check for a find_match locally */

  obs2 = find_match(str,onobj);
  if (sizeof(obs2) > 0) 
    return obs2;

  /* default to return find_living ...  */

  if (ob = find_living((string)this_player()->expand_nickname(str)))
    return ({ ob });

  return ({ });
} /* wiz_present() */

