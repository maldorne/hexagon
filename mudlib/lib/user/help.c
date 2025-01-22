// The help module. This will (should) give help on the required thingy.
// share and enjoy.
// nroff files removed, neverbot 06/2010

#include <mud/cmd.h>
#include <language.h>

#define help_dirs ({ "/lib/docs/mud/concepts/", \
                     "/lib/docs/mud/important/", \
                     "/lib/docs/helpdir/" })

#define creator_dirs ({ "/lib/docs/lfun/", \
                        "/lib/docs/efun/", \
                        "/lib/docs/coder/", \
                        "/lib/docs/driver/lpc/types/", \
                        "/lib/docs/driver/applies/", \
                        "/lib/docs/driver/concepts/", \
                        "/lib/docs/driver/lpc/constructs/", })


static int compare_strings(string a, string b)
{
  if (a == b) return 0;
  if (!a || !b) return 1;
  if (a > b) return 1;
  return -1;
}

int do_help(string str)
{
  string s, text;
  string * files;
  string * aux;
  int i, j;
  object ob;

  files = ({ });
  aux = ({ });

  if (!strlen(str))
  {
    s = "";
    /*
    s = "%^GREEN%^Temas importantes%^RESET%^:\n";
    s += sprintf("\n%-#*s\n\n", (int)this_user()->query_cols(),
      implode(get_dir("/lib/docs/mud/important/"), "\n"));

    s += "%^GREEN%^Conceptos generales del juego%^RESET%^:\n";
    s += sprintf("\n%-#*s\n\n", (int)this_user()->query_cols(),
      implode(get_dir("/lib/docs/mud/concepts/"), "\n"));

    s += "%^GREEN%^Otros temas%^RESET%^:\n";
    s += sprintf("\n%-#*s\n\n", (int)this_user()->query_cols(),
      implode(get_dir("/lib/docs/helpdir/"), "\n"));

    if (this_object()->query_coder())
    {
      s += "%^GREEN%^Ayuda de comandos de programadores%^RESET%^:\n";
      s += sprintf("\n%-#*s\n\n", (int)this_user()->query_cols(),
        implode(get_dir("/lib/docs/coder/"), "\n"));

      s += "%^GREEN%^Ayuda de programación%^RESET%^:\n";

      aux = ({ });

      for (i = 0; i < sizeof(creator_dirs); i++)
      {
        // Este ya lo hemos mostrado antes
        if (creator_dirs[i] == "/lib/docs/coder/")
          continue;
        // Si no es un directorio, nos lo saltamos
        if (file_size(creator_dirs[i]) != -2)
          continue;
        aux += get_dir(creator_dirs[i]);
      }

      aux = sort_array(aux, "compare_strings");

      s += sprintf("\n%-#*s\n\n", (int)this_user()->query_cols(),
        implode(aux, "\n"));
    }
    */

    s += _LANG_HELP_SYNTAX;

    this_object()->more_string(s + "\n", capitalize(_LANG_HELP_NAME));
    return 1;
  }

  /*
  if (sscanf(str, "hechizo %s", s) == 1 )
  {
    if ((text = this_object()->help_spell(s)) && strlen(text))
      write(text);
    else
      write("No conoces ese hechizo.\n");
    return 1;
  }

  if ((text = this_object()->help_spell(str)) && strlen(text))
  {
    write(text);
    return 1;
  }

  if (sscanf(str, "dote %s", s) == 1 )
  {
    if ((text = this_object()->help_feat(s)) && strlen(text))
      write(text);
    else
      write("No conoces esa dote.\n");
    return 1;
  }

  if ((text = this_object()->help_feat(str)) && strlen(text))
  {
    write(text);
    return 1;
  }
  */

  // check if it is a cmd
  ob = load_object(CMD_HANDLER);
  if (ob)
  {
    text = ob->query_unaliased_cmd(str);
    aux = keys(ob->query_hash());

    // what we are really looking for _is_ a cmd
    if ((ob->query_hash()[text]) && (text = ob->query_hash()[text]["file"]))
    {
      // check if the player has permissions to run the cmd
      if (member_array(text, ob->query_available_cmds(this_object()->player())) != -1)
      {
        if (ob = load_object(text))
        {
          write(_LANG_HELP_CMD);
          write(_LANG_CMD_SYNTAX + ob->query_usage() + "\n\n");
          write((ob->query_help() ? wrap(ob->query_help()) : _LANG_HELP_CMD_NO_HELP) + "\n");
          return 1;
        }
      }
    }
  }

  if (member_array(str, _LANG_HELP_EMOTIONS) != -1)
  {
    s = SOUL_OBJECT->query_soul_list();
    if (!strlen(s))
      write(_LANG_HELP_NO_HELP_ABOUT);
    else
      this_object()->more_string(s, _LANG_HELP_MORE_EMOTIONS);
    return 1;
  }

  /*
  aux = ({ });

  // fill an array in the form 
  // file_name_i, directory_of_file_i, i+1, i+1, i+2, i+2, etc
  for (i = 0; i < sizeof(help_dirs); i++)
  {
    if (file_size(help_dirs[i]) != -2)
      continue;
    aux = get_dir(help_dirs[i]);
    for (j = 0; j < sizeof(aux); j++)
      files += ({ aux[j], help_dirs[i] });
  }

  if (this_object()->query_coder())
    for (i = 0; i < sizeof(creator_dirs); i++)
    {
      if (file_size(creator_dirs[i]) != -2)
        continue;
      aux = get_dir(creator_dirs[i]);
      for (j = 0; j < sizeof(aux); j++)
        files += ({ aux[j], creator_dirs[i] });
    }

  // the help corresponds to a file name
  if ((i = member_array(str, files)) != -1)
  {
    s = "%^GREEN%^Ayuda de: "+str + "%^RESET%^\n\n";
    s += read_file(files[i+1] + files[i]);

    this_object()->more_string(s + "\n", "Ayuda");
    return 1;
  }
  */

  // finally we check if the help about some inventory item exists
  {
    object * objs;
    int flag, loop;
    flag = 0;
    objs = find_match(str, this_player());

    // if we do not have items in the inventory that match the name,
    // finally we try with the help of the emotions
    if (!sizeof(objs))
    {
      s = (string)SOUL_OBJECT->help_soul(str);

      if (!strlen(s))
      {
        notify_fail(_LANG_HELP_NO_HELP_ABOUT);
        return 0;
      }

      s = wrap(s);
      this_user()->set_finish_func("end_of_help");
      this_user()->more_string(s);
      return 1;
    }

    for (loop = 0; loop < sizeof(objs); loop++)
    {
      if (text = (string)objs[loop]->query_help())
      {
        write(_LANG_HELP_HELP_ABOUT);
        flag = 1;
      }
    }

    if (!flag)
      write(_LANG_HELP_NO_HELP_FOR_ITEM);

    return 1;
  }

  notify_fail(_LANG_HELP_NO_HELP_ABOUT);
  return 0;

} /* do_help() */

