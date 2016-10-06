
// char to string
string chr(int c)
{
  string s;

  s = " ";
  s[0] = c;

  return s;
}

// Extracted from the old /global/basic/print_object.c, neverbot
// (and heavily improved)
static string to_string(mixed ob, varargs int offset, string pre) 
{
  int i;
  string ret;

  ret = "";

  if (pre)
    ret += "                   "[0..offset - strlen(pre)] + pre;
  else
    ret += "                   "[0..offset];

  if (intp(ob)) 
  {
    ret += ob + "\n";
    return ret; // "int";
  }
  else if (stringp(ob)) 
  {
    ret += "\"" + ob + "\"\n";
    return ret; // "string";
  }
  else if (objectp(ob)) 
  {
    if (userp(ob)) 
    {
      ret += "<" + file_name(ob) + ">\n";
      return ret; // "user object";
    }    

    ret += "<" + file_name(ob) + ">\n";
    
    // if (ob->short())
    //   ret += "(short) " + ob->short() + "\n";
    // else if (ob->query_name())
    //   ret += "(name)  " + ob->query_name() + "\n";
    // else
    //   ret += "(argghhhh)\n";
    
    return ret; // "object";
  }
  else if (mappingp(ob))
  {
    if (!m_sizeof(ob))
      ret += "([ ])\n";
    else
    {
      string * keys;
      keys = m_indices(ob);
      
      ret += "([ -- size " + sizeof(keys) + "\n";

      for (i = 0; i < sizeof(keys); i++)
      {
        ret += "                       "[0..offset+2] + keys[i] + " : \n";
        ret += to_string(ob[keys[i]], offset + 2, "");
        // if (i < sizeof(keys)-1)
        //   ret += "\n";
      }

      ret += "                       "[0..offset] + "])\n";
    }
    return ret; // "mapping";
  }
  else if (arrayp(ob)) 
  {
    if (!sizeof(ob))
      ret += "({ })\n";
    else 
    {
      ret += "({ -- size " + sizeof(ob) + "\n";
      
      for (i = 0; i < sizeof(ob); i++)
      {
        ret += to_string(ob[i], offset + 5, ""+i+". ");
        // if (i < sizeof(ob)-1)
        //   ret += "\n";
      }
      
      ret += "                       "[0..offset] + "})\n";
    }
    return ret; // "array";
  }
  else if (undefinedp(ob))
  {
    ret += "nil\n";
    return ret; // "nil";
  }

  return ret; // "oh dear";
}

static string print_object(mixed ob)
{
  write(to_string(ob));

  if (intp(ob)) 
  {
    return "int";
  }
  else if (stringp(ob)) 
  {
    return "string";
  }
  else if (objectp(ob)) 
  {
    if (userp(ob)) 
      return "user object";

    return "object";
  }
  else if (mappingp(ob))
  {
    return "mapping";
  }
  else if (arrayp(ob)) 
  {
    return "array";
  }
  else if (undefinedp(ob))
  {
    return "nil";
  }

  return "oh dear";
}

private string number_as_string_aux(int n);
string number_as_string(int n);

string query_num(int n, int limit) 
{
  if (limit && n>limit) 
    return "miles de"; /* this is a little pointless ... */
  return number_as_string(n);
}

string number_as_string(int n)
{
  string ret;
  int i;

  if (n < 0) return "miles de"; /* fo wraps... */
  if (n > 99999) return "miles de"; /* sorry 'bout the hard limit */
  ret = nil;
  if ((i = n/1000)) {
    n = n%1000;
    if (i==1) ret = "mil";
    else
      ret = number_as_string_aux(i) + " mil";
    if (!n) return ret;
  }
  if ((i = n/100)) {
    n = n%100;
      if (!ret) ret="";
      else ret += " ";
      switch (i)
      {
        case 1: if (!n) ret += "cien"; else ret += "ciento"; break;
        case 2: ret += "doscientos"; break;
        case 3: ret += "trescientos"; break;
        case 4: ret += "cuatrocientos"; break;
        case 5: ret += "quinientos"; break;
        case 6: ret += "seiscientos"; break;
        case 7: ret += "setecientos"; break;
        case 8: ret += "ochocientos"; break;
        case 9: ret += "novecientos"; break;
      }  
      if (!n) return ret;
  }
  if (ret)
    return ret + " " + number_as_string_aux(n);
  return number_as_string_aux(n);
} /* query_num() */

private string number_as_string_aux(int n) 
{
  string ret;

  if (!n) return "cero";

  if (n > 99) return "muchos";
  
  if (n<30 && n>9) return ({ "diez", "once", "doce", "trece",
                             "catorce", "quince", "dieciséis", "diecisiete",
                             "dieciocho", "diecinueve", "veinte",
                             "veintiún", "veintidós", "veintitrés",
                             "veinticuatro", "veinticinco", "veintiséis",
                             "veintisiete", "veintiocho", "veintinueve" })[n-10];
  ret = ({ "", "", "", "treinta", "cuarenta", "cincuenta", "sesenta",
           "setenta", "ochenta", "noventa"})[n/10];
  if ((n = n%10)&&(ret!="")) ret += " y ";
  return ret + ({ "", "un", "dos", "tres", "cuatro", "cinco", "seis",
                  "siete", "ocho", "nueve"})[n];
} /* number_as_string() */


string query_multiple_short(mixed *obs, varargs int flag) 
{
  int i, j, k;
  string *str;
  mixed *bity;

  bity = ({ });
  for (i = 0; i < sizeof(obs); i++) 
  {
    if (!stringp(obs[i])) 
    {
      if (!pointerp(obs[i])) 
      // like a string.  First bit is the string.
      // second the object 
      {
        if(objectp(obs[i]))
        {
          if (flag)
            str = (string *)obs[i]->pretty_short();
          else
            str = (string *)obs[i]->short();
        }
      }
      else 
      {
        str = obs[i][0];
        obs[i] = obs[i][1];
      }
    } 
    else
      str = obs[i];
      
    if (!str) // ignore invis objects
      continue;
      
    if (stringp(str))
      str = ({ str });
      
    for (j=0;j<sizeof(str);j++)
      if ((k=member_array(str[j], bity)) == -1)
        bity += ({ str[j], ({ obs[i] }) });
      else
        bity[k+1] = ({ obs[i] })+bity[k+1];
  }
  str = ({ });
  for (i = 0; i < sizeof(bity); i+=2) 
  {
    j = sizeof(bity[i+1]);
    if (stringp(bity[i+1][0]))
      if (j==1)
        str += ({ bity[i] });
      else
      str += ({ (query_num(j,20)+
        " "+pluralize(bity[i])) });
    else if (j == 1)
    /* Propiedad determinate eliminada
    if (bity[i+1][0]->query_property("determinate"))
    str += ({ bity[i+1][0]->query_property("determinate")+bity[i] });
    else
    */
      // str += ({ add_a(bity[i]) });
      str += ({ bity[i] });
    else
    {
      if (flag)
        str += ({ (query_num(j,20)+
          " "+bity[i+1][0]->pretty_plural(bity[i])) });
      else
        str += ({ (query_num(j,20)+
          " "+bity[i+1][0]->query_plural(bity[i])) });
    }
  }
  
  if (!sizeof(str))
    return "";
    
  if (sizeof(str) == 1)
    return str[0];
    
  if (lower_case(str[sizeof(str)-1][0..0]) == "i")
    return implode(str[0..sizeof(str)-2],", ")+" e "+str[sizeof(str)-1];
  else
    return implode(str[0..sizeof(str)-2],", ")+" y "+str[sizeof(str)-1];
}

