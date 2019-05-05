
// /global/cmd/cmd_handler.c  -- Object based command handler
// Removed the #ifdefs (don't ask me why)
// Added verb Baldrick, dec '97
// Ported to dgd, neverbot aug '15

#include <mud/cmd.h>
#include <mud/secure.h>

int cmd_make_hash(int i);
int soul_com(string str, object me);

private static int save_all; // Flode
private static mapping cmd_dirs;
static mapping cmd_hash;     // For commands and their objects
static mapping cmd_aliases;  // For command aliases
static string last_dir;      // Last directory a command was found
static string current_verb;  // Used by query_verb() efun

void create()
{
  cmd_dirs = ([
      // "/lib/cmds/living/": ({LIVING_CMD, "Player"}),
       "/lib/cmds/player/":  ({PLAYER_CMD,  "Player"}),
      "/game/cmds/player/":  ({PLAYER_CMD,  "Player"}),
       "/lib/cmds/coder/":   ({CODER_CMD,   "Coder"}),
      "/game/cmds/coder/":   ({CODER_CMD,   "Coder"}),
       "/lib/cmds/manager/": ({MANAGER_CMD, "Project Manager"}),
      "/game/cmds/manager/": ({MANAGER_CMD, "Project Manager"}),
       "/lib/cmds/admin/":   ({ADMIN_CMD,   "Administrator"}),
      "/game/cmds/admin/":   ({ADMIN_CMD,   "Administrator"}),
      // "/net/cmds/":              ({0,   "InterMUD network"}),
      // "/lib/cmds/handler/cmds/": ({0,   "Command handler"}),
    ]);

  cmd_hash = ([ ]);
  cmd_aliases = ([ ]);
  last_dir = "";

  seteuid(getuid());
  current_verb = "";
  cmd_make_hash(0);
}

void dest_me()
{
  destruct(this_object());
}

int clean_up()
{
  return 0;
}

// The simul for query_verb() queries this if efun::query_verb() == 0
string query_current_verb() { return current_verb; }

// The real code :)
string find_cmd(string verb)
{
  // Try to find the command for 'verb', or return 0
  // if not found. Remember it's location..
  string s, *dirs;
  int i;

  if (cmd_hash[verb])
  {
    last_dir = cmd_hash[verb]["dir"];
    return cmd_hash[verb]["file"];
  }

  dirs = map_indices(cmd_dirs);

  for (i = 0; i < sizeof(dirs); i++)
  {
    s = dirs[i] + verb;
    if (file_size(s + ".c") > 0)
    {
      last_dir = dirs[i];
      cmd_hash[verb] = ([ "file" : s + ".c", "count" : 0 ]);
      cmd_hash[verb]["dir"] = last_dir;
      return s;
    }
  }

  return nil;
}

string * query_available_cmds(object player)
{
  int * perms;
  string * avail_cmds;
  string * dirs;
  string * aux;
  int i;

  perms = ({ });
  avail_cmds = ({ });
  dirs = ({ });
  aux = ({ });

  if (!player)
    return nil;

  // Obtenemos los permisos que el objeto tiene
  if (player->query_player())
    perms += ({ PLAYER_CMD, });
  if (player->query_coder())
    perms += ({ CODER_CMD, });
  if (player->query_manager())
    perms += ({ MANAGER_CMD, });
  if (player->query_admin())
    perms += ({ ADMIN_CMD, });

  // Obtenemos todos los posibles directorios de comandos
  aux = keys(cmd_dirs);

  // Filtramos los directorios de comandos segun los permisos de nuestro objeto
  for (i = 0; i < sizeof(aux); i++)
  {
    if (member_array(cmd_dirs[aux[i]][0], perms) != -1)
      dirs += ({ aux[i] });
  }

  // dirs ahora contiene los directorios de cmds a los que el objeto tiene acceso

  aux = keys(cmd_hash);

  for (i = 0; i < sizeof(aux); i++)
  {
    if (member_array(cmd_hash[aux[i]]["dir"], dirs) != -1)
      avail_cmds += ({ cmd_hash[aux[i]]["file"] });
  }

  return avail_cmds;
}

