
#pragma save_types

#include <areas/common.h>
#include <areas/weather.h>
#include <mud/cmd.h>

inherit "/lib/core/object";

/*
 * Traduccion para CcMud, neverbot 02/2006
 *
 * ok format...
 * name ({ your st, everyone st })
 * or (one arg version ie sigh)
 * name ({ your st, their st, everyone st })
 * or (two only arg version.)
 * name ({ your st, everyone st, arg st, their arg, everyone arg })
 * or (one or two arg version)
 * name ({ ({ as above }), ({ as above }) })
 * for cases where the male and female versions are different.
 * works on targets gender. should really be the two arg only version.
 * hmmm going to need some actual processing.
 *
 * 0 == search for living name
 * 1 == search for living name and change thingy
 * 2 == search for change thingy and living name
 * 3 == search for change thingy
 */

mapping soul_data;
string lastarg, calc_long, global_adj;

/* Bishop adding get_messages as a function, so we can use it elsewhere */
mapping get_messages(object me, string verb, string str, mixed valid_targets);
mapping query_soul_data() { return ([ ]) + soul_data; }

void create()
{
  ::create();
  set_name("emociones");
  set_short("Controlador de las emociones");
  set_long("Éste es el controlador extraordinario de las emociones.\n");
  add_alias("controlador");
  add_plural("Controladores");
  add_plural("controladores");

  calc_long = "";
  lastarg = "";

  /* Taniwha 1995, move it here, players can't pick it up */
  catch(load_object(HOME));
  call_out("move", 2, HOME);

  global_adj = SOUL_DATA->query_global_adj();
  soul_data = SOUL_DATA->query_soul_data();
}

// string query_long()
string query_soul_list()
{
  int i;
  string s, s1, s2, bit;
  string *cmds;
  mixed  *values;

  s = "Éstos son los comandos de emociones disponibles:\n"+
    "   # significa que admite argumentos.\n"+
    "   * significa que sólo puede utilizarse con seres vivos.\n"+
    "   + significa que puede usarse tanto con seres vivos como de modo independiente.\n"+
    "Número total de emociones: "+m_sizeof(soul_data)+".\n\n";

  if (strlen(calc_long))
  {
    s += sprintf("  %#-*s\n", this_user()->query_cols(), calc_long);
    // s += "\nUtiliza el comando \"emocion\" para informar de posibles errores.\n";
    return s;
  }

  cmds   = m_indices(soul_data);
  values = m_values(soul_data);
  calc_long = "";
  cmds = sort_array(cmds);

  for (i = 0; i < sizeof(cmds);i ++)
  {
    // bit = cmds[i];
    values = soul_data[cmds[i]];
    bit = " ";
    if (sscanf(values[1][0],"%s$arg:%s",s1,s2) == 2)
      bit += "#";
    else
      bit += " ";
    if (sizeof(values[1]) == 3)
      bit += "*";
    else if (sizeof(values[1]) == 5)
      bit += "+";
    else
      bit += " ";
    calc_long += bit + " " +cmds[i]+ "\n";
  }


  s += sprintf("  %#-*s\n", this_user()->query_cols(), calc_long);
  // s += "\nUtiliza el comando \"emocion\" para informar de posibles errores.\n";

  return s;
}

int soul_sort(string s1, string s2) {
  return s1 > s2;
}

/*
string long(string str, int dark){
  return query_long();
}
*/

