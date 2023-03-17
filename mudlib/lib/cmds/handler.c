
// /global/cmd/cmd_handler.c  -- Object based command handler
// Removed the #ifdefs (don't ask me why)
// Added verb Baldrick, dec '97
// Ported to dgd, neverbot aug '15

#include <mud/cmd.h>
#include <mud/secure.h>
#include <language.h>

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
       "/lib/cmds/user/"   : ({ USER_CMD,    CMD_CATEGORY_USER }),
       "/lib/cmds/player/" : ({ PLAYER_CMD,  CMD_CATEGORY_PLAYER }),
      "/game/cmds/player/" : ({ PLAYER_CMD,  CMD_CATEGORY_PLAYER }),
       "/lib/cmds/coder/"  : ({ CODER_CMD,   CMD_CATEGORY_CODER }),
      "/game/cmds/coder/"  : ({ CODER_CMD,   CMD_CATEGORY_CODER }),
       "/lib/cmds/admin/"  : ({ ADMIN_CMD,   CMD_CATEGORY_ADMIN }),
      "/game/cmds/admin/"  : ({ ADMIN_CMD,   CMD_CATEGORY_ADMIN }),
      // "/net/cmds/":              ({ 0,   CMD_CATEGORY_NET }),
      "/lib/cmds/meta/"    : ({ ADMIN_CMD,   CMD_CATEGORY_HANDLER }),
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

mapping query_cmd_dirs() { return cmd_dirs; }
mapping query_hash() { return cmd_hash; }
string query_last_dir() { return last_dir; }
mapping query_aliases() { return cmd_aliases; }
string query_alias(string verb) { return cmd_aliases[verb]; }

