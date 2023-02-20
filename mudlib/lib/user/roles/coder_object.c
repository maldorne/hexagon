
#include <mud/secure.h>
#include <areas/common.h>
#include <language.h>

// prototypes
object *wiz_present(string str, object onobj, varargs int nogoout);
void inform_of_call(object ob, mixed *argv);
mixed *parse_args(string str, string close);
string desc_object(mixed o);
string desc_f_object(object o);

private static object *dest_obj;
private static int objn, majd;

void create()
{
}

void role_commands()
{
  add_action("do_update", "update");

  add_action("do_a_call", "call");
  add_action("get_creator", "coder");
  add_action("get_inv", "inv");

  // add_action("parse_frogs", ";*");
  // add_action("show_stats", "stat");
  // add_action("upgrade_player", "upgrade");

  add_action("do_dest", "dest");

  add_action("trans", "trans");
  add_action("whereis", "whereis");
  add_action("get_pathof", "pathof");
  add_action("goback", "goback");
  add_action("do_find", "find");
  add_action("do_debug", ({ "dump", "debug" }));

  add_action("find_shadows", "shadows");
}

private int _update_objects(object *ov)
{
  string filename;
  int dummy, cloned;
  int i, j;
  object *invent, rsv, env, dup, loaded, ob;
  // next three Hamlet's
  object totell;
  mapping load_junk, attributes;
  string itemname;
  object thing;

  if (this_player(1) != this_user())
    return 0;

  load_object(ROOM_VOID);

  rsv = find_object(ROOM_VOID); // RSV = Room Slash Void
  if (!rsv)
  {
    // Die in horror
    notify_fail(_LANG_CODER_OBJECT_NO_RSV);
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
      notify_fail(_LANG_CODER_OBJECT_DO_NOT_UPDATE);
      return 0;
    }

    filename = file_name(ov[i]);

    if (sscanf(filename, "%s#%d", filename, dummy) != 2) // a room? a handler? something not cloned
      cloned = false;
    else
      cloned = true;

    if (ov[i])
    {
      load_junk = "/lib/core/basic/auto_load"->create_auto_load( ({ ov[i] }) );
      totell = environment(ov[i]);
      itemname = ov[i]->query_short();
      attributes = ov[i]->query_auto_load_attributes();
    }

    for (j = 0; j < sizeof(invent); j++)
    {
      if (interactive(invent[j]))
        invent[j]->move(rsv);
      else
        invent[j]->dest_me(); // Taniwha 1995, so rooms stop filling when updated
    }

    invent -= ({ nil });

    if (ov[i])
      ov[i]->dest_me();
    if (ov[i])
      ov[i]->dwep();
    if (ov[i])
      destruct(ov[i]);

    // reload after destruction
    catch(loaded = load_object(filename));

    if (!cloned)
      ov[i] = find_object(filename);
    else
    {
      if (loaded)
        loaded->move(totell);
      else
      {
        // Damn thing didn't reload... Oops...
        if (totell)
        {
          if (interactive(totell))
            if (strlen(itemname))
              tell_object(totell, _LANG_CODER_OBJECT_NAME_BROKEN);
          else
            tell_object(totell, _LANG_CODER_OBJECT_BROKEN);
          thing = clone_object("/lib/obj/iou");
          thing->add_auto_load_info(filename, load_junk[filename]);
          thing->move( totell );
        }
      }

      catch(dup = clone_object(filename));

      ov[i] = dup;

      if (!ov[i])
      {
        if (totell)
        {
          if (interactive(totell))
            if (strlen(itemname))
              tell_object(totell, _LANG_CODER_OBJECT_NAME_BROKEN);
          else
            tell_object(totell, _LANG_CODER_OBJECT_BROKEN);
          thing = clone_object("/lib/obj/iou");
          thing->add_auto_load_info(filename, load_junk[filename]);
          thing->move( totell );
        }
        continue;
      }
      // neverbot 01/2021
      // hope this works :/
      else
      {
        ov[i]->init_auto_load_attributes(attributes);
      }
    }

    // just players in this list, every other thing was destroyed
    for (j = 0; j < sizeof(invent); j++)
      if (invent[j])
        invent[j]->move(ov[i]);

    // updating something in our inventory?
    if (env)
      ov[i]->move(env);

    write(_LANG_CODER_OBJECT_UPDATED + desc_f_object(ov[i]) + ".\n");
  }

  return 1;
} /* _update_objects() */

