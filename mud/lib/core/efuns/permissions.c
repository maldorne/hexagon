
#include <log.h>

/*
 * Function name:   valid_exec
 * Description:     Checks if a certain 'program' has the right to use exec()
 * Arguments:       name: Name of the 'program' that attempts to use exec()
 *                        Note that this is different from file_name(),
 *                        Programname is what 'function_exists' returns.
 *                  NOTE, the absence of a leading slash in the name.
 * Returns:         True if exec() is allowed.
 */
int valid_exec(string name) 
{
  if (name == "/lib/core/login.c")
    return TRUE;

  return FALSE;
} 

int valid_load(string path, mixed euid, string func) { return 1; }

int valid_hide(object ob) 
{
  return load_object(SECURE)->load_object(SECURE)->is_administrator(geteuid(ob));
}

int valid_ident(string euid) 
{
  return load_object(SECURE)->is_administrator(euid);
} 

int valid_link(string from, string to) 
{
  return FALSE;
}

int valid_override(string file, string func) 
{
  string *bing;

  bing = explode(file, "/") - ({ "", "." });

  switch (bing[0]) 
  {
    case "secure" :
      return TRUE;
    case "std" :
    case "obj" :
    case "simul_efun" :
    case "global" :
    case "table" :
      return (func != "snoop");
    default :
      return FALSE;
  }
} 

/*
 * Read permisions for all of the objects in the game.
 * This IS fun.
 * But seems to be buggy, fix nov '95, Baldrick.
 */

int valid_read(string path, mixed euid, string func) 
{
  string *bing;
  mixed master;
  int i;

  if (func == "restore_object") 
    return 1;
  if (func == "file_size")
    return 1;

  if (objectp(euid)) 
    euid = geteuid(euid);

  if (load_object(SECURE)->is_administrator(euid)) 
    return 1;
  
  bing = explode(path, "/");
  
  if (bing)
    bing = bing - ({ "", "." });
  else
    return 1;

  if (!sizeof(bing)) 
  return 1;

  if (euid == "tmp")
    return bing[0] == "tmp";

  // Comprobacion añadida:
  // Cualquier directorio 'old' sera utilizado como
  // una 'papelera de reciclaje' de codigo antiguo,
  // solo legible por admins. neverbot 02/2006
  for (i = 0; i < sizeof(bing); i++)
  {
    if (bing[i] == "old")
    {
      return (load_object(SECURE)->check_permission(euid, bing, READ_MASK));
    }
  }

  if (load_object(SECURE)->check_permission(euid, bing, READ_MASK)) 
    return 1;

  switch (bing[0]) 
  {
    /*
    case "players" :
      // return (sizeof(bing) < 3) || load_object(SECURE)->check_permission(euid, bing, READ_MASK);
      return (load_object(SECURE)->check_permission(euid, bing, READ_MASK));
    */

    case "log" : 
    // Logs de tiendas, baules, etc
    if ((sizeof(bing) >= 2) && (bing[1] == "common"))
    return 1;

      return (load_object(SECURE)->check_permission(euid, bing, READ_MASK));

    case "secure" :
      return (euid == "secure") || load_object(SECURE)->check_permission(euid, bing, READ_MASK);

    case "d" :
       // /d open read for Thanes...
     // No, los thanes solo tienen acceso a su dominio
       // if("/secure/thanes"->query_of(euid)) return 1;

    case "home" :
      if (sizeof(bing) >= 2) 
    {
        if ((bing[0]=="home"?bing[1]:capitalize(bing[1])) == euid) 
      return 1;
      
        master = bing[0] + "/" + bing[1] + "/master.c";

        // Bits down to master->valid_read fixed by Wonderflug, nov 95
        if ( load_object(SECURE)->get_checked_master()[master] )
          return 1;
        if ( !file_exists( master ) )
        {
          /* This is the case where no master.c exists */
          load_object(SECURE)->get_checked_master()[master] = 1;
          return 1;
        }
        if (!find_object(master) && !load_object(SECURE)->get_checked_master()[master]
            && catch(master->dead_frogs())) 
        {
          /* this is the case the master.c exists but does not load */
          load_object(SECURE)->get_checked_master()[master] = 1;
          return 1;
        }
        return (int) master->valid_read(bing, euid, func);
      }
      return 1;

    case "save" :
      if (sizeof(bing) >= 2 && bing[1] == "players") 
        return (load_object(SECURE)->check_permission(euid, bing, READ_MASK));
      if (sizeof(bing) >= 2 && bing[1] == "accounts") 
        return (load_object(SECURE)->check_permission(euid, bing, READ_MASK));

      if (sizeof(bing) >= 2 && bing[1] == "post") 
        return (euid == "mailer");

      return (euid == "Room");

  case "cmds" :
    if (sizeof(bing) >= 2)
    {
      object handler;
      handler = load_object("/cmds/handlers/cmd_handler.c");
      
      if (!handler)
        return 0;

      if (!euid)
      // Si no tenemos uid efectiva la necesitamos para que 
      // las llamadas al handler de comandos funcionen correctamente
      {
        if (this_player())
          euid = geteuid(this_player());
        else
          euid = geteuid(previous_object());
      }
      
      if (member_array("/"+bing[0]+"/"+bing[1]+"/", 
        handler->query_available_directories_by_euid(euid)) == -1)
        return 0;
      
      return 1;
    }
    else
      return 1;
  }

  return 1;

} /* valid_read() */


