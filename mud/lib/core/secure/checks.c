
#include <trace.h>

#include <files/log.h>
#include <mud/secure.h>
#include <user/user.h>
#include <user/player.h>

nomask int valid_progname(string progname)
{
  mixed ** trace;
  int i;

  trace = call_trace();

  for (i = sizeof(trace) - 1; i >= 0; i--)
    if (trace[i][TRACE_PROGNAME] == progname)
      return 1;

  return 0;
}

/*
 * Function name:   valid_exec
 * Description:     Checks if a certain 'program' has the right to use exec()
 * Arguments:       name: Name of the 'program' that attempts to use exec()
 *                        Note that this is different from file_name(),
 *                        Programname is what 'function_exists' returns.
 *                  NOTE, the absence of a leading slash in the name.
 * Returns:         True if exec() is allowed.
 */
/*
nomask int valid_exec(string name)
{
  if (name == "/lib/core/login.c")
    return 1;

  return -1;
}
*/

// nomask int valid_load(string path, mixed euid, string func) { return 1; }

nomask int valid_hide(object ob)
{
  return SECURE->is_administrator(geteuid(ob));
}

nomask int valid_ident(string euid)
{
  return SECURE->is_administrator(euid);
}

nomask int valid_link(string from, string to)
{
  return -1;
}

nomask int valid_override(string file, string func)
{
  string *bing;

  bing = explode(file, "/") - ({ "", "." });

  switch (bing[0])
  {
    case "secure" :
      return 1;
    case "std" :
    case "obj" :
    case "simul_efun" :
    case "global" :
    case "table" :
      return (func != "snoop");
    default :
      return -1;
  }
}

// Read permisions for all of the objects in the game.
// This IS fun.
// But seems to be buggy, fix nov '95, Baldrick.

