
#define TO_STRING_PADDING "                       "

// Extracted from the old /global/basic/print_object.c, neverbot
// (and heavily improved)
static string to_string(mixed ob, varargs int offset, string pre) 
{
  int i;
  string ret;

  ret = "";

  if (pre)
    ret += TO_STRING_PADDING[0..offset - strlen(pre)] + pre;
  else
    ret += TO_STRING_PADDING[0..offset];

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
    if (!map_sizeof(ob))
      ret += "([ ])\n";
    else
    {
      string * keys;
      keys = map_indices(ob);
      
      ret += "([ -- size " + sizeof(keys) + "\n";

      for (i = 0; i < sizeof(keys); i++)
      {
        ret += TO_STRING_PADDING[0..offset+2] + keys[i] + " : \n";
        ret += to_string(ob[keys[i]], offset + 2, "");
        // if (i < sizeof(keys)-1)
        //   ret += "\n";
      }

      ret += TO_STRING_PADDING[0..offset] + "])\n";
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
      
      ret += TO_STRING_PADDING[0..offset] + "})\n";
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