string * query_available_directories(object player)
{
  string * result;
  result = ({ });

  if (player->query_player())
    result += ({
                "/lib/cmds/player/",
                "/game/cmds/player/"
              });
  if (player->query_coder())
    result += ({
                "/lib/cmds/coder/",
                "/game/cmds/coder/"
              });
  if (player->query_manager())
    result += ({
                "/lib/cmds/manager/",
                "/game/cmds/manager/"
              });
  if (player->query_admin())
  {
    result += ({
                "/lib/cmds/admin/",
                "/game/cmds/admin/",
                // "/lib/cmds/handler/cmds/",
                // "/net/cmds/"
              });
  }

  return result;
}

string * query_available_directories_by_euid(string euid)
{
  object user;
  user = find_living(euid);

  if (!user)
    return ({ });
  return query_available_directories(user);
}

mapping query_available_cmds_by_category(object player)
{
  int i, j;
  string * aux;
  mixed * categories;
  mapping directories;
  mapping result;

  aux = ({ });
  categories = ({ });
  directories = ([ ]);
  result = ([ ]);

  if (!player)
    return nil;

  // Obtenemos todos los posibles tipos de comandos
  aux = keys(cmd_dirs);

  for (i = 0; i < sizeof(aux); i++)
  {
    if (member_array(cmd_dirs[aux[i]][1], categories) == -1)
      categories += ({ ({ cmd_dirs[aux[i]][1], aux[i] }) });
    // directories[aux[i]] = ({ });
  }

  if (player->query_player())
  {
    directories["/lib/cmds/player/"] = ({  });
    directories["/game/cmds/player/"] = ({  });
  }
  if (player->query_coder())
  {
    directories["/lib/cmds/coder/"] = ({  });
    directories["/game/cmds/coder/"] = ({  });
    // directories["/net/cmds/"] = ({  });
    // directories["/lib/cmds/handlers/cmds/"] = ({  });
  }
  if (player->query_manager())
  {
    directories["/lib/cmds/manager/"] = ({  });
    directories["/game/cmds/manager/"] = ({  });
  }
  if (player->query_admin())
  {
    directories["/lib/cmds/admin/"] = ({  });
    directories["/game/cmds/admin/"] = ({  });
  }

  // En categories tenemos una lista de listas, donde cada una
  // contiene el tipo de comando y el directorio de dichos comandos

  aux = keys(cmd_hash);

  for (i = 0; i < sizeof(aux); i++)
    if (!undefinedp(directories[cmd_hash[aux[i]]["dir"]]))
      directories[cmd_hash[aux[i]]["dir"]] += ({ cmd_hash[aux[i]]["file"], });

  // Tenemos en directories un mapping donde cada clave es un directorio
  // para comandos y cada entrada una lista con los paths completos
  // de todos los comandos bajo ese directorio

  aux = keys(directories);

  for (i = 0; i < sizeof(aux); i++)
  {
    for (j = 0; j < sizeof(categories); j++)
    {
      if (aux[i] == categories[j][1])
      {
        result[categories[j][0]] = directories[aux[i]];
        break;
      }
    }
  }

  // Tenemos en result un mapping donde cada clave es el tipo de comando
  // (Jugador, Administrador, etc) y cada entrada una lista con los paths
  // completos a todos los comandos de ese tipo

  // Por ultimo filtramos si hay algun tipo de comando con una lista vacia

  aux = keys(result);
  for (i = 0; i < sizeof(aux); i++)
    if (result[aux[i]] == ({ }))
      map_delete(result, aux[i]);

  return result;
}

// called from /lib/living/queue.c, in perform_next_action()