/* this stuff added by asmodean, stolen from sojan, who stole it from chrisy */
int valid_save_binary(string file)
{
  string *path;
 
  if(!file || file=="")
    return 0;
 
  if(file_name(previous_object()) == "/global/virtual/compiler")
    return 0;
 
  path = explode(file, "/") - ({ "" });
 
  if(!sizeof(path))
    return 0;
 
 
  switch(path[0])
  {
  case "home":
  case "tmp":
  case "open":
  case "d":
    return 0;
 
  case "secure":
  case "std":
  case "global":
  case "net":
    return 1;
 
  case "obj":
    if(sizeof(path)==2)
      return 1;
 
    if(path[1] == "armours" || path[1] == "weapons")
      return 0;
 
    return 1;
  }
  return 0;
}
 

int valid_seteuid(object ob, string euid) 
{
  string crea;

  if (euid == "tmp") 
      return 1;
  
  crea = creator_file(file_name(ob));
  // crea = creator_file(file_name(ob), 1);

  if (crea == "Root" || crea == "Room") 
  return 1;

  return (euid == crea) || !euid;
} 


/*
 * The master object is asked if it is ok to shadow object ob. Use
 * previous_object() to find out who is asking.
 *
 * In this example, we allow shadowing as long as the victim object
 * hasn't denied it with a query_prevent_shadow() returning 1.
 */
int valid_shadow(object ob) 
{
  string dummy;

  return (!ob->query_prevent_shadow(previous_object()) &&
          !sscanf(file_name(ob), "/secure/%s", dummy));
}

int valid_socket(object ob, string func, mixed *info) 
{
  return TRUE;
}


/*
 * This controls the write permisions for everything in the mud.
 * It defaults to 0 if all else fails.
 * Any permisions can be overridden by the global permision giving/
 * changeing code.
 *
 * The master.c's in the domain and wizard dirs will be chebe checked if
 * any access is done to them.  This is a very flexible system,
 * but perhaps a little hard for some people to use.
 */
 
#define PLAYEROBS ({ "/global/player", "/global/coder", \
                     "/global/coordinator", "/global/administrator", })
 
int valid_write(string path, mixed euid, string func) 
{
  string *bing;
  mixed master;

  if (objectp(euid)) 
  euid = geteuid(euid);
  
  if (load_object(SECURE)->is_administrator(euid)) 
  return 1;
  
  bing = explode(path, "/");

  if (bing)
    bing = bing - ({ ".", "" });
  else
    return 0;

  /* Little patch to plug a security leak -- Wahooka */
  if (func == "save_object" && (sizeof(bing) >= 2) &&
      (bing[0] == "save") && (bing[1] == "players")) 
  {
    if(member_array(base_name(previous_object()), PLAYEROBS) != -1) 
      return 1;
    return 0;
  }
  else if (func == "save_object" && (sizeof(bing) >= 2) &&
      (bing[0] == "save") && (bing[1] == "accounts")) 
  {
    if (base_name(previous_object()) == "/global/account") 
      return 1;
    return 0;
  }
  else if (func == "save_object" && bing[0] == "secure") 
  {
    if(load_object(SECURE)->is_administrator(euid)) 
      return 1;
    else 
      return 0;
  }
  else if (func == "save_object") 
    return 1;
  
  if (euid == "tmp") 
  {
    if (bing[0] == "tmp") 
      return 1;
    if (sizeof(bing) > 1 && bing[1] == "dungeons") 
      return 1;
    return 0;
  }

  // if (bing[sizeof(bing)-1] == ERROR_LOG) return 1;
  if ((sizeof(bing) == 0) || (bing[sizeof(bing)-1] == ERROR_LOG)) 
    return 1;

  if (load_object(SECURE)->check_permission(euid, bing, WRITE_MASK)) 
    return 1;
  
  switch (bing[0]) 
  {
    case "log" :
      if (func == "log_file")
        return 1;
      break;
    case "open" :
    case "tmp" :
      return 1;
    case "net" :
      return euid == "Network";
    case "obj" :
      if (euid == "ims")
        return 1;
      break;
    case "home" :
    case "d" :
      if ((sizeof(bing) == 1) || 
          ((bing[0]=="home"?bing[1]:capitalize(bing[1])) == euid) ||
          load_object(SECURE)->is_administrator(euid) ) 
      {
        return 1;
      }
        
      master = bing[0] + "/" + bing[1] + "/master";
        
      if (find_object(master) && !load_object(SECURE)->get_checked_master()[master]
            && !catch(master->dead_frogs())) 
      {
        return (find_object(master) == previous_object() ||
               find_object(master)->valid_write(bing, euid, func));
      }

      load_object(SECURE)->get_checked_master()[master] = 1;
      break;

    case "save" :
      if ((sizeof(bing) >= 2) && (bing[1] == "players"))
        // return query_admin(euid);
        return 0;
      if ((sizeof(bing) >= 2) && (bing[1] == "accounts"))
        return 0;
    
      if (euid == "Room" ||
         (euid == "mailer" && sizeof(bing) == 3 && bing[1] == "post")) 
        return 1;
  }

  return 0;
} /* valid_write() */


