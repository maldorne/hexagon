
void create()
{
}


object *wzpresent2(string str, mixed onobj) {
  int i;
  object *obs, obj, *obs2;
  string s1, s2;

  if (pointerp(onobj)) {
    obs = ({ });
    for(i = 0; i < sizeof(onobj); i++)
    obs += wzpresent2(str,onobj[i]);
    return obs;
  }

  // if (str == "all")
        // Añadido al find_match posterior
  // if (str == "todo")
  // return all_inventory(onobj);

  /* every fish */

  if ((sscanf(str,"cada %s",s1) == 1) || 
      (sscanf(str,"each %s",s1) == 1) ){
    obs2 = all_inventory(onobj);
    obs = ({ });
    for (i=0;i<sizeof(obs2);i++)
    if (obs2[i]->id(s1)) obs += ({ obs2[i] });
    return obs;
  }

  obs2 = find_match(str,onobj);
  if(sizeof(obs2)) {
    return obs2;
  }

  if (obj = present(str,onobj)) return ({ obj });
  for (obj = first_inventory(onobj); obj; obj = next_inventory(obj)) {
    s2 = file_name(obj);
    if (sscanf(s2, "%s"+str+"#%d", s1, i)
    || sscanf(s2, "%s#"+str, s1)) {
      return ({ obj });
    }
  }
  return ({ });
} /* wiz_present2() */

object *wiz_present(string str, varargs object onobj, int nogoout) {
  /* nogoout is so that it WON'T check the environment of onobj */
  int i,j;
  object ob, *obs, *obs2;
  string s1, s2, *sts;

  if (!str || !onobj) {
    return ({ });
  }

  /* all the simple ones first */
  if(str[0] == '@') {
    if (ob = find_living(
    (string)this_player()->expand_nickname(extract(str, 1))))
    return ({ ob });
    notify_fail("Imposible encontrar el objeto living: "+extract(str,1)+".\n");
    return ({ });
  }

  if ((str == "me") || (str == "yo")) return ({ this_player() });

  if ((str == "here") || (str == "aqui")) return ({ environment(this_player()) });

  if ((str == "everyone") || (str == "todos"))  return users();

  if(str[0] == '/') {
    if (ob = find_object(str)) return ({ ob });
    if (sizeof((sts = (string *)get_cfiles(str))))  {
      obs = ({ });
      for (i=0;i<sizeof(obs);i++)
      if ((ob = find_object(sts[i])))
      obs += ({ ob });
      return obs;
    }
    notify_fail("Objeto no estaba cargado en memoria: "+str+".\n");
    return ({ });
  }

  /* (fish) == environment of fish */

  if (str[0] == '(' && str[strlen(str) - 1] == ')') {
    obs = wiz_present(extract(str,1,strlen(str) - 2),onobj);
    if (!sizeof(obs)) return obs;
    for (i = 0; i < sizeof(obs); i++) obs[i] = environment(obs[i]);
    return obs;
  }

  if ((sscanf(str,"%s except %s",s1,s2) == 2) ||
      (sscanf(str,"%s but %s",s1,s2) == 2) ||
      (sscanf(str,"%s excepto %s",s1,s2) == 2)) {

    obs = wiz_present(s1, onobj);
    obs2= wiz_present(s2, onobj);
    for (i=0;i<sizeof(obs2);i++){
      while ((j = member_array(obs2[i], obs)) > -1){
        obs = delete(obs, j--, 1);
                  }
                }
    return obs;
  }

  /* handle "fish on fish2" */
  /* Hmm....sounds fishy to me.  Timion 97 */

  if ((sscanf(str,"%s on %s",s1,s2) == 2) ||
  (sscanf(str,"%s in %s",s1,s2) == 2) ||
  (sscanf(str,"%s en %s",s1,s2) == 2))  {
    obs = wiz_present(s2, onobj);
    if (!sizeof(obs)) return obs;
    obs2 = ({ });
    for (i = 0; i < sizeof(obs); i++)
    obs2 += wiz_present(s1,obs[i],1);
    return obs2;
  }

  /* fish and fish2 */

  if ((sscanf(str,"%s and %s",s1,s2) == 2) ||
  (sscanf(str,"%s y %s",s1,s2) == 2)) {
    obs = wiz_present(s1, onobj);
    obs2= wiz_present(s2, onobj);
    for (i=0;i<sizeof(obs);i++)  /* remove duplicates ... */
    if (member_array(obs[i], obs2) < 0) obs2 += ({ obs[i] });
    return obs2;
  }

  /* fish except fish2 */

  if ((sscanf(str,"player %s",s1)) || (sscanf(str,"jugador %s",s1)))
  return ({ find_player(s1) });

  if (!sizeof(obs2 = wzpresent2(str,onobj)) && !nogoout) {
    obs2 = wzpresent2(str, environment(onobj));
  }

  if (sizeof(obs2)) return obs2;

  /* check for a find_match locally */

  obs2 = find_match(str,onobj);
  if (sizeof(obs2) > 0) return obs2;

  /* default to return find_living ...  */

  if (ob = find_living((string)this_player()->expand_nickname(str)))
  return ({ ob });

  return ({ });
} /* wiz_present() */

mixed stats() 
{
  return ({ });
}