int do_update(string str)
{
  string tring, *filenames, err;
  object ob, *val, *obs;
  int loop;

  notify_fail(_LANG_CODER_OBJECT_DONT_EXIST);
  tring = str;

  // if (this_player(1) != this_user())
  //   return 0;

  if (!strlen(str) || (member_array(str, _LANG_CODER_OBJECT_HERE) != -1))
  {
    str = _LANG_CODER_OBJECT_HERE[0];
    filenames = ({ base_name(environment(this_player())) });
  }
  else
    filenames = get_cfiles(str);

  if (sizeof(filenames) == 0)
  {
    val = wiz_present(tring, this_player());

    if (!sizeof(val))
    {
      notify_fail(_LANG_CODER_OBJECT_NO_OBJECTS);
      return 0;
    }

    return _update_objects(val);
  }

  obs = ({ });

  for (loop = 0; loop < sizeof(filenames); loop++)
  {
    str = filenames[loop];
    ob = load_object(str);

    if (!ob)
    {
      if (file_size(str) >= 0)
      {
        // stop trying to do this, if load_object did not work, this
        // won't work either.
        //    neverbot 01/2017
        // if (err = catch(str->bing_with_me()))
        //   write(_LANG_CODER_OBJECT_CANNOT_LOAD + str + ", error:\n\t" + err + ".\n");
        write(_LANG_CODER_OBJECT_CANNOT_LOAD);
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
    return _update_objects(obs);

} /* do_update() */

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

  if (this_player(1) != this_user())
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

      // not using a catch, so the error will get to the driver
      // and the error handler will inform the coder about program and line
      // catch {
        switch(sizeof(argv))
        {
          case 0:
            retobj = call_other(ov[i], fn);
            break;
          case 1:
            retobj = call_other(ov[i], fn, argv[0]);
            break;
          case 2:
            retobj = call_other(ov[i], fn, argv[0], argv[1]);
            break;
          case 3:
            retobj = call_other(ov[i], fn, argv[0], argv[1], argv[2]);
            break;
          case 4:
            retobj = call_other(ov[i], fn, argv[0], argv[1], argv[2], argv[3]);
            break;
          case 5:
            retobj = call_other(ov[i], fn, argv[0], argv[1], argv[2], argv[3], argv[4]);
            break;
          case 6:
            retobj = call_other(ov[i], fn, argv[0], argv[1], argv[2], argv[3], argv[4], argv[5]);
            break;
          default:
            notify_fail("More than 6 parameters? Nope.\n");
            return 0;
        }
      // } : {
      //     write("Se ha producido un error durante el call.\n");
      //     return 1;
      // }

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
    if (interactive(ob[i]) && (sizeof(ob) !=1 ||
      MASTER->high_programmer(geteuid(ob[i]))))
    {
      write("No destruyes a " + ob[i]->query_cap_name() +
            " (no puedes destruir objetos interactive).\n");
      continue;
    }
    // if (interactive(ob[i]) && !(this_player()->query_admin() || this_player()->query_thane()))
    if (interactive(ob[i]) && !(this_player()->query_admin()) )
    {
      log_file("dest", "["+ctime(time(), 4)+"] " +
        this_player()->query_cap_name()+" tried (unsuccessfully) to dest the interactive object '"+
        ob[i]->query_cap_name()+"'.\n");
      write("Lo siento, no te está permitido destruir a "+ob[i]->query_cap_name()+".\n");
      continue;
    }

    catch(shrt = (string)ob[i]->short());
    dobj = desc_object(ob[i]);

    if ( ob[i] && ob[i]->query_cap_name() && environment(ob[i]) )
    {
      // log_file("dest",(string)this_player(1)->query_cap_name()+
      log_file("dest",  "["+ctime(time(), 4)+"] " +
        (string)this_player()->query_cap_name()+
        " dest'ed "+ob[i]->query_cap_name() + /* " de "+
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
  notify_fail("Inventory of what?\n");

  if (!strlen(str))
  {
    ov = ({ this_player() });
  }
  else
  {
    sscanf(str, "of %s", str);
    ov = wiz_present(str, this_player());
  }

  if (!sizeof(ov))
    return 0;

  for (i = 0; i < sizeof(ov); i++)
  {
    if (!ov[i])
      continue;

    if ( (interactive(ov[i])) && !(ov[i]->query_coder()) &&
       !(this_object()->query_admin()) && !(ov[i] == this_object()))
    {
      log_file("inv", this_object()->query_cap_name()+" tried to view the inventory of interactive object: "+
                      ov[i]->query_cap_name()+" ["+ctime(time(), 4)+"]\n");
      write("You are not allowed to view the inventory of "+ov[i]->query_cap_name()+".\n");
      continue;
    }

    write("Inventory of " + desc_object(ov[i]) + ":\n");
      // " en " + desc_object(environment(ov[i])) + ":\n");

    obj = first_inventory(ov[i]);

    while (obj)
    {
      write("  " + desc_f_object(obj) + "\n");
      obj = next_inventory(obj);
    }
  }

  return 1;
} /* inv() */

/* Free form parse_args code */
/*
protected int parse_frogs(string str) {
  mixed junk;

  if (this_player(1) != this_object()) return 0;
  // We are not as such looking for an end thingy of any sort...
  junk = parse_args(str, ";");
  // It has already printed an error, so we return 1...
  if (!junk) return 1;
  log_file("exec",(string)this_player(1)->query_cap_name()+" exec'd "+str+"\n");
  write("The line "+str+" Returns: \n");
  printf("%O\n", junk[0]);
  return 1;
} */
/* parse_forgs() */

/*
int show_stats(string str) {
  object *ob;
  mixed *ob1;
  string s,bing;
  int i,j;

  bing = "";
  ob = wiz_present(str, this_object());
  if (!sizeof(ob)) {
  write("No existe tal objeto.\n");
  return 1;
  }
  for (j=0;j<sizeof(ob);j++) {
  // if (interactive(ob[j]) && !(this_player()->query_admin() || this_player()->query_thane()) && !(this_player() == obj(j))
  // {
  //  log_file("LOCATE",this_player()->query_cap_name()+" tried to Stat "+ob[j]->query_cap_name()+".\n");
  //  write("You are not allowed to stat "+ob[j]->query_cap_name()+".\n");
  // continue;
  // }

  ob1 =ob[j]->stats();
  if (!pointerp(ob1))
  continue;
  s = "";
  for (i=0;i<(sizeof(ob1)-1);i++)
  if (ob1[i])
  if (ob1[i][0])
  s += ob1[i][0] + ": "+ob1[i][1]+"\n";

  bing += sprintf("%-*#s\n\n\n", this_object()->query_cols(), s);
  }
  this_object()->more_string(bing);
  return 1;
} show_stats() */


/*
int upgrade_player() {
  object ob;

  ob = clone_object("/secure/upgrade");
  ob->new_player(this_object());
  return 1;
} upgrade_player() */


int trans(string str)
{
  object *obs;
  int i;

  if (!strlen(str) || !(sizeof(obs = wiz_present(str, this_player()))))
  {
    write("¿Transportar a quién?\n");
    return 1;
  }

  for (i = 0; i < sizeof(obs); i++)
  {
    if (environment(obs[i]) == environment(this_player()))
    {
      write(desc_object(obs[i])+" ya está aquí.\n");
      continue;
    }

    /*
    if (interactive(obs[i]) &&
       !(this_player()->query_thane() || this_player()->query_admin()) &&
       !(obs[i]->query_property("test_char"))
    {
      log_file("BUSTED",this_player()->query_cap_name()+" tried to illegally trans "+obs[i]->query_cap_name()+".\n");
      write("Sorry, You are not powerful enough to trans "+obs[i]->query_cap_name()+".  Ask a higher ranking immortal.\n");
      continue;
    }
    */

    if (environment(obs[i]) && (!obs[i]->query_coder()))
      log_file("trans", (string)this_player(1)->query_cap_name()+" (trans'ed) "+
        obs[i]->query_cap_name()+" from "+file_name(environment(obs[i]))+
        " to "+file_name(environment(this_player()))+" ["+ctime(time(),4)+"]\n");

    tell_object(obs[i], "\nEres transferido mágicamente a algún lugar.\n\n");
    write("Transfieres a " + obs[i]->query_cap_name() + " hasta aquí.\n");
    obs[i]->move_living("X", environment(this_player()));
  }

  return 1;
} /* trans() */

int whereis(string str)
{
  object *ov,e;
  int i;

  notify_fail("¿Dónde esta el qué?\n");
  ov = wiz_present(str, this_player());

  if (!sizeof(ov))
    return 0;

  for (i = 0; i < sizeof(ov); i++)
  {
    if (ov[i]->query_invis() > 1)
      continue;

    if (interactive(ov[i]))
      if (!(this_player()->query_admin() || this_player()->query_thane()))
      {
        log_file("whereis", this_player()->query_cap_name()+" attempted to locate interactive: " +
                            ov[i]->query_cap_name()+" ["+ctime(time(),4)+"]\n");
        write("Lo siento, no te está permitido localizar jugadores.\n");
        continue;
      }

    write(desc_object(ov[i]) + " está: \n");
    e = ov[i];
    while (e = environment(e))
      write("  en " + desc_f_object(e) + "\n");
  }

  return 1;
} /* whereis() */

int get_pathof(string str)
{
  object *ov;
  int i;

  notify_fail("¿Path de qué?\n");
  ov = wiz_present(str,this_player());

  if (!sizeof(ov))
    return 0;

  for (i = 0; i < sizeof(ov); i++)
  {
    if (!ov[i])
      continue;
    /*
    if (sizeof(ov) > 1) {
    */
    write("Path de " + desc_object(ov[i]) + " en " +
          desc_object(environment(ov[i])) + ":\n");
    /*
    }
    */
    write(file_name(ov[i])+ "\n");
  }

  return 1;
} /* get_pathof() */

/* added by Dank Mar 3 '93.  also added query_prev() to move.c */
int goback(string str)
{
  object ob;

  if (!(ob = this_player()->query_prev()))
    write("La localización anterior no es válida (quizá ya no está cargada en memoria).\n");
  else
  {
    this_player()->set_no_prompt();
    this_player()->move_living("X", ob);
  }

  return 1;
}

int do_find(string str)
{
  string func, thing, s, ping;
  object *obs, fish;
  int i;

  notify_fail("Sintaxis: find funcion() <objeto(s)>\n");

  if (!strlen(str))
    return 0;

  if (sscanf(str, "%s() %s", func, thing) != 2)
    if (sscanf(str, "%s %s", func, thing) != 2)
      return 0;

  obs = wiz_present(thing, this_player());

  if (!sizeof(obs))
  {
    notify_fail("No se encontró '" + thing + "'.\n");
    return 0;
  }

  s = "";

  for (i = 0; i < sizeof(obs); i++)
  {
    if (ping = function_exists(func, obs[i]))
      s += "*** " + desc_object(obs[i])+": "+func+"() encontrada en " + ping + " ***\n";
    else
      s += "*** " + desc_object(obs[i]) + ": " + func + "() no encontrada ***\n";

    fish = obs[i];

    while(fish = shadow(fish, 0))
      if (function_exists(func, fish))
        s += "      Shadowed por " + file_name(fish) + "\n";
  }

  write(s);
  return 1;
} /* do_find() */

int do_debug(string str)
{
  object *obs;

  if (!strlen(str))
  {
    notify_fail("Sintaxis: " + query_verb() + " <objeto>\n");
    return 0;
  }

  if (!sizeof(obs = wiz_present(str, this_player())))
  {
    notify_fail("Objeto " + str + " no encontrado.\n");
    return 0;
  }

  write(debug_info((query_verb() == "dump"?0:1), obs[0]));
  return 1;
} /* do_debug() */

int find_shadows(string s)
{
  object *objs, *shadows;
  object nobj;
  int i, j;

  if (!strlen(s))
    s = "me";

  objs = wiz_present(s, this_player());

  if (sizeof(objs) == 0)
  {
    notify_fail("No puedo encontrar el objeto.\n");
    return 0;
  }

  for (i = 0; i < sizeof(objs); i++)
  {
    shadows = objs[i]->_query_shadows();

    // shadows = ({ });
    // nobj = objs[i];

    // while (nobj = shadow(nobj, 0))
    //   shadows += ({ nobj });

    if (!sizeof(shadows))
    {
      write(desc_f_object(objs[i]) + " no esta siendo shadowed.\n");
    }
    else
    {
      write(desc_f_object(objs[i]) + " esta siendo shadowed por:\n");

      for (j = 0; j < sizeof(shadows); j++)
        write("    " + file_name(shadows[j]) + "\n");
    }
  }

  return 1;
} /* find_shadows() */

mixed stats()
{
  return ({
          });
}

