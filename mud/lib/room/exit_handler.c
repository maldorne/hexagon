// External exit_handler for room.c...  should make it less "fat"
//      room.c references these functions below [Piper 12/24/95]
// Algunos retoques, neverbot 6/03
// Añadidas nearest_exits, 04/2009
// Sistema de guardias protectores de salidas, neverbot 15/04/2009

#include <room/room.h>
#include <basic/material.h>
#include <common/properties.h>

mapping shorten;
mapping nearest_exits;
mapping broad_nearest_exits;

void create() 
{
  shorten = SHORTEN;
  nearest_exits = NEAREST;
  broad_nearest_exits = BROAD_NEAREST;
}

string * query_nearest_directions(string dir)
{
    if (undefinedp(nearest_exits[dir]))
        return ({ });
    return ({ }) + nearest_exits[dir];
}

string * query_broad_nearest_directions(string dir)
{
    if (undefinedp(broad_nearest_exits[dir]))
        return ({ });
    return ({ }) + broad_nearest_exits[dir];
}

string expand_alias(mixed *aliases,string str)
{
  int i;
 
  if (!aliases)
    return str;
 
  if ((i=member_array(str,aliases))==-1)
    return str;
 
  if (i%2)
    return aliases[i-1];
  return str;
} /* expand_alias() */

string expand_direc(string str)
{
  string s1,s2;
  
  if(sscanf(str,"%s %s",s1,s2) == 2)  return s1;
  return str;
}  /* expand_direc() */

mixed* add_exit(mapping door_control, mapping exit_map,
                varargs mixed *dest_other, string *dest_direc, object *hidden_objects, 
                string direc, mixed dest, string type, string material)
{
  mixed *stuff;
  //  string exit_string, 
  //  short_exit_string;

  // tell_object(find_living("neverbot"), "Direccion: "+direc+" Tipo: "+type+"\n");

  if(!material) {
   switch(type) {
    case "gate" :
      material = S_METAL;
      break;
    case "door" :
      material = S_MADERA;
      break;
    default :
      material = DESCONOCIDO;
      break; 
   }
  }

  if(!exit_map) 
    exit_map = ([ ]);
  
  exit_map[direc] = ({ dest, type, material });
  
  if(member_array(direc, dest_other) != -1)
      return ({ });

  // En stuff almacenamos:
  // ({ dest (room de destino) + 
  //    ({ array con las estadisticas del tipo de salida }) +
  //    direccion contraria (norte ->sur)
  //  })
  stuff = ({dest}) + ROOM_HAND->query_exit_type(type, direc);

  dest_other += ({ direc, stuff });
  dest_direc += ({ expand_direc(direc) });
  // exit_string = 0;
  // short_exit_string = 0;
  if((stuff = (mixed)ROOM_HAND->query_door_type(type, direc, dest)))
  {
    //  door_control[direc] = ({ clone_object(DOOR_OBJECT) });
    //  door_control[direc][0]->setup_door(direc,this_object(),dest,stuff);
    //  hidden_objects += ({ door_control[direc][0] });
    door_control[direc] = dest;
  }

  return ({ door_control, exit_map, dest_other, dest_direc, hidden_objects });
}

/*
mixed* modify_exit(mapping door_control, mapping doors_in,
                mixed *dest_other, mixed *hidden_objects,
                string direc, mixed *data)
*/
mixed* modify_exit(mapping door_control,
                mixed *dest_other, mixed *hidden_objects,
                string direc, mixed *data)
{
  int i, j;
  if ((i=member_array(direc, dest_other)) == -1)
          return ({ });
 
  for (j=0;j<sizeof(data);j+=2)
    switch (lower_case(data[j])) {
      case "message" :
        dest_other[i+1][ROOM_MESS] = data[j+1];
        break;
      case "obvious" :
        dest_other[i+1][ROOM_OBV] = data[j+1];
        break;
      case "function" :
        dest_other[i+1][ROOM_FUNC] = data[j+1];
        break;
      case "size" :
        dest_other[i+1][ROOM_SIZE] = data[j+1];
        break;
      case "enter" :
        dest_other[i+1][ROOM_ENTER] = data[j+1];
        break;
      case "dest" :
        dest_other[i+1][ROOM_DEST] = data[j+1];
        break;
//    case "open" :
//      if (door_control[direc])
//        door_control[direc][0]->set_open(data[j+1]);
//      break;
//    case "key" :
//      if (door_control[direc])
//        door_control[direc][0]->set_key_prop(data[j+1]);
//      break;
//    case "other" :
//      if (door_control[direc])
//        door_control[direc][0]->set_other_id(data[j+1]);
//      break;
//    case "open desc" :
//      if (door_control[direc])
//        door_control[direc][0]->set_open_desc(data[j+1]);
//      break;
//    case "close desc" :
//      if (door_control[direc])
//        door_control[direc][0]->set_close_desc(data[j+1]);
//      break;
//    case "undoor" :
//      if (door_control[direc]) {
//        door_control[direc][0]->go_away();
//        hidden_objects = hidden_objects - ({ door_control[direc][0] });
//      }
//      door_control = m_delete(door_control, direc);
//      break;
    }
 
  // return ({ door_control,doors_in,dest_other,hidden_objects });
  return ({ door_control,dest_other,hidden_objects });
} /* modify_exit() */