int end_of_help()
{
  return 1;
}


/*
string search_help(string str)
{
  if (file_size(NROFF_DIR+str+".o") > 0)
    return NROFF_DIR+str;
  if (this_player()->query_coder() && file_size(CNROFF_DIR+str+".o") > 0)
    return CNROFF_DIR+str;
}

string create_help(string str)
{
  int i;
  mixed cross_ref;

  for (i = 0;i < sizeof(help_dirs); i++)
    if (file_size(help_dirs[i]+str)>0)
    {
      NROFF_HAND->create_help(help_dirs[i], str);
      return NROFF_DIR+str;
      return help_dirs[i]+str;
    }

  if (this_player()->query_coder())
    for (i = 0;i < sizeof(creator_dirs); i++)
      if (file_size(creator_dirs[i]+str)>0)
      {
        NROFF_HAND->create_chelp(creator_dirs[i], str);
        return CNROFF_DIR+str;
        return creator_dirs[i]+str;
      }

  cross_ref = read_file("/lib/docs/cross_ref");
  cross_ref = explode(cross_ref, "%");

  if ((i=member_array(str,cross_ref))==-1)
    return 0;

  return extract(cross_ref[i+1],0,strlen(cross_ref[i+1])-2);
  // use extract, not explode
}
*/

mixed * stats()
{
  return ({ });
}