string parse_string(string s, object me, mixed ob, string arg, int uhn){

  string s1, s2, s3, s4, str, s5;
  int i;

  str = s;
  s4 = "";
  while (sscanf(str,"%s$%s$%s",s1,s2,s3) == 3)
    switch (s2)
  {
  case "mcname" :
    str = s1+me->query_cap_name()+s3;
    break;
  case "mname" :
    str = s1+me->query_name()+s3;
    break;
  case "mpronoun" :
    str = s1+me->query_pronoun()+s3;
    break;
  case "hpronoun" :
    str = s1+ob->query_pronoun()+s3;
    break;
  case "mobj" :
    str = s1+me->query_objective()+s3;
    break;
  case "hobj" :
    str = s1+ob->query_objective()+s3;
    break;
  case "mposs" :
    str = s1+me->query_possessive()+s3;
    break;
  case "hposs" :
    str = s1+ob->query_possessive()+s3;
    break;
  case "mvocal" :
    str = s1+me->query_vowel()+s3;
    break;
  case "hvocal" :
    str = s1+ob->query_vowel()+s3;
    break;
  case "mnumeral" :
    str = s1+me->query_numeral()+s3;
    break;
  case "hnumeral" :
    str = s1+ob->query_numeral()+s3;
    break;
  case "lastarg" :
    str = s1+lastarg+s3;
    break;
  case "mhcname" :
    if (uhn)
    {
      // str = s1+ob->query_cap_name()+"'s"+s3;
      str = s1+ob->query_cap_name()+s3;
      break;
    }
  case "hcname" :
    if (uhn)
    {
      str = s1+ob->query_cap_name()+s3;
      break;
    }
  case "hname" :
    if (uhn)
    {
      str = s1+ob->query_name()+s3;
      break;
    }
  default :
    s4 += s1+"$"+s2;
    if (!s3)
      str = "$";
    else
      str = "$"+s3;
    break;
  }

  str = s4+str;

  while (sscanf(str,"%s$force#%s#%d$%s",s1,s2,i,s3)==4)
  {
    call_out("do_force", i, ({s2, ob}));
    str = s1+s3;
  }

  while (sscanf(str,"%s$arg:%s$%s",s1,s2,s3)==3)
    if (arg == "?")
    {
      string *yellow;

      yellow = explode(s2, ",");
      lastarg = yellow[random(sizeof(yellow))];
      str = s1+lastarg+s3;
      lastarg = replace(lastarg, "tu", me->query_possessive());
    }
    else if (s2 == "#")
    {
      lastarg = replace(arg, "tus", me->query_possessive());
      str = s1+arg+s3;
    }
    else if (sscanf(","+s2+global_adj+",","%s,"+arg+"%s,%s",s2,s4,s5)==3)
    {
      str = s1+arg+s4+s3;
      lastarg = replace(arg+s4, "su", me->query_possessive());
    }
    else if (sscanf(","+s2+",", "%s,#,%s", s2, s4) == 2)
    {
      str = s1+arg+s3;
      lastarg = replace(arg, "sus", me->query_possessive());
    }
    else
    {
      // Cadogan by hand of Radix...
      notify_fail("No puedes hacer eso. Escribe \"ayuda <emoción>\" "+
            "para ver las opciones disponibles.\n");
      //  write("You cannot do that. Available options are "+
      //    replace(s2, ",", ", ")+"\n");
      return "";
    }

  while (sscanf(str,"%s$ifarg:%s~$%s",s1,s2,s3)==3)
  {
    string estr;

    sscanf(s2,"%s$else$%s",s2,estr);
    if (arg && arg != "")
      str = s1+parse_string(s2,me,ob,arg, 1)+s3;
    else if (estr)
      str = s1+parse_string(estr,me,ob,arg, 1)+s3;
    else
      str = s1+s3;
  }
  return str;
}

/* Bishop with valid targets, needed for channel code */
object* find_all_liv(string str, object me, varargs object *valid_targets)
{
  mixed ob;
  object *ret;
  int i;

  ret = ({ });

  str = lower_case(str);
  if (str && me->expand_nickname(str))
    str = me->expand_nickname(str);

  if (sizeof(valid_targets))
  {
    valid_targets -= ({ nil });
    for (i = 0; i < sizeof(valid_targets); i++)
    {
     if (valid_targets[i]->id(str))
        ret += ({ valid_targets[i] });
    }
    /* Bishop - forgot to return!  Soul anyone on mud via channel, bad bad bad! */
    return ret;
  }