void set_save_all() { save_all = 1; }
void reset_save_all() { save_all = 0; }
int query_save_all() { return save_all; }

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

  // get all object permissions
  perms += ({ USER_CMD, });
  if (player->query_player())
    perms += ({ PLAYER_CMD, });
  if (player->query_coder())
    perms += ({ CODER_CMD, });
  if (player->query_admin())
    perms += ({ ADMIN_CMD, });

  // get every possible cmd directories
  aux = keys(cmd_dirs);

  // filter directories with our object permissions
  for (i = 0; i < sizeof(aux); i++)
  {
    if (member_array(cmd_dirs[aux[i]][0], perms) != -1)
      dirs += ({ aux[i] });
  }

  // dirs now has the directories the object has access to
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

  result += ({ "/lib/cmds/user/", });

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
  if (player->query_admin())
  {
    result += ({
                "/lib/cmds/admin/",
                "/game/cmds/admin/",
                "/lib/cmds/meta/",
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

// show the cmd hash info for object player
// of the category type = filter
mapping query_hash_by_category(int filter)
{
  string * aux;
  mapping result;
  int i;

  result = ([ ]);
  aux = keys(cmd_hash);

  for (i = 0; i < sizeof(aux); i++)
    if (cmd_hash[aux[i]]["category"] == filter)
      result[aux[i]] = cmd_hash[aux[i]];

  return result;
}

private string query_category_name(int category)
{
  switch(category)
  {
    case USER_CMD:
      return CMD_CATEGORY_USER;
    case PLAYER_CMD:
      return CMD_CATEGORY_PLAYER;
    case CODER_CMD:
      return CMD_CATEGORY_CODER;
    case ADMIN_CMD:
      return CMD_CATEGORY_ADMIN;
    default:
      // should not happen?
      return CMD_CATEGORY_PLAYER;
  }     
}

private int query_category_type(string category)
{
  switch(category)
  {
    case CMD_CATEGORY_USER:
      return USER_CMD;
    case CMD_CATEGORY_PLAYER:
      return PLAYER_CMD;
    case CMD_CATEGORY_CODER:
      return CODER_CMD;
    case CMD_CATEGORY_ADMIN:
    case "admin":
      return ADMIN_CMD;
    default:
      // should not happen?
      return PLAYER_CMD;
  }     
}

// if filter is provided, show only the available cmds for object player
// of the category type = filter
mapping query_available_cmds_by_category(object player, varargs int filter)
{
  int i, * perms;
  string * aux;
  mixed * categories;
  mapping result;

  aux = ({ });
  categories = ({ });
  result = ([ ]);
  perms = ({ });

  if (!player)
    return nil;

  // get all object permissions
  perms += ({ USER_CMD, });
  if (player->query_player())
    perms += ({ PLAYER_CMD, });
  if (player->query_coder())
    perms += ({ CODER_CMD, });
  if (player->query_admin())
    perms += ({ ADMIN_CMD, });

  // get all possible cmd types
  aux = keys(cmd_dirs);

  for (i = 0; i < sizeof(aux); i++)
  {
    if (member_array(cmd_dirs[aux[i]][0], perms) == -1)
      continue;

    if (filter && (cmd_dirs[aux[i]][0] != filter))
      continue;

    if (member_array(cmd_dirs[aux[i]][1], categories) == -1)
      categories += ({ cmd_dirs[aux[i]][1] });
    // directories[aux[i]] = ({ });
  }

  aux = keys(cmd_hash);

  // get all possible cmds and put them in its category
  for (i = 0; i < sizeof(aux); i++)
  {
    string category;

    category = query_category_name(cmd_hash[aux[i]]["category"]);

    if (member_array(category, categories) == -1)
      continue;

    if (undefinedp(result[category]))
      result[category] = ({ });

    result[category] += ({ cmd_hash[aux[i]]["file"], });
  }

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
  if (!thisob)
    return 0;

  // allow commands being called from the login
  if ((previous_object(1) != thisob) && (!SECURE->valid_progname("/lib/core/login")))
    return 0;

  // if (!interactive(thisob) && cmd_dirs[last_dir][0] != LIVING_CMD)
  if (!interactive(thisob))
   return 0;

  // seteuid(ROOT);
  euid = geteuid(thisob);
  s = cmd_aliases[verb];

  if (!s)
    return 0;

  verb = s;

  s = find_cmd(s);

  if (!s)
    return 0;

  // Check their position now...
  switch(cmd_dirs[last_dir][0])
  {
    case ADMIN_CMD:
      if (!thisob->query_admin())
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
    // always allow
    case USER_CMD:
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
    notify_fail(_LANG_CMD_ERROR_LOADING);
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

  string * paths, * packages, * files, s, * a, err, * aliases;
  int i, j, k, l, count;
  object cmd;

  seteuid(ROOT);
  cmd_hash = ([ ]);
  cmd_aliases = ([ ]);
  count = 0;
  paths = keys(cmd_dirs);

  for (i = 0; i < sizeof(paths); i++)
  {
    if (verbose)
      write("Scanning directory: " + paths[i] + "\n");

    files = get_dir(paths[i] + "*.c");
    // files = get_files(paths[i] + "*.c");

    for (j = 0; j < sizeof(files); j++)
    {
      err = catch(cmd = load_object(paths[i] + files[j]));

      // we are not using cmds that don't load
      if (err || !cmd)
      {
        if (verbose)
          write("Error loading: " + paths[i] + files[j] + "\n");

        if (package("issues"))
          package("issues")->add_issue("Cmd " + paths[i] + files[j] + " does not load", this_object());

        continue;
      }

      a = explode(files[j], ".");
      s = implode(a[0..sizeof(a)-2], ".");

      if (verbose)
        write("  Command: " + s + "\n");

      cmd_hash[s] =
        ([
          "file":     base_name(cmd),
          "count":    0,
          "category": cmd_dirs[paths[i]][0],
          "dir":      paths[i],
        ]);
      count++;

      // multilanguage: only execute cmds with provided aliases
      aliases = cmd->query_aliases();

      // if we do not have any alias, as a fallback use the filename
      if (sizeof(aliases) == 0)
        aliases = ({ s });

      for (k = 0; k < sizeof(aliases); k++)
      {
        if (verbose && (aliases[k] != s))
          write("    Alias: " + aliases[k] + "\n");

        cmd_aliases[aliases[k]] = s;
      }
    }
  }

  // now, load cmds from packages
  packages = get_dir("/packages/*");

  for (i = 0; i < sizeof(packages); i++)
  {
    string * directories;

    // not loadable packages
    if (!package(packages[i]))
      continue;

    directories = get_dir("/packages/" + packages[i] + "/cmds/*");

    // this package does not provide cmds
    if (!sizeof(directories))
      continue;

    for (j = 0; j < sizeof(directories); j++)
    {
      int type;
      string path;

      type = query_category_type(directories[j]);

      path = "/packages/" + packages[i] + "/cmds/" + directories[j];

      if (verbose)
        write("Scanning directory: " + path + "\n");

      files = get_dir(path + "/*.c");

      for (k = 0; k < sizeof(files); k++)
      {
        err = catch(cmd = load_object(path + "/" + files[k]));

        // we are not using cmds that don't load
        if (err || !cmd)
        {
          if (verbose)
            write("Error loading: " + path + "/" + files[k] + "\n");
          continue;
        }

        a = explode(files[k], ".");
        s = implode(a[0..sizeof(a)-2], ".");

        if (verbose)
          write("  Package command: " + s + "\n");

        cmd_hash[s] =
          ([
            "file":     base_name(cmd),
            "count":    0,
            "category": type,
            "dir":      path + "/",
            "package":  packages[i],
          ]);
        count++;

        // add the directory to the cmd_dirs mapping
        cmd_dirs[path + "/"] = ({ type, query_category_name(type) });

        // multilanguage: only execute cmds with provided aliases
        aliases = cmd->query_aliases();

        // if we do not have any alias, as a fallback use the filename
        if (sizeof(aliases) == 0)
          aliases = ({ s });

        for (l = 0; l < sizeof(aliases); l++)
        {
          if (verbose && (aliases[l] != s))
            write("    Package command alias: " + aliases[l] + "\n");

          cmd_aliases[aliases[l]] = s;
        }
      }
    }
  }

  seteuid(CMD_EUID);

  if (verbose)
    write("Total: "+count+" commands\n");

  return count;
}

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
      // write("Use nosoul to turn the soul back on when it is fixed.\n");
      tell_object(me, _LANG_CMD_NO_SOULS);
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
