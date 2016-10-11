// Radix - Added wis_present call to 'cd' allowing 'cd here'
// or any other object, same as 'ed'
// December 15, 1995

//inherit "/std/basic/ls";
//inherit "/global/le";

// #define MASTER "/secure/master"

inherit base "/lib/user/roles/player_role.c";

inherit find   "/lib/user/roles/coder_find.c";
inherit path   "/lib/user/roles/coder_path.c";
inherit editor "/lib/user/roles/coder_editor.c";

// inherit "/global/coder/coder_object_comm.c";
// inherit "/global/coder/coder_info_comm.c";
// inherit "/global/coder/coder_communicate.c";

static mixed in_editor;

string desc_object(mixed o);
string desc_f_object(object o);

void create()
{
  base::create();
  find::create();
  path::create();
  editor::create();
}

static void role_commands()
{
  add_action("update", "update");
  // add_action("indent", "indent");

  // neverbot 07/05
  // add_action("review", "review");

  // This may not work: (baldrick)
  // coder_object_comm::wiz_commands();
  // coder_info_comm::wiz_commands();
  // coder_communicate::wiz_commands();

  // coder_object_comm::app_commands();
  // coder_info_comm::app_commands();
  // coder_inter_comm::wiz_commands();

  // add_action("list_files","ls");
  // add_action("le", "le");

  add_action("edit","ed");
  add_action("set_home_dir", "homedir");

  // coder_object_comm::all_commands();
  // coder_info_comm::all_commands();

  path::role_commands();
} 














static int do_update(object *ov) 
{
  string pname, dummy;
  int i, j;
  object *invent, rsv, env, dup, loaded;
  /* next three Hamlet's */
  object totell;
  string *load_junk;
  string itemname;
  object thing;

  if (this_player(1) != this_player()) 
      return 0;

  // Easy way to preload something...
  "room/void"->bingle_bingle();

  rsv = find_object("room/void");  /* RSV = Room Slash Void */
  if (!rsv) { /* Die in horror */
    notify_fail("¡La habitación en el vacío ha desaparecido!\n");
    return 0;
  }

  for (i = 0; i < sizeof(ov); i++) 
  {
    if (!ov[i]) 
        continue;
    env = environment(ov[i]);
    invent = all_inventory(ov[i]);

    // Added logging of "update <player>"  Radix, Dec 15, 1995
    if(interactive(ov[i]))
    {
      write("Tu NO updateas a '" + ov[i]->query_cap_name() + 
          "', (no se pueden updatear objetos interactive()).\n");
      return 0;
    }

    for (j = 0; j < sizeof(invent); j++)
    {
      if(interactive(ov[i]) || interactive(invent[j]) )
          invent[j]->move(rsv);
      else 
          invent[j]->dest_me(); // Taniwha 1995, so rooms stop filling when updated
    }

    pname = file_name(ov[i]);
    if (sscanf(pname, "%s#%d", pname, dummy) != 2) 
    {
      /* Next three Hamlet's */
      load_junk = "/global/basic/auto_load"->create_update_auto_load(ov[i]);
      totell = environment(ov[i]);
      itemname = ov[i]->query_short();

      ov[i]->dest_me();
      if (ov[i]) 
          ov[i]->dwep();
      if (ov[i]) 
          destruct(ov[i]);

      //            file_size("/secure/master");     Struck me as senseless.
      /* Uh, you just dested it.  obviously it's not there...
      if (!ov[i])
      ov[i] = find_object(pname);
      */

      catch(call_other(pname, "??"));
      ov[i] = find_object(pname);
    } 
    else 
    {
      loaded = find_object(pname);

      /* Next three Hamlet's */
      if (loaded) {
        load_junk = "/global/basic/auto_load"->create_update_auto_load(loaded);
        totell = environment(loaded);
        itemname = loaded->query_short();
      }

      if (loaded) 
          loaded->dest_me();
      if (loaded) 
          loaded->dwep();
      if (loaded) 
          destruct(loaded);

      /* These are Hamlet's too */
      catch(loaded = load_object(pname));
      if(loaded)
          loaded->move(totell);
      else { /* Damn thing didn't reload... Oops... */
        if(totell) {
          if(interactive(totell))
            if(strlen(itemname))
              tell_object(totell,"Ups. Tu "+itemname+" acaba de romperse.\n");
          else
            tell_object(totell,"Ups. Algo que tenías acaba de romperse.\n");
          thing = clone_object( "/obj/misc/iou" );
          thing->add_auto_string( load_junk );
          thing->move( totell );
        }
      }

      catch(dup = clone_object(pname));
      if (dup && ov[i]) 
      {
        ov[i]->dest_me();
        if (ov[i]) 
            ov[i]->dwep();
        if (ov[i]) 
            destruct(ov[i]);
        ov[i] = dup;
      }
    }

    if (!ov[i]) {
      if(totell) {
        if(interactive(totell))
          if(strlen(itemname))
            tell_object(totell,"Ups. Tu "+itemname+" acaba de romperse.\n");
        else
          tell_object(totell, "Ups. Algo que tenías acaba de romperse.\n");
        thing = clone_object( "/obj/misc/iou" );
        thing->add_auto_string( load_junk );
        thing->move( totell );
      }
      continue;
    }

    for (j = 0; j < sizeof(invent); j++)
    if (invent[j]) invent[j]->move(ov[i]);

    if (env) ov[i]->move(env);
    write("Actualizado (updated): " + desc_f_object(ov[i]) + ".\n");
  }
  return 1;
} /* do_update() */