  /* Wonder how the players will like this one, the thieves surely will..
   * removing the pssibility to soul all in a room
   * Baldrick, jan '95 (Hmm.. this years first change..:=)
   */

  // Flode changed this fix - 280898
  // if (str == "all" || str[0..0] == "0")
  // if (search_for_all(str, me))
  /* Ahora find_match filtra escondidos con el flag 1
  if (str == "all" || str[0..0] == "0")
  {
    notify_fail("Sorry, you don't know all.\n");
    return 0;
  }
  */

  /* Usuario al azar... lo eliminamos
  if (str == "alguien")
  {
    // Radix was, was a random() return of users() array
    // Wonderflug, it is again.  But we filter invisibles
    ret = users();
    for (i=0;i<sizeof(ret);i++)
      if ( ret[i]->query_invis() )
        ret = delete(ret, i--, 1);

    return ({ ret[random(sizeof(ret))] });
  }
  */

  ob = find_match(str, environment(me), 1);
  for (i = 0; i < sizeof(ob); i++)
    if ( living(ob[i]) && ob[i] != me
      && member_array(ob[i], ret) == -1)
      ret += ({ ob[i] });

  if (sizeof(ret))
    return ret;

  /* Will add an attemt to hide invis people in the room
   * Baldrick, dec '94
   * Fixed above attempt, Wonderflug oct '95
   */

  // Flode - players first, then npcs
  if ((ob = find_player(str)) && ob != me &&
     (!ob->query_invis() || me->query_coder()))
    return ({ ob });

  // Flode 060201 - distant souls on npcs had to go, hence the
  //        environment(ob) == environment(TP) check.
  if ( (ob = find_living(str)) && ob != me && environment(ob) == environment(me) &&
    ( !ob->query_invis() || me->query_coder() ))
    ret = ({ ob });

  return ret;
} /* find_all_liv() */

string get_name(object ob)
{
  return (string)ob->query_cap_name();
}

int soul_command(string verb, string str, object m)
{
  mixed target;
  mixed msg;
  mapping msgs;
  object me;
  object *told, *env;
  int i;
  mixed * keys;

  /* set up whoever is doing this soul */
  if ( m )
    me = m;
  else if ( previous_object() )
    me = previous_object();
  else
    me = this_player();

  lastarg = "";

  msgs = get_messages(me, verb, str, ({ }));

  if (!msgs["status"])
    return 0;
  if (stringp(msgs["status"]))
  {
    tell_object(me, msgs["status"]);
    return 1;
  }

  told = ({ });
  env = ({ });

  /* Who is being told? */
  // foreach(target,msg in msgs)
  // {
  //   if (objectp(target))
  //     told += ({ target });
  // }

  keys = map_indices(msgs);

  for (i = 0; i < sizeof(keys); i++)
  {
    if (objectp(keys[i]))
      told += ({ keys[i] });
  }

  /* Loop through... if it's an object, we tell them their message
   * Then we check the environment to see if that environment has been
   * told. Tell it (hidden check) and then add to told environments
   * so we don't spam.  Add past to interactives, and soul_acts(Radix)
   */
  // foreach(target,msg in msgs)
  for (i = 0; i < sizeof(keys); i++)
  {
    target = keys[i];
    msg = msgs[keys[i]];

    if (objectp(target))
    {
      /* Flode 110599 - Why was this taken out? */
      if ((mixed)target->query_blocking() == (mixed)me->query_name())
      {
        tell_object(me, ((string)target->query_cap_name()) + " te "+
             "está bloqueando.\n");
        return 1;
      }

      target->event_soul(me, msg+"\n");

      if ((!target->query_hidden()) && (!target->query_invis()) &&
         (!target->query_hide_shadow()) &&
         (member_array(environment(target),env) == -1))
      {
        event(environment(target), "soul", msgs["default"] + "\n", told);
        env += ({ environment(target) });
      }

      if (target->query_npc())
        target->soul_act(me, me->query_verb());
      else if (target != me)
        target->add_past(msg);
    }
  }
  return 1;
}