// remove_exit (check "undoor" above.. this is easier... go figure) [Piper]

mixed* remove_exit(mapping door_control,
       mapping exit_map, mixed *dest_other, mixed *dest_direc,
       mixed *hidden_objects, string direc)
{
  int i;
  if (!dest_other)
    dest_other = ({ });
  i = member_array(direc, dest_other);
  if (i == -1)
    return ({ });
  
  dest_other = delete(dest_other, i, 2);
  dest_direc = delete(dest_direc, i/2, 1);

  /*
  if (door_control[direc]) {
    door_control[direc][0]->go_away();
    hidden_objects = hidden_objects - ({ door_control[direc][0] });
  }
  */

  door_control = m_delete(door_control, direc);    
  exit_map = m_delete(exit_map, direc);

  return ({ door_control, exit_map, dest_other,
            dest_direc, hidden_objects });
} /* remove_exit() */

mixed *query_size(mixed *dest_other, string direc, object room_ob)
{
  int i;
 
  if ((i=member_array(direc, dest_other))==-1)
    return ({ });
  if (stringp(dest_other[i+1][ROOM_SIZE]))
  {   
    i = call_other(room_ob, dest_other[i+1][ROOM_SIZE]); 
    return ({ });
  }
  if (pointerp(dest_other[i+1][ROOM_SIZE]))
  {
    i = call_other(dest_other[i+1][ROOM_SIZE][0],
                      dest_other[i+1][ROOM_SIZE][1]);
    return ({ });
  }
  return dest_other[i+1][ROOM_SIZE];
} /* query_size() */

/*
int do_exit_command(mapping door_control, mapping door_locks,
                    mapping exit_map, mixed *dest_direc,
                    mixed *dest_other, mixed *aliases,
                    string str, mixed verb, object ob,
                    object foll, object room_ob)
*/

int do_exit_command(mapping door_control,
                    mapping exit_map, mixed *dest_direc,
                    mixed *dest_other, mixed *aliases,
                    string str, mixed verb, object ob,
                    object room_ob)
{
  string special_mess;
  int i, result;
  mixed zip;
  object door;
  mixed * guards;
 
  if (!verb)
    verb = query_verb();
  else 
  {
    if (pointerp(verb)) 
    {
       special_mess = verb[1];
       verb = verb[0];
    }
    if (!sscanf(verb, "%s %s", verb, str) !=2)
       str = "";
  }
  if (!ob)
    ob = this_player();

  // cannot move in passed out
  if (ob->query_property(PASSED_OUT_PROP))
  {
    // do not give an error message, because we will be giving clues
    // about which are the "right" exits (the ones returning an error)
    // notify_fail(ob->query_timed_property(PASSED_OUT_PROP));
    return 0;
  }

  if ((i = member_array(verb, dest_direc)) == -1)
    if ((i = member_array(verb, aliases)) == -1)
      return 0;
    else
      if ((i = member_array(aliases[i-1], dest_direc)) == -1)
        return 0;

  // ok must be two command exit 
  if (dest_direc[i] != dest_other[i*2]) 
  {
    string s1,s2;
    int j;
 
    sscanf(dest_other[i*2],"%s %s",s1,s2);
    str = expand_alias(aliases,str);

    if (s2 != str) 
    {
      zip = dest_direc[i+1..sizeof(dest_direc)];
      while (1)
        if ((j = member_array(verb, zip)) != -1) 
        {
          i += j+1;
          sscanf(dest_other[i*2],"%s %s", s1, s2);
          if (str == s2)
            break;
          zip = zip[j+1..sizeof(zip)];
        } else
          return 0;
    }
  }