nomask int valid_read(string path, mixed euid, varargs string func)
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

  if (SECURE->is_administrator(euid))
    return 1;

  bing = explode(path, "/");

  if (bing)
    bing = bing - ({ "", ".", "*" });
  else
    return 1;

  if (!sizeof(bing))
    return 1;

  if (euid == TMP_EUID)
    return bing[0] == "tmp";

  // added check:
  // any "old" directory will be used as a "recycle bin" of old code,
  // only readable by admins. neverbot 02/2006

  for (i = 0; i < sizeof(bing); i++)
  {
    if (bing[i] == "old")
    {
      return (SECURE->check_permission(euid, bing, READ_MASK));
    }
  }

  if (SECURE->check_permission(euid, bing, READ_MASK))
    return 1;

  switch (bing[0])
  {
    case "log":
      // logs for shops, vaults, etc
      if ((sizeof(bing) >= 2) && (bing[1] == "common"))
        return 1;

      return (SECURE->check_permission(euid, bing, READ_MASK));

    // case "secure":
    //   return (euid == SECURE_EUID) || SECURE->check_permission(euid, bing, READ_MASK);

    case "game":
      if (sizeof(bing) < 3)
        return 1;

      if ((sizeof(bing) >= 3) && (bing[2] == euid))
        return 1;

      master = "/" + bing[0] + "/" + bing[1] + "/" + bing[2] + "/master.c";

      // Bits down to master->valid_read fixed by Wonderflug, nov 95
      if ( SECURE->get_checked_master()[master] )
        return 0;

      if ( !file_exists( master ) )
      {
        /* This is the case where no master.c exists */
        SECURE->get_checked_master()[master] = 1;
        return 0;
      }

      if (!find_object(master) && !SECURE->get_checked_master()[master]
          && catch(master->dead_frogs()))
      {
        /* this is the case the master.c exists but does not load */
        SECURE->get_checked_master()[master] = 1;
        return 0;
      }

      return (int) master->valid_read(bing, euid, func);

    case "home":
      if (sizeof(bing) < 2)
        return 1;

      if ((sizeof(bing) >= 2) && (bing[1] == euid))
        return 1;

      master = "/" + bing[0] + "/" + bing[1] + "/master.c";

      // Bits down to master->valid_read fixed by Wonderflug, nov 95
      if ( SECURE->get_checked_master()[master] )
        return 0;

      if ( !file_exists( master ) )
      {
        /* This is the case where no master.c exists */
        SECURE->get_checked_master()[master] = 1;
        return 0;
      }

      if (!find_object(master) && !SECURE->get_checked_master()[master]
          && catch(master->dead_frogs()))
      {
        /* this is the case the master.c exists but does not load */
        SECURE->get_checked_master()[master] = 1;
        return 0;
      }

      return (int) master->valid_read(bing, euid, func);

    case "save":
      if (sizeof(bing) >= 2 && bing[1] == "players")
        return (SECURE->check_permission(euid, bing, READ_MASK));
      if (sizeof(bing) >= 2 && bing[1] == "accounts")
        return (SECURE->check_permission(euid, bing, READ_MASK));

      if (sizeof(bing) >= 2 && bing[1] == "post")
        return (euid == MAIL_EUID);

      return (euid == ROOM_EUID);

    case "cmds":
      if (sizeof(bing) >= 2)
      {
        object handler;
        handler = load_object("/cmds/handlers/cmd_handler.c");

        if (!handler)
          return 0;

        // if we do not have an effective uid we need it so the calls
        // to the command handler will work. neverbot
        if (!euid)
        {
          if (this_player())
            euid = geteuid(this_player());
          else
            euid = geteuid(initial_object());
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

// this stuff added by asmodean, stolen from sojan, who stole it from chrisy
nomask int valid_save_binary(string file)
{
  string *path;

  if (!file || file=="")
    return 0;

  // if (file_name(previous_object()) == "/global/virtual/compiler")
  //   return 0;

  path = explode(file, "/") - ({ "" });

  if (!sizeof(path))
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
      if (sizeof(path)==2)
        return 1;

      if (path[1] == "armours" || path[1] == "weapons")
        return 0;

      return 1;
  }
  return 0;
}

nomask int valid_seteuid(object ob, string euid)
{
  string current;
  string crea;

  current = geteuid(ob);

  // when seteuid("")
  if (!strlen(euid))
    return 1;

  if ((current == ROOT) ||
      (current == BACKBONE) ||
      (current == euid))
  {
    return 1;
  }

  if (((base_name(ob) == USER_OB) ||
       (base_name(ob) == PLAYER_OB)) &&
      (ob->query_coder()))
  {
    object player;
    player = ob->player();
    return ((euid == ob->query_name()) ||
            (euid == player->query_name()));
  }

  if (geteuid(initial_object()) == euid)
    return 1;

  // if (euid == TMP_EUID)
  //   return 1;

  crea = SECURE->creator_file(file_name(ob));

  if ((crea == ROOT) ||
      (crea == BACKBONE) ||
      (crea == euid))
  {
    return 1;
  }

  return 0;
}

/*
 * The master object is asked if it is ok to shadow object ob. Use
 * previous_object() to find out who is asking.
 */
nomask int valid_shadow(object ob)
{
  if (ob->query_prevent_shadow(previous_object()))
    return 0;

  return 1;

  // string dummy;

  // return (!ob->query_prevent_shadow(previous_object()) &&
  //         !sscanf(file_name(ob), "/secure/%s", dummy));
}

nomask int valid_socket(object ob, string func, mixed *info)
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

#define PLAYEROBS ({ "/lib/player", "/lib/core/login", "/lib/user" })

nomask int valid_write(string path, mixed euid, string func)
{
  string *bing;
  mixed master;

  if (objectp(euid))
    euid = geteuid(euid);

  if (SECURE->is_administrator(euid))
    return 1;

  bing = explode(path, "/");

  if (sizeof(bing))
    bing = bing - ({ ".", "" });
  else
    return 0;

  // Little patch to plug a security leak -- Wahooka
  if (func == "save_object" && (sizeof(bing) >= 2) &&
     (bing[0] == "save") && (bing[1] == "players"))
  {
    if (member_array(base_name(previous_object()), PLAYEROBS) != -1)
      return 1;
    return 0;
  }
  else if (func == "save_object" && (sizeof(bing) >= 2) &&
          (bing[0] == "save") && (bing[1] == "accounts"))
  {
    if (member_array(base_name(previous_object()), PLAYEROBS) != -1)
      return 1;
    return 0;
  }
  else if (func == "save_object" && bing[0] == "secure")
  {
    if (SECURE->is_administrator(euid))
      return 1;
    else
      return 0;
  }
  else if (func == "save_object")
    return 1;

  if (euid == TMP_EUID)
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

  if (SECURE->check_permission(euid, bing, WRITE_MASK))
    return 1;

  switch (bing[0])
  {
    case "log":
      if (func == "log_file")
        return 1;
      break;

    case "open":
    case "tmp":
      return 1;

    case "net":
      return euid == NETWORK_EUID;

    case "obj":
      if (euid == "ims")
        return 1;
      break;

    case "home":
      if (bing[1] == euid)
        return 1;

      master = "/" + bing[0] + "/" + bing[1] + "/master.c";

      if (find_object(master) && !SECURE->get_checked_master()[master]
            && !catch(master->dead_frogs()))
      {
        return (find_object(master) == previous_object() ||
               find_object(master)->valid_write(bing, euid, func));
      }

      SECURE->get_checked_master()[master] = 1;
      break;

    case "game":
      if (sizeof(bing) <= 2)
        return 0;

      // game/{areas,quests/permission
      if ((sizeof(bing) >= 3) &&
          (bing[2] == euid))
        return 1;

      master = "/" + bing[0] + "/" + bing[1] + "/" + bing[2] + "/master.c";

      if (find_object(master) && !SECURE->get_checked_master()[master]
            && !catch(master->dead_frogs()))
      {
        return (find_object(master) == previous_object() ||
               find_object(master)->valid_write(bing, euid, func));
      }

      SECURE->get_checked_master()[master] = 1;
      break;

    case "save":
      if ((sizeof(bing) >= 2) && (bing[1] == "players"))
        // return query_admin(euid);
        return 0;
      if ((sizeof(bing) >= 2) && (bing[1] == "accounts"))
        return 0;

      if (euid == ROOM_EUID ||
         (euid == "mailer" && sizeof(bing) == 3 && bing[1] == "post"))
        return 1;
  }

  return 0;
} /* valid_write() */

// used in login
// returned value -1 means the name is ok
// other value is the index of the wrong character inside the string
nomask int valid_user_name(string str)
{
  int i;
  for (i = 0; i < strlen(str); i++)
    if (str[i] < 'a' || str[i] > 'z')
      return i;
  return -1;
}

// used in login
int valid_email(string str)
{
  int i;
  string * pieces;

  if (!strlen(str))
    return 0;

  // first character must be a letter
  if (str[0] < 'a' || str[0] > 'z')
    return 0;

  // only letters, numbers, @ and dots
  for (i = 0; i < strlen(str); i++)
  {
    if (str[i] >= 'a' && str[i] <= 'z')
      continue;
    if (str[i] >= 'A' && str[i] <= 'Z')
      continue;
    if (str[i] >= '0' && str[i] <= '9')
      continue;

    if (str[i] == '@')
      continue;
    if (str[i] == '.')
      continue;

    return 0;
  }

  pieces = explode(str, "@");
  if (sizeof(pieces) != 2)
    return 0;

  pieces = explode(pieces[1], ".");
  if (sizeof(pieces) < 2)
    return 0;

  return 1;
}

// checked for ccmud, neverbot 07/05

nomask int valid_snoop(object snooper, object snoopee, object pobj)
{
  return -1;
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
  if (snooper->query_invis() < 2)
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
    if (!str) {
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


int valid_adjust_xp(object prev, object this, int i)
{
  if (i < 1)
    return 1;

  if (prev->query_coder() && this->query_coder())
    return 1;

  if (base_name(prev)[0..3] == "/lib")
    return 1;

  if (interactive(this)) // neverbot
  if ((!prev->query_npc() && !prev->is_player()) ||
     (immortal_create_me(prev)) )
  {
    log_file("adjust_xp", "File: " + base_name(prev) +
             "\n\tcreated by: " + prev->query_create_me() +
             " adjusting: " + this->query_cap_name() +
             "\n\tamount: " + i +
             " (this_player() = " + this_player()->query_cap_name() + ") "+ctime(time(), 4)+".\n");
    return 0;
  /*
  sprintf("%O - %s - %d - %s\n",
     prev,
     this->query_name(),
     i,
     this_player()->query_name()));

  log_file("ADJUST", "XP: create_me "+origin->query_create_me()+
     " "+base_name(origin)+" calling "+base_name(destination)+
     " for "+amt+"\n");

  */
  }

  return 1;
}