// Flode 071200 - Filter out ending period if punctation in front.
private mapping real_get(object me, string verb, string str, mixed valid_targets);

/* Bishop, making useable for other things - god this is a mess
 * Returns will be a mapping of the form
 * "status" :
     0 - utter failure (no soul of that verb)
     string - failure message (bad data types to that verb)
     1 - success (sizeof returns guaranteed to be 4)
 * object  - object's message
 * "default" - everyone's message (not the doer, not the targets)
 */
mapping get_messages(object me, string verb, string str, mixed valid_targets)
{
  mapping ret;

  ret = real_get(me, verb, str, valid_targets);

  if (ret["status"])
  {
    mixed * k;
    mixed key_t;
    string v;
    int i, character;

    k = keys(ret);

    // foreach(key_t in k)
    for (i = 0; i < sizeof(k); i++)
    {
      key_t = k[i];

      if (stringp(ret[key_t]) && strlen(v = (string)ret[key_t]) >= 3)
        if (v[strlen(v)-1] == '\n')
        {
          character = v[strlen(v)-3];
          if ((character == ',') || (character == '.') || (character == '?') || (character == '!'))
          // if (member_array(v[strlen(v)-3], ",.?!") != -1)
            ret[key_t] = character + "\n";
        }
        else
        {
          character = v[strlen(v)-2];
          if ((character == ',') || (character == '.') || (character == '?') || (character == '!'))
          // if (member_array(v[strlen(v)-2], ",.?!") != -1)
            ret[key_t] = character;
        }
    }
  }
  return ret;
}

