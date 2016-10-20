
/* 
 * code to handle forcing.   The method used will be to ask the object to
 * do the command.  While this is similar to the original method of doing this
 * it will be used to be more in the style of asking things to do things,
 * rather than the making things do things without asking them first.
 *
 * neverbot limpiando codigo. Dejo el do_force_on_me y lo necesario, elimino las
 * variables y la posibilidad de impedir forces. Quito el add_action force tambien.
 */

#define LOG_FORCE "force" 
#define FORCE_COST 1
#define ILLEGAL_FORCE ({ "alias", "unalias", "mv", "mkdir", "call", "rm", \
                         "force", "kill", "gauge", "exec", "promote", \
                         "new_domain", "rmdir", "cd", "history", "echoall", "shout" })

void create()
{

}

/*
nosave string nfname;
nosave int no_force_me;

void force_commands() {
  sscanf(file_name(this_object()), "%s#", nfname);
  if (!this_object()->query_property("npc") &&
     this_object()->query_property("force") || nfname == "/global/lord")
    add_action("do_force", "force");
  if (nfname == "/global/lord")
    add_action("no_force", "noforce");
} */
/* force_commands() */

/*
protected int no_force(string str) {
  if (nfname!="/global/lord")
    return 0;
  if (str == "on") {
    no_force_me = 1;
    write("Ok.\n");
    return 1;
  }
  if (str == "off") {
    no_force_me = 0;
    write("Ok.\n");
    return 1;
  }
  write("Forzar en ti esta actualmente ");
  if (no_force_me) {
    write("deshabilitado.\n");
  }
  else {
    write("habilitado.\n");
  }
  return 1;
}  */
/* no_force() */

/*
int do_force(string str) {
  string who, what;
  object *obs, ob;
  int i;

  if (nfname == "/global/player" && !this_player()->query_property("force")) {
    notify_fail("Todavia no tienes la habilidad para hacer eso.\n");
    return 0;
  }
  if (!str || sscanf(str, "%s %s", who, what) != 2) {
    notify_fail("Uso : force <persona> <comando>\n");
    return 0;
  }
  if (this_player()->adjust_social_points(-FORCE_COST) < 0) {
    notify_fail("No tienes suficientes puntos sociales.\n");
    return 0;
  }
  who = (string)this_player()->expand_nickname(who);
  who = lower_case(who);
  obs = find_match(who, environment(this_object()));
  if (!sizeof(obs))
    if (!(ob = find_living(who))) {
      notify_fail("No hay nada con ese nombre.\n");
      return 0;
    } else
      obs += ({ ob });
  for (i=0;i<sizeof(obs);i++)
    if (!obs[i]->do_force_on_me(what))
      write(capitalize(who) + " no quiere hacer eso.\n");
  write("Ok.\n");
  return 1;
} */
/* do_force() */

int do_force_on_me(string str, varargs int msg) 
{
  // string temp1, temp2;
  object forcer;

  forcer = this_player(1);

  if (!forcer) 
    forcer = previous_object();
  
  log_file(LOG_FORCE, "[" + ctime(time(),4) + "] '" + forcer->query_name() + 
                      "' forces '" +
                      this_object()->query_name() + (this_object()->query_npc()?"' (npc)":"'") + 
                      " to do '" + str+"'.\n");
  
  if (!msg)
    tell_object(this_object(), forcer->query_cap_name() +
                " intenta forzarte a '" + str + "'.\n");
  /*
  if (no_force_me) 
  {
    event(users(), "inform", forcer->query_cap_name()+" fuerza a "+
                   this_object()->query_name()+" a hacer "+str+" (y falla)", "force");
    log_file("FORCE", " (falla)\n");
    return 0;
  }
  if (!sscanf(str, "%s %s", temp1, temp2)) {
    temp1 = str;
  }
  */
    
  /* Delete this at your own peril */
  /*
  if (member_array(temp1, ILLEGAL_FORCE) != -1 &&
      !"secure/master"->god(geteuid(forcer))) {
    event(users(), "inform", forcer->query_cap_name()+" fuerza a "+
                   this_object()->query_name()+" a hacer "+str+" (y falla)", "force");
    log_file("FORCE", " (falla)\n");
    return 0;
  }
  */
  
  // command(str);
  // this_object()->queue_action(str); 
  this_object()->do_command(str);

  if (forcer)
    event(users(), "inform", forcer->query_cap_name()+" forces "+
                 this_object()->query_name()+" to do '"+str+"'", "force");

  return 1;
}

mixed stats()
{
  return ({ });
}