  // Las monturas no pueden pasar por salidas tipo rope ni door
  if (ob->query_ride() && 
      (room_ob->query_ex_type(dest_direc[i]) == "rope" || 
       room_ob->query_ex_type(dest_direc[i]) == "door" ) )
      return 0;

  door = room_ob->query_doors(dest_direc[i]);

  if (door && !door->is_open()) 
  {    
    string fail_msg;
    fail_msg = "";

    // Nuevo sistema de personalizacion de puertas,
    // neverbot 10/03
    if (door->query_reset_message())
      notify_fail("La puerta hacia "+dest_direc[i]+" está cerrada.\n");
    else if (door->query_known_exit(dest_direc[i]))
      notify_fail("La puerta "+dest_direc[i]+" está cerrada.\n");
    else 
    {
      if(door->query_gender() == 1) // male
        fail_msg += (door->query_number())?("Los "):("El ");
      else
        fail_msg += (door->query_number())?("Las "):("La ");
      fail_msg += dest_direc[i] + " está"+((door->query_number())?("n"):(""));
      if(door->query_gender() == 1) // male
        fail_msg += (door->query_number())?(" cerrados.\n"):(" cerrado.\n");
      else
        fail_msg += (door->query_number())?(" cerradas.\n"):(" cerrada.\n");
      notify_fail(fail_msg);
    }
    return 0;
  }

  // Comprobamos si la salida tiene guardias asignados
  if (sizeof(guards = room_ob->query_guards()) )
  {
    int j, k;
    mixed * list;

    for (j = 0; j < sizeof(guards); j++)
    {
      int sneak;
      sneak = 0;
      list = guards[j];
      for (k = 0; k < sizeof(list); k+=2)
      {
        // Si esta el guardia y esta protegiendo la salida escogida
        if (list[k] && (list[k+1] == dest_direc[i]) &&
            (environment(list[k]) == room_ob))
        {  
          // El npc impide el paso
          if (!list[k]->guardian_check(ob) )
          {
            if (ob->query_timed_property(file_name(list[k]) + "_guard_lock") )
            {
                notify_fail(list[k]->query_short() + " ya se ha dado cuenta de lo que intentas.\n");
                return 0;
            }
            
            // Azar de percepcion del guardia contra destreza del invasor
            if (list[k]->query_level() + list[k]->query_per() + random(list[k]->query_per()) >= 
                ob->query_level() + ob->query_dex() + random(ob->query_dex()))
            { 
              if (list[k]->guardian_message())
                notify_fail(list[k]->guardian_message());
              else
                notify_fail(list[k]->query_short() + " te impide el paso.\n");

              ob->add_timed_property(file_name(list[k])+"_guard_lock", 1, 20);
              return 0;
            }
            else
            {
              tell_object(ob, list[k]->query_short() + " está vigilando esa dirección, " +
                  "pero en un descuido te cuelas sin que se dé cuenta.\n");
              sneak = 1;
              break;
            }
          }
        }
      }
      
      // si nos hemos colado a un guardia, no seguimos comprobando los demas
      // en otro caso se podria ver el mensaje de "te cuelas" seguido del "te descubre"
      if (sneak)
        break;
    }
  }

  if (dest_other[i*2+1][ROOM_FUNC])
    if (stringp(dest_other[i*2+1][ROOM_FUNC])) 
    {
      if (!call_other(room_ob, dest_other[i*2+1][ROOM_FUNC],
          str, ob, special_mess))
        return 0;
    } 
  else 
  {
    if (pointerp(dest_other[i*2+1][ROOM_FUNC]))
      if (!call_other(dest_other[i*2+1][ROOM_FUNC][0],
                      dest_other[i*2+1][ROOM_FUNC][1], ob, special_mess))
        return 0;
  }
 