private mapping real_get(object me, string verb, string str, mixed valid_targets)
{
  mixed *data;
  mixed liv,ob,tmp_ob,ret;
  string other,s1,s2,livfail;
  int i,j,lvl;

  ret = ([ ]);
  if ( !strlen(verb) )
    return (["status":0]);

  data = soul_data[verb];

  if (!data)
    return (["status":0]);

  /* this used?
  if (str && sizeof(data[1]) == 2 && data[1][0] != '#')
  return (["status": "No matter how hard you try, you fail to manage "+
     "this.\n" ]);
   */
  /* this if/else/for/switch parses the string passed by the player into
   * object bits and string bits, according to how the specific soul
   * expects the string.  each pair of elements in data[0] represents
   * a different way of parsing things
   */

  if (!data[0] || !str || !strlen(str))
  {
    liv = 0;
    other = "";
  }
  else
  for (j = 0; j < sizeof(data[0]); j += 2)
    switch (data[0][j])
    {
    case 0 :
      if (lvl > 1)
        break;
      if (sscanf(str, data[0][j+1], s1)==1
        && (sizeof((tmp_ob = find_all_liv(s1, me, valid_targets)))))
      {
        lvl = 1;
        other = "";
        liv = s1;
        ob = tmp_ob;
      }
      else
        livfail = s1;
      break;

    case 1 :
      if (lvl > 2)
        break;
      if (sscanf(str, data[0][j+1], s1, s2)==2
        && (sizeof((tmp_ob = find_all_liv(s2, me, valid_targets)))))
      {
        lvl = 2;
        liv = s2;
        other = s1;
        if (data[0][j+1] != "%s %s")
          lvl++;
        ob = tmp_ob;
      }
      else
        livfail = s2;
      break;

    case 2 :
      if (lvl > 2)
        break;
      if (sscanf(str, data[0][j+1], s1, s2) ==2
        && (sizeof((tmp_ob = find_all_liv(s1, me, valid_targets)))))
      {
        lvl = 2;
        liv = s1;
        other = s2;
        if (data[0][j+1] != "%s %s")
          lvl++;
        ob = tmp_ob;
      }
      else
        livfail = s2;
      break;

    case 3 :
      if (lvl > 0)
        break;
      if (sscanf(str, data[0][j+1], s1) == 1)
      {
        other = s1;
        liv = 0;
        ob = ({ });
      }
      break;
  }

  if (liv)
  {
    /* The soul has been directed at someone.  The string of someone's
     * is now in liv; ob may be an array of objects corresponding to them.
     * Extra stuff they typed is in other.
    */
    string mine, his, every;

    str = other;
    if (!sizeof(ob))
    {
      return (["status":"Lo siento, pero '"+liv+"' no está conectado.\n"]);
    }

    /* Get the strings that will be parsed. */
    if (sizeof(data[1])==3)
    {
      mine = data[1][0];
      his = data[1][1];
      every = data[1][2];
    }
    else if (sizeof(data[1])==5)
    {
      mine = data[1][2];
      his = data[1][3];
      every = data[1][4];
    }
    else
    {
      return (["status":"No puedes utilizar ese comando de emoción "+
            "de esa forma.\n"]);
    }

    if (sizeof(ob) == 1)
    {
      /* Signal success */
      ret["status"] = 1;

      /* To me */
      ret[me] = parse_string(mine, me, ob[0], str, 1);

      /* If we can't parse, error is generated in parse
       * and we just return an error */
      if (!strlen(ret[me]))
        return (["status":0]);
      ret[me]+=".";

      /* To our one target */
      /* Bishop - we don't want to parse it if a channel (valid targets set)
       * because we don't care!  Only show 3rd party, and this can create
       * lil forces that someone stuck in the string parse routine (sigh)
       */
      if (!sizeof(valid_targets))
        ret[ob[0]] = parse_string(his, me, ob[0], str, 1)+".";

      /* To everyone who can hear me */
      ret["default"] = parse_string(every, me, ob[0], str, 1)+".";
    }
    else
    {
      ret["status"] = 1;
      s2 = parse_string(mine, me, ob, str, 0);

      if (!s2)
        return (["status":0]);

      tmp_ob = map_array(ob, "get_name", this_object());
      s1 = implode(tmp_ob[1..1000], ", ")+" y "+tmp_ob[0];
      s1 = replace(s2, ({ "$hcname$", s1, "$mhcname$", s1 }));
      ret[me] = s1+".\n";

      s2 = parse_string(every, me, ob, str, 0);
      s1 = replace(s2, ({ "$hcname$", s1, "$mhcname$", s1 }));
      ret["default"] = s1+".\n";
      /* Hm. what to do? Need a global return, can't get it.
       * Must do parsing after, but how with only a string?
       */
      for (i = 0; i < sizeof(ob); i++)
      {
        ret[ob[i]] = replace(s2,
           ({
            "$hcname$",
            implode(tmp_ob - ({ get_name(ob[i])}),", ")+" y tu",
            "$mhcname$",
            implode(tmp_ob - ({ get_name(ob[i])}),", ")+" y tus",
           }))+ ".\n";
      }
    }
    return ret;
  } /* if (liv) */
  else if (sizeof(data[1])==3)
  {
    if (!livfail)
      return (["status":"No tienes el valor de hacer eso.\n"]);
    else
      return (["status":"No puedo encontrar a '"+livfail+"'.\n"]);
  }
  else
  {
    /* No target, just hit myself and everyone */
    ret["status"] = 1;
    if (!strlen(other))
      other = "";
    ret[me] = parse_string(data[1][0], me, ob, other, 0);
    if (!strlen(ret[me]))
      return (["status": 0 ]);
    ret[me]+=".";
    ret["default"] = parse_string(data[1][1], me, me, other, 0)+".";
    return ret;
  }
}

void do_force(mixed* str){

  int i;

  if (pointerp(str[1]))
    for (i=0;i<sizeof(str[1]);i++)
      str[1][i]->soul_com_force(str[0]);
  else
    str[1]->soul_com_force(str[0]);
}