int cmd(string verb, string tail, object thisob)
{
  object ob;
  string s, euid, orig_verb, error;
  int ret;

  orig_verb = verb;

  // Flode, 250499. Security
  if (!thisob || (interactive(thisob) && previous_object(1) != thisob))
    return 0;

  // if (!interactive(thisob) && cmd_dirs[last_dir][0] != LIVING_CMD)
  if (!interactive(thisob))
   return 0;

  // seteuid(ROOT);
  euid = geteuid(thisob);
  s = cmd_aliases[verb];

  if (stringp(s) && (s != ""))
    verb = s;

  s = find_cmd(verb);

  if (!s)
    return 0;

  // write("Executing cmd <"+s+"> with args <"+tail+">\n");

  // Check their position now...
  switch(cmd_dirs[last_dir][0])
  {
    case ADMIN_CMD:
      if (!thisob->query_admin())
        return 0;
      break;
    case MANAGER_CMD:
      if (!thisob->query_manager())
        return 0;
      break;
    case CODER_CMD:
      if (!thisob->query_coder())
        return 0;
      break;
    case PLAYER_CMD:
      if (!thisob->query_player())
        return 0;
      break;
  }

  cmd_hash[verb]["count"]++;

  // Baldrick, 1998-12-05
  // if (!(ob = find_object(s)))
  // {
  //   catch(s->FORCE_LOAD());
  //   ob = find_object(s);
  // }

  ob = load_object(s);

  if (!ob)
  {
    notify_fail("Error cargando el comando.\n");
    return 0;
  }

  if (ob->_query_doclone())
    ob = clone_object(s);

  seteuid(CMD_EUID);
  current_verb = verb;
  ret = (int)ob->_cmd(tail, thisob, orig_verb);
  current_verb = "";

  if (ob->_query_dodest() || ob->_query_doclone())
    ob->dest_me();

  return ret;
}

int cmd_make_hash(int verbose)
{
  // This goes and finds _all_ the valid commands in the listed
  // paths, and also works out the command aliases from the
  // _CMD_ALIASES file in each directory.

  string *paths, *files, s, *a;
  int i, j, k, l, count;

  seteuid(ROOT);
  cmd_hash = ([ ]);
  cmd_aliases = ([ ]);
  count = 0;
  paths = keys(cmd_dirs);

  for (i = 0; i < sizeof(paths); i++)
  {
    if (verbose)
      write("Scanning directory: "+paths[i]+"\n");

    files = get_dir(paths[i] + "*.c");
    // files = get_files(paths[i] + "*.c");

    for (j = 0; j < sizeof(files); j++)
    {
      a = explode(files[j], ".");
      s = implode(a[0..sizeof(a)-2], ".");

      if (verbose)
        write("    Command: "+s+"\n");

      cmd_hash[s] =
        ([
        "file":         paths[i]+s+".c",
        "count":        0,
        "dir":          paths[i],
        ]);
      count++;
    }

    s = read_file(paths[i]+"_CMD_ALIASES");

    // if (stringp(s) && s!=0 && s!="")
    if (stringp(s) && (s != ""))
    {
      a = explode(s, "\n");
      for (j = 0; j < sizeof(a); j++)
      {
        k = 0;
        files = explode(replace(a[j],"\t", " "), " ");

        if (sizeof(files) && files[0] == "alias")
          k++;

        if (sizeof(files) > k)
        {
          for (l = k+1; l < sizeof(files); l++)
            if (files[l] != files[k])
            {
              if (verbose) write("    Alias "+
                files[l]+" to "+
                files[k]+"\n");
              cmd_aliases[files[l]]= files[k];
            }
        }
      }
    }
  }

  seteuid(CMD_EUID);
  return count;
}

mapping query_cmds() { return cmd_dirs; }
mapping query_hash() { return cmd_hash; }
string query_last_dir() { return last_dir; }
mapping query_aliases() { return cmd_aliases; }
string query_alias(string verb) { return cmd_aliases[verb]; }

string query_unaliased_cmd(string verb)
{
  if (cmd_aliases[verb])
    return cmd_aliases[verb];
  else
    return verb;
}

/* Added by Baldrick. */
int soul_com(string str, object me)
{
  string str1, str2;
  int i;

  if (sscanf(str,"%s %s", str1, str2) != 2)
    str1 = str;

  if (!me->query_property("nosoul"))
  {
    if (!load_object(SOUL_OBJECT) )
    {
      // write("Soul errors!  Notify an immortal.\n");
      // write("Use nosoul to turn the soul back on when it is fixed.\n");
      tell_object(me, "Error con las emociones, notifícaselo a un programador.\n");
      // me->add_property("nosoul",1);
      return 0;
    }

    i = SOUL_OBJECT->soul_command(str1, str2, me);

    /* souls are trivial */
    if ( i )
      me->set_trivial_action();
    return i;
  }

  return 0;
} /* soul_com() */

void set_save_all()
{
  // if (base_name(previous_object()) == "/home/flode/fun/nastiness/nastiness")
    save_all = 1;
}

void reset_save_all()
{
  // if (base_name(previous_object()) == "/home/flode/fun/nastiness/nastiness")
    save_all = 0;
}

int query_save_all() { return save_all; }
