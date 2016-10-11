
// prototypes
object *wiz_present(string str, varargs object onobj, int nogoout);

void create()
{
}

static void role_commands()
{
  add_action("update", "update");
}

static string desc_object(mixed o)
{
  string str;

  if (!o) 
    return "** Espacio-Vacio **";
  
  if (!catch(str = (string)o->short()) && str) 
    return str;
  
  if (!catch(str = (string)o->query_name()) && str) 
    return str;
  
  return file_name(o);
} 

static string desc_f_object(object o)
{
  string str; //, tmp;

  str = desc_object(o);

  if (o && (str != file_name(o))) 
  {
    // if (tmp)
    //   str += " (" + tmp + ")";
    // else
      str += " (" + file_name(o) + ")";
  }

  return str;
} 

static int do_update(object *ov) 
{
  string pname, dummy;
  int i, j;
  object *invent, rsv, env, dup, loaded;
  // next three Hamlet's
  object totell;
  string *load_junk;
  string itemname;
  object thing;

  if (this_player(1) != this_object()->query_player()) 
    return 0;

  // Easy way to preload something...
  "/lib/areas/void"->bingle_bingle();

  rsv = find_object("/lib/areas/void"); // RSV = Room Slash Void
  if (!rsv) 
  { 
    // Die in horror
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
    if (interactive(ov[i]))
    {
      write("Tu NO updateas a '" + ov[i]->query_cap_name() + 
            "', (no se pueden updatear objetos interactive()).\n");
      return 0;
    }

    for (j = 0; j < sizeof(invent); j++)
    {
      if (interactive(ov[i]) || interactive(invent[j]) )
        invent[j]->move(rsv);
      else 
        invent[j]->dest_me(); // Taniwha 1995, so rooms stop filling when updated
    }

    pname = file_name(ov[i]);
    if (sscanf(pname, "%s#%d", pname, dummy) != 2) 
    {
      // Next three Hamlet's
      load_junk = "/lib/core/basic/auto_load"->create_update_auto_load(ov[i]);
      totell = environment(ov[i]);
      itemname = ov[i]->query_short();

      ov[i]->dest_me();
      if (ov[i]) 
          ov[i]->dwep();
      if (ov[i]) 
          destruct(ov[i]);

      // file_size("/secure/master");     Struck me as senseless.
      // Uh, you just dested it.  obviously it's not there...
      // if (!ov[i])
      //   ov[i] = find_object(pname);
      
      catch(call_other(pname, "??"));
      ov[i] = find_object(pname);
    } 
    else 
    {
      loaded = find_object(pname);

      // Next three Hamlet's
      if (loaded) {
        load_junk = "/lib/core/basic/auto_load"->create_update_auto_load(loaded);
        totell = environment(loaded);
        itemname = loaded->query_short();
      }

      if (loaded) 
          loaded->dest_me();
      if (loaded) 
          loaded->dwep();
      if (loaded) 
          destruct(loaded);

      // These are Hamlet's too
      catch(loaded = load_object(pname));
      if (loaded)
          loaded->move(totell);
      else 
      { 
        // Damn thing didn't reload... Oops... 
        if (totell) 
        {
          if (interactive(totell))
            if (strlen(itemname))
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

    if (!ov[i]) 
    {
      if (totell) 
      {
        if (interactive(totell))
          if (strlen(itemname))
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
      if (invent[j]) 
        invent[j]->move(ov[i]);

    if (env) 
      ov[i]->move(env);

    write("Updated: " + desc_f_object(ov[i]) + ".\n");
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
  
  if (!strlen(str) || str == "here") 
  {
    str = file_name(environment(this_player()));
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
    
    if (!sizeof(val)) 
    {
      notify_fail("No hay archivos/objetos coincidentes.\n");
      return 0;
    }
    
    return do_update(val);
  }
  
  obs = ({ });
  
  for (loop = 0; loop < sizeof(filenames); loop++) 
  {
    str = filenames[loop];
    ob = find_object(str);
    if (!ob) 
    {
      if (file_size(str) >= 0) 
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


mixed stats() 
{
  return ({ 
          });
}
            