void add_soul_command(string name, mixed format, mixed thingo)
{
  if (soul_data[name])
    return;

  if (format && !pointerp(format))
    return ;
  if (!stringp(name) || !pointerp(thingo)
    || member_array( sizeof(thingo), ({ 2, 3, 5 }) ) == -1 )
    return ;
  soul_data[name] = ({ format, thingo });
}

string query_soul_command(string name)
{
  return (soul_data[name]);
}

int query_soul_command_exist(string name)
{
  if (soul_data[name])
  return 1;
  else
  return 0;
}

void delete_soul_command(string name)
{
  soul_data = m_delete(soul_data, name);
}

string help_soul(string str)
{
  int j, off;
  string s1, s2, s3, ret, ret_aux, *bit;
  mixed *data;

  data = soul_data[str];
  if (!data)
    return "";

  ret = "";
  ret_aux = "";

  if (!pointerp(data[0]))
    return "El comando de emoción '"+str+"' no tiene parámetros opcionales.\n";

  for (j = 0; j < sizeof(data[0]);j += 2)
  {
    bit = explode(" " + data[0][j+1]+" ","%s");
    switch (data[0][j])
    {
    case 0:
      ret += str+bit[0]+"<persona>"+bit[1];
      break;
    case 2:
      ret += str+bit[0]+"<persona>"+bit[1]+"<argumento>"+bit[2];
      break;
    case 1:
      ret += str+bit[0]+"<argumento>"+bit[1]+"<persona>"+bit[2];
      break;
    case 3:
      ret += str+bit[0]+"<argumento>"+bit[1];
      break;
    }
    ret += "\n";
  }

  ret = "Sintaxis del comando de emoción %^BOLD%^'"+str+"'%^RESET%^:\n"+
  sprintf("  %-#*s\n\n", this_user()->query_cols(), ret);

  if (sizeof(data[1]) == 2 || sizeof(data[1]) == 5)
  {
    ret += "Sin aplicarlo a seres vivos:\n";
    bit = ({ });
    if (sscanf(data[1][0], "%s$arg:%s$%s", s1,s2,s3) == 3)
      bit = explode(s2,",");
    if (s2 == "#")
      ret_aux = "Puede utilizarse cualquier texto como argumento.\n";
    else if (sscanf(data[1][0], "%s$ifarg:%s~$%s", s1, s2, s3) == 3)
      ret_aux += "Puede utilizarse sin argumentos.\n"+
        "Los argumentos posibles son: "+implode(bit,", ")+".\n";
    else if (!sizeof(bit))
      ret_aux += "No tiene argumentos.\n";
    else
      ret_aux += "Los argumentos posibles son: "+implode(bit,", ")+".\n";

    if (this_player())
      ret_aux = sprintf("  %-=*s\n",(int)this_user()->query_cols()-2, ret_aux);

    ret += ret_aux;
    off = 2;
  }

  ret_aux = "";

  if (sizeof(data[1]) == 3 || sizeof(data[1]) == 5)
  {
    ret += "Aplicándolo a seres vivos:\n";
    bit = ({ });
    if (sscanf(data[1][off+0], "%s$arg:%s$%s", s1,s2,s3) == 3)
      bit = explode(s2,",");
    if (s2 == "#")
      ret_aux = "Puede utilizarse cualquier texto como argumento.\n";
    else if (sscanf(data[1][off+0], "%s$ifarg:%s~$%s", s1, s2, s3) == 3)
      ret_aux += "Puede utilizarse sin argumentos.\n"+
        "Los argumentos posibles son: "+implode(bit,", ")+".\n";
    else if (!sizeof(bit))
      ret_aux += "No tiene argumentos.\n";
    else
      ret_aux += "Los argumentos posibles son: "+implode(bit,", ")+".\n";
    if (this_player())
      ret_aux = sprintf("  %-=*s\n",(int)this_user()->query_cols()-2, ret_aux);

    ret += ret_aux;
    off = 2;
  }

  return ret;
}