// Revisado para CcMud, neverbot 07/05

int valid_snoop(object snooper, object snoopee, object pobj) 
{
  return FALSE;
  /*
  if (snooper == snoopee) {
    tell_object(snooper, "No puedes snoopearte a ti mismo.\n");
    return 0;
  }
  if (snoopee && query_snoop(snoopee)) {
    tell_object(snooper,
      snoopee->query_cap_name()+" ya está siendo snoopeado.\n");
    return 0;
  }
  if (snooper->query_snoopee()) 
  {
    if (!snooper->query_property("quiet snoop")) 
    {
        event(users(), "inform", snooper->query_cap_name()+" deja de snoopear a "+
          snooper->query_snoopee()->query_name(), "snoop");
        tell_object((object)snooper->query_snoopee(),
          snooper->query_cap_name()+" deja de snoopearte.\n");
    } else {
        event(users(), "inform", snooper->query_cap_name()+" deja de qsnoopear a "+
          snooper->query_snoopee()->query_name(), "snoop"); 
        snooper->remove_property("quiet snoop");
    }
    snooper->set_snoopee(0);
  }
  if (!snoopee)
    return 1;

  if (query_snoop(snoopee))
    return 0;
  if (!snooper->query_coder())
    return 0;
  if (pobj == this_object()) {
    event(users(), "inform", snooper->query_cap_name()+" comienza a qsnoopear a "+
      snoopee->query_name(), "snoop"); 
    return 1;
  }
  if (query_verb() == "qsnoop" && query_admin(geteuid(snooper)))
  {
    tell_object(snooper, "Comienzas a qsnoopear a "+
      snoopee->query_cap_name()+".\n");
    snooper->add_property("quiet snoop", 1);
  } 
  else 
  {
    tell_object(snoopee, "Estás siendo snoopeado por "+
      snooper->query_cap_name()+".\n");
    tell_object(snooper, "Comienzas a snoopear a "+
      snoopee->query_cap_name()+".\n");
  }
  // Radix...
  if(snooper->query_invis() < 2)
  {
    log_file("snoop","[" + ctime(time(),4) + "] " +
      (string)snooper->query_cap_name() + " " +
      query_verb() + "s " + (string) snoopee-> query_cap_name()+ ".\n");
          snooper->set_snoopee(snoopee);

    // El input_to se ejecuta despues de terminar esta funcion, por lo que la razon
    // se introduce cuando YA estas snoopeando... lo quito por comodidad
    // neverbot 07/05
    // tell_object(snooper, "Indica la razón por la que estás snoopeando:\n");
    // input_to("snoop_reason");
    event(users(), "inform", snooper->query_cap_name()+" comienza a snoopear a "+
        snoopee->query_name(), "snoop");
  }
  snoop_list[snooper] = snoopee;
  return 1;
  */
} /* valid_snoop() */


/*
void snoop_reason(string str) {
    object snooper;

    snooper = this_player();
    if (this_player(1) != this_player()) {
  write("No puedes forzar a alguien a hacer esto...\n");
  return ;
    }
 //     if (!high_programmer(geteuid(this_player()))) {
  // write("Not a high programmer.\n");
  // return ;
 //      }

    if (!snoop_list[snooper]) {
  write("Se ha desconectado.\n");
  return ;
    }
    if(!str) {
  write("Snoop cancelado.\n");
  log_file("snoop", "  No ha indicado razon (termina).\n");
  return;
    }
    log_file("snoop", "  Razón: " + str + "\n");
    if (snoop(snooper, snoop_list[snooper]))
  write("Snoop con éxito.\n");
    else
  write("Snoop falló.\n");
}
*/


