
string fix_string(string ret)
{
  return ret;

  /*
  
  string *st;
  int i;

  // if (!term_name) term_name = "dumb";
  if (!term_name) term_name = "ansi";

  if (!colour_map)
  colour_map = (mapping)TERM_HANDLER->set_term_type(term_name);

  if (!stringp(ret) || ret == "")
  return ret;

  // Ya lo hacemos al final
  // ret += "%^RESET%^";

  // Nuevas terminales para CcMud, Folken 7/03
  if (term_name == "web"){
    ret = (string)TERM_HANDLER->fix_web_mode(ret);
  }

  if (term_name == "ansi-enye"){
    ret = (string)TERM_HANDLER->fix_enye_mode(ret);
  }

  st = explode(ret, "%^");
  ret = "";
  for (i=0;i<sizeof(st);i++) {

    // if(st[i][0..8] == "OCTARINE:")
    // if(query_see_magic()) st[i] = extract(st[i], 9);
    // else continue;

    if (colour_map[st[i]])
    ret += colour_map[st[i]];
    else
    ret += st[i];
  }
  return ret+colour_map["RESET"];

  */
}