int update(string str) 
{
  string tring, *filenames, err;
  object ob, *val, *obs;
  int loop;

  notify_fail("No existe tal objeto.\n");
  tring = str;
  
  if (!str || str == "here") 
  {
    str = file_name(environment());
    sscanf(str, "%s#%d", str, loop);
    filenames = ({ "/" + str });
    str = "here";
  }
  else 
  {
    filenames = get_cfiles(str);
  }
  
  if (sizeof(filenames) == 0) 
  {
    val = wiz_present(tring, this_player());
    
    if(!sizeof(val)) 
    {
      notify_fail("No hay archivos/objetos coincidentes.\n");
      return 0;
    }
    
    return do_update(val);
  }
  
  obs = ({ });
  
  for(loop = 0; loop < sizeof(filenames); loop++) 
  {
    str = filenames[loop];
    ob = find_object(str);
    if (!ob) 
    {
      if(file_size(str) >= 0) 
      {
        if (!(err = catch(str->bing_with_me())))
            write(str + " cargado.\n");
        else
            write("No se pudo cargar "+str+", error:\n\t"+err+".\n");
      }
      else 
      {
        val = wiz_present(tring, this_player());
        obs += val;
      }
    }
    else 
    {
      obs += ({ ob });
    }
  }
  
  if (!obs)
    return 0;
  else
    return do_update(obs);
        
} /* update() */

static int edit(string str) 
{
  string *filenames, spam;
  object *things;
  int egg;

  if (this_player(1) != this_player()) return 0;

  if (!str) {
    in_editor = "(hidden)";
    // ed("frog", "fini_editor");
    editor("frog"); // TODO
    return 1;
  }
  /* dodgy idea, but allows 'ed here' or 'ed strawberry' */
  if (sizeof(things = wiz_present(str, this_player()))) {
    spam = file_name(things[0]);
    sscanf(spam, "%s#%d", spam, egg);
    if(spam[0..6] == "/global")
    filenames = get_files(str);
    else
    if (file_size(spam) < 0)
    filenames = ({ spam+".c" });
    else
    filenames = ({ spam });
  } else
  filenames = get_files(str);
  if (!sizeof(filenames)) {
    str = get_path(str);
    } else {
    if (sizeof(filenames) > 0) {
      str = filenames[0];
      if (sizeof(filenames) > 1) {
        int loop;

        loop = 0;
        while(loop < sizeof(filenames) && file_size(filenames[loop]) < 0)
        loop++;
        if(loop >= sizeof(filenames)) {
          write("El archivo no existe.\n");
          return 0;
        }
        else {
          str = filenames[loop];
        }
        write("Ambiguo, usando: " + str + "\n");
      }
    }
  }
  if (file_size(str) == -2) {
    write("Es un directorio.\n");
    return 1;
  }
  in_editor = str;
  if (!valid_write(str, geteuid(), "frog"))
    write("[sólo lectura] ");
  // ed(str, "fini_editor");
  editor(str); // TODO
  return 1;
} /* edit() */

void set_in_editor(mixed funcname) 
{
  int fsize3;
  fsize3 = strlen(file_name(previous_object()));
  
  if(file_name(previous_object())[fsize3-8..fsize3-1] == "/cmds/ed")
    in_editor = funcname;
}

mixed query_in_editor() { return in_editor; }

void fini_editor() {
  in_editor = 0;
} /* fini_editor() */

static int set_home_dir(string str) 
{
  if (this_player(1) != this_object()) 
      return 0;
  if (str) 
      home_dir = get_path(str);
    write("Homedir establecido en '"+home_dir+"'.\n");
  return 1;
} 

// int review() 
// {
//   player::review();
//   coder_info_comm::review();
//   return 1;
// } 


mixed stats() 
{
  return ({ }) + base::stats() + 
                 find::stats() + 
                 path::stats() + 
                 editor::stats();
}