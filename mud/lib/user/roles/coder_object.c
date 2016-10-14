
#include <mud/secure.h>

// prototypes
object *wiz_present(string str, object onobj, varargs int nogoout);
void inform_of_call(object ob, mixed *argv);
static mixed *parse_args(string str, string close);

private static object *dest_obj;
private static int objn, majd;

void create()
{
}

static void role_commands()
{
  add_action("update", "update");
  add_action("do_a_call", "call");
  add_action("get_creator","coder");
  add_action("get_inv","inv");

  // add_action("parse_frogs", ";*");
  // add_action("dest", "destruct");
  // add_action("dest", "destruir");
  add_action("do_dest", "dest");

  // add_action("show_stats", "stat");
  add_action("trans", "trans");
  add_action("whereis","whereis");
  add_action("get_pathof","pathof");
  add_action("goback","goback");
  // add_action("upgrade_player", "upgrade");
  add_action("find_shadows", "shadows");
  add_action("do_find", "find");
  add_action("do_debug", "dump");
  add_action("do_debug", "debug");  
}

static string desc_object(mixed o)
{
  string str;

  if (!o) 
    return "** Espacio-Vacio **";
  
  if (!catch(str = (string)o->short()) && strlen(str)) 
    return str;
  
  if (!catch(str = (string)o->query_name()) && strlen(str)) 
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
  ROOM_VOID->bingle_bingle();

  rsv = find_object(ROOM_VOID); // RSV = Room Slash Void
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

  if (this_player(1) != this_player()) 
    return 0;

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

int do_a_call(string str) 
{
  /* call fish(x,y,z) object */
  mixed * args;
  string *s, s1, s2;
  string fn, os;
  mixed *argv;
  object *ov;
  mixed retobj;
  object fish, shad;
  string f, file;
  int i;

  if (this_player(1) != this_player()) 
    return 0;
  
  if (!strlen(str)) 
  {
    notify_fail("Sintaxis: call lfun(arg[,arg[,arg...]]) objeto[s]\n");
    return 0;
  }

  log_file("calls", (string)this_player()->query_cap_name()+
                    " calls "+str+" ["+ctime(time(),4)+"]\n");

  s = explode("&"+str+"&", ")");

  if (sizeof(s) < 2 || sscanf(s[0], "%s(%s", s1, s2) != 2) 
  {
    notify_fail("Sintaxis: call lfun(arg[,arg[,arg...]]) objeto[s]\n");
    return 0;
  }

  fn = replace(s1[1..], " ", "");
  s[0] = s2;
  args = parse_args(implode(s, ")"), ")");

  if (!sizeof(args)) 
    return 1;
  
  argv = args[0];
  os = args[1][0..strlen(args[1])-2];
 
  while (strlen(os) && os[0] == ' ') 
    os = os[1..];
  
  notify_fail("No puedo encontrar el objeto "+os+".\n");
  ov = wiz_present(os, this_player());

  if (!sizeof(ov)) 
    return 0;

  // if (sizeof(argv) < 6) 
  //   argv += allocate(6 - sizeof(argv));

  for (i = 0; i < sizeof(ov); i++) 
  {
    fish = ov[i];
    
    if (fish != this_player() && fish->query_coder() && !this_player()->query_admin())
    {
      log_file("call_prog", (string)this_player()->query_cap_name()+
        " calls '"+str+"' ["+ctime(time(),4)+"]\n");
      notify_fail("Tu intento de hacer un call sobre un inmortal ha sido "+
        "grabado.\n");
      return 0;
    }
    
    while (shad = shadow(fish, 0)) 
    {
      fish = shad;
      if (f = function_exists(fn, fish)) 
        file = f;
    }

    if (!file || !strlen(file)) 
      file = function_exists(fn, ov[i]);
    
    if (file && strlen(file)) 
    {
      string error;

      switch(sizeof(argv))
      {
        case 0:
          error = catch(retobj = call_other(ov[i], fn));
          break;
        case 1:
          error = catch(retobj = call_other(ov[i], fn, argv[0]));
          break;
        case 2:
          error = catch(retobj = call_other(ov[i], fn, argv[0], argv[1]));
          break;
        case 3:
          error = catch(retobj = call_other(ov[i], fn, argv[0], argv[1], argv[2]));
          break;
        case 4:
          error = catch(retobj = call_other(ov[i], fn, argv[0], argv[1], argv[2], argv[3]));
          break;
        case 5:
          error = catch(retobj = call_other(ov[i], fn, argv[0], argv[1], argv[2], argv[3], argv[4]));
          break;
        case 6:
          error = catch(retobj = call_other(ov[i], fn, argv[0], argv[1], argv[2], argv[3], argv[4], argv[5]));
          break;
        default:
          notify_fail("More than 6 parameters? Nope.\n");
          return 0;
      }

      if (error)
      {
        write("Se ha producido un error:\n" + error + "\n");
        return 1;
      }

      inform_of_call(ov[i], ({ fn }) + argv);
  
      if ( ov[i] && interactive(ov[i]) && !ov[i]->query_coder() )
        log_file("call_player",(string)this_player()->query_cap_name()+" calls '"+str+
          "' -- player "+(string)ov[i]->query_name()+" ["+ctime(time(),4)+"]\n");
      
      write("*** Función en '"+ desc_object(ov[i])+"' encontrada en "+ file+ " ***\n");
      write("Returned: " + to_string(retobj) + "\n");
      // printf("%O\n", retobj);
    } 
    else
      write("*** Función en '"+desc_object(ov[i])+"' no encontrada ***\n");
    
    file = "";
  }

  return 1;
} /* do_a_call() */

// returns true if s is a 'yes' equiv response 
int affirmative(string s) 
{
  s = lower_case(s);
  return (s == "y" || s == "s" || s == "si" || s == "ok" || s == "please");
} /* affirmative() */

void ask_dest() 
{
  if (!pointerp(dest_obj) || objn >= sizeof(dest_obj)) 
  {
    write("No hay más cosas por destruir.\n");
    dest_obj = ({ }); // wipe array to free memory 
    return;
  }

  write("¿Destruir " + desc_object(dest_obj[objn]) + "? ");
  input_to("dest_answer");
  return;
} /* ask_dest() */

void dest_answer(string s)
{
  string err, shrt;

  if (affirmative(s)) 
  {
    if (majd) 
    {
      shrt = (string)dest_obj[objn]->short();
      err = catch(dest_obj[objn]->dwep());
      write("DWEP falló: " + err + "\n");
    
      if (dest_obj[objn]) 
      {
        write("Este objeto no quiere ser destruido.\n");
        err = catch(destruct(dest_obj[objn]));
        write("destruct() falló: " + err + "\n");
      }

      majd = 0;
      
      if (dest_obj[objn]) 
        write("No se pudo destruir.\n");
      else 
      {
        say((string)this_player()->query_cap_name()+" destruye "+
          (shrt ? shrt : "algo") +".\n");
        write("Ok.\n");
      }

      objn++;
      ask_dest();
      return;
    } 
    else 
    {
      err = catch(dest_obj[objn]->dest_me());
      write("dest_me falló: " + err + "\n");
    
      if (dest_obj[objn]) 
      {
        write("Este objeto no desea ser destruido, ¿aún deseas hacerlo?");
        majd = 1;
        input_to("dest_answer");
        return;
      }

      write("Ok.\n");
      objn++;
      ask_dest();
      return;
    }
  } 
  else if (s == "q" || s == "quit") 
  {
    write("Ok. No se destruiran más objetos.\n");
    dest_obj = ({ });
    return;
  }

  write("Ok. No destruido.\n");
  objn++;
  ask_dest();
  return;
} /* dest_answer() */

int do_dest(string str) 
{
  object *ob;
  int i;
  string qstr, err, shrt, dobj;

  dest_obj = ({ });

  if (!strlen(str)) 
  {
    notify_fail("¿Destruir el qué?\n");
    return 0;
  }

  notify_fail("No puedo encontrar '" + str + "' para destruir.\n");

  if (sscanf(str, "query %s", qstr) == 1) 
  {
    dest_obj = wiz_present(qstr, this_player());
    if (!sizeof(dest_obj)) 
      return 0;
    objn = 0;
    majd = 0; // MAJOR dest needed
    ask_dest();
    return 1;
  }

  ob = wiz_present(str, this_player());
  
  if (!sizeof(ob)) 
    return 0;

  for (i = 0; i < sizeof(ob); i++)
  {
    if(interactive(ob[i]) && (sizeof(ob) !=1 || 
      "/lib/core/secure"->high_programmer(geteuid(ob[i]))))
    {
      write("No destruyes a " + ob[i]->query_cap_name() + 
            " (no puedes destruir objetos interactive).\n");
      continue;
    }
    // if(interactive(ob[i]) && !(this_player()->query_admin() || this_player()->query_thane()))
    if(interactive(ob[i]) && !(this_player()->query_admin()) )
    {
      log_file("dest", "["+ctime(time(), 4)+"] " + 
        this_player()->query_cap_name()+" intentó (sin éxito) destruir el objeto interactive '"+
        ob[i]->query_cap_name()+"'.\n");
      write("Lo siento, no te está permitido destruir a "+ob[i]->query_cap_name()+".\n");
      continue;
    }

    catch(shrt = (string)ob[i]->short());
    dobj = desc_object(ob[i]);

    if( ob[i] && ob[i]->query_cap_name() && environment(ob[i]) )
    {
      // log_file("dest",(string)this_player(1)->query_cap_name()+
      log_file("dest",  "["+ctime(time(), 4)+"] " + 
        (string)this_player()->query_cap_name()+
        " destruye "+ob[i]->query_cap_name() + /* " de "+
        environment(ob[i])->query_short() + */ " en " +
        file_name(environment(ob[i]))+"\n");
      
      event(users(), "inform", (string)this_player()->query_cap_name() +
        " destruye "+ob[i]->query_cap_name() + /* " de "+
        environment(ob[i])->query_short() + */ " en "+
        file_name(environment(ob[i])), "dest");
    }

    err = catch(ob[i]->dest_me());
    
    if (err)
      write("dest_me falló: " + err + "\n");

    if (ob[i])
      dest_obj += ({ ob[i] });
    else
    {
      write("Destruyes " + dobj + ".\n");
      say((string)this_player()->query_cap_name()+" destruye "+
        (shrt ? shrt : "algo") + ".\n");
    }
  }
  
  if (sizeof(dest_obj) > 0) 
  {
    objn = 0;
    majd = 0;
    ask_dest();
    return 1;
  }
  
  return 1;
} /* dest() */

int get_creator(string str) 
{
  object * ov;
  int i;

  notify_fail("Creator of what?\n");
  ov = wiz_present(str,this_player());

  if (!sizeof(ov)) 
    return 0;

  for (i = 0; i < sizeof(ov); i++) 
  {
    write("Creator of " + desc_object(ov[i]) + ": " +
          SECURE->creator_file (file_name(ov[i])) + 
          ", uid: " + getuid(ov[i]) + 
          ", euid: " + geteuid(ov[i]) + "\n");
  }

  return 1;
} /* get_creator() */

int get_inv(string str) 
{
  object *ov, obj;
  int i;

  // thanks for the great error message!
  notify_fail("¿Inventario de qué?\n"); 

  if (!strlen(str)) 
  {
    ov = ({ this_player() });
  } 
  else 
  {
    sscanf(str, "de %s", str);
    ov = wiz_present(str, this_player());
  }

  if (!sizeof(ov)) 
    return 0;

  for (i = 0; i < sizeof(ov); i++) 
  {
    if (!ov[i]) 
      continue;
    
    if( (interactive(ov[i])) && !(ov[i]->query_coder()) && 
       !(this_object()->query_admin()) && !(ov[i] == this_object()))
    {
      log_file("inv", this_object()->query_cap_name()+" intentó ver el inventario del interactive: "+
                      ov[i]->query_cap_name()+" ["+ctime(time(), 4)+"]\n");
      write("No te está permitido ver el inventario de "+ov[i]->query_cap_name()+".\n");
      continue;
    }

    write("Inventario de " + desc_object(ov[i]) + " en " +
          desc_object(environment(ov[i])) + ":\n");

    obj = first_inventory(ov[i]);
    
    while (obj) 
    {
      write("  " + desc_f_object(obj) + "\n");
      obj = next_inventory(obj);
    }
  }

  return 1;
} /* inv() */

mixed stats() 
{
  return ({ 
          });
}
            
