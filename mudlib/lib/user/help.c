
// The help module. This will (should) give help on the required thingy.
// share and enjoy.
// Eliminados archivos nroff, neverbot 06/2010

#include <mud/cmd.h>

#define help_dirs ({ "/lib/docs/mud/concepts/", "/lib/docs/mud/important/","/lib/docs/helpdir/"})

#define creator_dirs ({ "/lib/docs/lfun/", "/lib/docs/efun/", "/lib/docs/coder/",\
"/lib/docs/driver/lpc/types/", "/lib/docs/driver/applies/", \
"/lib/docs/driver/concepts/", "/lib/docs/driver/lpc/constructs/", })


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

    s += "\nSintaxis: ayuda <tema>\n";

    this_object()->more_string(s + "\n", "[Ayuda]");

    return 1;
  }

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

  // Comprobamos si es un comando de cmds
  ob = load_object(CMD_HANDLER);
  if (ob)
  {
    text = ob->query_unaliased_cmd(str);
    aux = keys(ob->query_hash());

    // Lo que estamos buscando realmente _es_ un cmd
    if ((ob->query_hash()[text]) && (text = ob->query_hash()[text]["file"]))
    {
      // Comprobamos si el jugador tiene permiso sobre el cmd
      if (member_array(text, ob->query_available_cmds(this_object()->player())) != -1)
      {
        if (ob = load_object(text))
        {
          write("Ayuda sobre el comando '" + str + "':\n");
          write( ((ob->query_help())?(ob->query_help()):("No hay ningún texto de ayuda concreto."))
            + "\n\n");
          return 1;
        }
      }
    }
  }

  // Ya no hay 'mirar soul', neverbot 02/2006
  if (( str == "emocion") || (str == "emoción") || (str == "emociones"))
  {
    s = SOUL_OBJECT->query_soul_list();
    if (!strlen(s))
      write("No hay ayuda sobre eso.\n");
    else
      this_object()->more_string(s, "[Emociones]");
    return 1;
  }

  aux = ({ });

  // Rellenamos un array de la forma
  // nombre_de_archivo_i, directorio_del_archivo_i, i+1, i+1, i+2, i+2, etc
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

  // La ayuda se corresponde con un nombre de archivo
  if ((i = member_array(str, files)) != -1)
  {
    s = "%^GREEN%^Ayuda de: "+str + "%^RESET%^\n\n";
    s += read_file(files[i+1] + files[i]);

    this_object()->more_string(s + "\n", "[Ayuda]");
    return 1;
  }

  // Por ultimo comprobamos si la ayuda existe sobre un objeto del inventario
  {
    object * objs;
    int flag, loop;
    flag = 0;
    objs = find_match(str, this_player());

    // Si no tenemos objetos en el inventario que se correspondan con el nombre,
    // por último probamos con la ayuda de las emociones
    if (!sizeof(objs))
    {
      s = (string)SOUL_OBJECT->help_soul(str);

      if (!strlen(s))
      {
        notify_fail("Lo siento, no hay ayuda sobre "+str+".\n");
        return 0;
      }

      s = sprintf("%-=*s", this_user()->query_cols(), s);
      this_player()->set_finish_func("end_of_help");
      this_player()->more_string(s);
      return 1;
    }

    for(loop = 0; loop < sizeof(objs); loop++)
    {
      if(text = (string)objs[loop]->get_help())
      {
        write("Ayuda sobre " + objs[loop]->query_name() + ":\n" + text + "\n\n");
        flag = 1;
      }
    }

    if(!flag)
    {
      if(sizeof(objs) > 1)
        write("No hay ayuda sobre estos objetos.\n");
      else
        write("No hay ayuda sobre este objeto.\n");
    }

    return 1;
  }

  notify_fail("No hay ayuda sobre "+str+".\n");
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

mixed stats()
{
  return ({ });
}