  if (!special_mess)
    catch( result = (int)ob->move_living(dest_other[i*2], 
                                      dest_other[i*2+1][ROOM_DEST],
                                      dest_other[i*2+1][ROOM_MESS],
                                      dest_other[i*2+1][ROOM_ENTER]));
  else
    catch( result = (int)ob->move_living(dest_other[i*2], 
                                      dest_other[i*2+1][ROOM_DEST],
                                      special_mess,
                                      dest_other[i*2+1][ROOM_ENTER]));

/*
  // visited places, neverbot 08/2010
  if (result)
  {
    // prototipo de la funcion:
    // void add_visited(string file_name, string direction, int is_stealth, 
    //        int is_mounted, string mount_name)
    ob->add_visited(
      file_name(room_ob), // room que ha visitado
      dest_direc[i],      // direccion que ha tomado
      0,                  // flag estaba sigilando
      ob->query_riding(), // flag iba montado
      (ob->query_riding()?ob->query_mount()->query_name():"") // nombre de la montura usada
      );
  }
  */

  return result;
} /* do_exit_command() */

/*
 * Those "Obvious exits:" messages [Piper 1/5/96]
 * Yes folks. This creates that really stupid obvious exits message you keep
 * getting. Hope you really hate it.
 * specialy changed to handle stupid things. If you can figure it out I
 * will shake your hand. Look for docs coming soon to a universe near you.
 */
string query_dirs_string(mixed *dest_direc, mixed *dest_other,
                         object room_ob, string exit_color)
{
  string ret, *dirs;
  string exit_string;
  int no, i, nostore, size, add;
  object door;
 
  if (!dest_direc || sizeof(dest_direc)==0)
    dest_direc = ({ });
  dirs = ({ });
  size = sizeof(dest_other);
  for (i=0; i < size; i+=2){

    // Modificado el sistema para que las salidas se vean igual que
    //  con el short_exit_string, neverbot 6/03

    door = room_ob->query_doors(dest_other[i]);
    if (door) {
      if (door->is_open()) 
         dirs+= ({ "-"+dest_other[i]+"-" });
      else 
         dirs+= ({ "("+dest_other[i]+")" });
    }
    else {
      add = 0;
      if (dest_other[i+1][ROOM_OBV]) {
        no += 1;
        add = 1;
      } else if (stringp(dest_other[i+1][ROOM_OBV])) {
        nostore = 1;
        add = (int)call_other(this_object(),dest_other[i+1][ROOM_OBV]);
      } else if (pointerp(dest_other[i+1][ROOM_OBV])) {
        nostore = 1;
        add = (int)call_other(dest_other[i+1][ROOM_OBV][0],dest_other[i+1][ROOM_OBV][1]);
      }
      if (!add)
        continue;
      dirs += ({ dest_other[i] });
    }
  }

  if (sizeof(dirs)==0) {
    if (nostore)
      // return exit_color+"There are no obvious exits.%^RESET%^";
    // exit_string = exit_color+"There are no obvious exits.%^RESET%^";
      return exit_color+"No puedes ver ninguna salida%^RESET%^";
    exit_string = exit_color+"No puedes ver ninguna salida%^RESET%^";
    return exit_string;
  }
  if (sizeof(dirs)==1) {
    if (nostore)
      // return exit_color+"There is one obvious exit: "+dirs[0]+".%^RESET%^";
    // exit_string = exit_color+"There is one obvious exit: "+dirs[0]+".%^RESET%^";
      return exit_color+"Puedes ver una salida: "+dirs[0]+"%^RESET%^";
    exit_string = exit_color+"Puedes ver una salida: "+dirs[0]+"%^RESET%^";
 
    return exit_string;
  }
  ret = " y "+dirs[sizeof(dirs)-1]+"";
  dirs = delete(dirs,sizeof(dirs)-1, 1);
  if (nostore)
  /*
    return exit_color+"There are "+query_num(sizeof(dirs)+1, 0)+
           " obvious exits : "+implode(dirs,", ")+ret+"%^RESET%^";
  exit_string = exit_color+"There are "+query_num(sizeof(dirs)+1, 0)+
                " obvious exits : "+implode(dirs,", ")+ret+"%^RESET%^";
  */
      return exit_color+"Puedes ver "+query_num(sizeof(dirs)+1, 0)+
           " salidas: "+implode(dirs,", ")+ret+"%^RESET%^";
  exit_string = exit_color+"Puedes ver "+query_num(sizeof(dirs)+1, 0)+
                " salidas: "+implode(dirs,", ")+ret+"%^RESET%^";
  return exit_string;
} /* query_dirs_string() */

string query_reduced_exit_name(string str){
  if (shorten[str])
    return shorten[str];
  else
    return str;
}
