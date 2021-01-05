// External exit_handler for room.c...  should make it less "fat"
//      room.c references these functions below [Piper 12/24/95]
// Some changes, neverbot 6/03
// nearest_exits added, 04/2009
// npc guards protecting exits added, neverbot 15/04/2009

#include <language.h>
#include <room/room.h>
#include <basic/material.h>
#include <common/properties.h>
#include <translations/exits.h>

mapping shorten;
mapping nearest_exits;
mapping broad_nearest_exits;

#define NEAREST 

#define BROAD_NEAREST 

void create() 
{
  shorten = ([
    DIR_NORTH:DIR_NORTH_SHORTEN, DIR_SOUTH:DIR_SOUTH_SHORTEN, DIR_WEST:DIR_WEST_SHORTEN, 
    DIR_EAST:DIR_EAST_SHORTEN, DIR_NORTHEAST:DIR_NORTHEAST_SHORTEN,
    DIR_NORTHWEST:DIR_NORTHWEST_SHORTEN, DIR_SOUTHEAST:DIR_SOUTHEAST_SHORTEN, 
    DIR_SOUTHWEST:DIR_SOUTHWEST_SHORTEN, DIR_UP:DIR_UP_SHORTEN,
    DIR_DOWN:DIR_DOWN_SHORTEN, DIR_IN:DIR_IN_SHORTEN, 
    DIR_OUT:DIR_OUT_SHORTEN, DIR_STAIRS:DIR_STAIRS_SHORTEN,
  ]);
  nearest_exits = ([
    DIR_NORTH:({DIR_NORTHEAST,DIR_NORTHWEST,}), DIR_SOUTH:({DIR_SOUTHWEST,DIR_SOUTHEAST,}),
    DIR_EAST:({DIR_NORTHEAST,DIR_SOUTHEAST,}), DIR_WEST:({DIR_NORTHWEST,DIR_SOUTHWEST,}),
    DIR_NORTHEAST:({DIR_NORTH,DIR_EAST,}), DIR_NORTHWEST:({DIR_WEST,DIR_NORTH,}),
    DIR_SOUTHEAST:({DIR_SOUTH,DIR_EAST,}), DIR_SOUTHWEST:({DIR_WEST,DIR_SOUTH,}),
  ]);
  broad_nearest_exits = ([
    DIR_NORTH:({DIR_NORTHEAST,DIR_NORTHWEST,DIR_WEST,DIR_EAST,}),
    DIR_SOUTH:({DIR_SOUTHWEST,DIR_SOUTHEAST,DIR_EAST,DIR_WEST,}),
    DIR_EAST:({DIR_NORTHEAST,DIR_SOUTHEAST,DIR_SOUTH,DIR_NORTH,}),
    DIR_WEST:({DIR_NORTHWEST,DIR_SOUTHWEST,DIR_SOUTH,DIR_NORTH,}),
    DIR_NORTHEAST:({DIR_NORTH,DIR_EAST,DIR_SOUTHEAST,DIR_NORTHWEST,}),
    DIR_NORTHWEST:({DIR_WEST,DIR_NORTH,DIR_NORTHEAST,DIR_SOUTHWEST,}),
    DIR_SOUTHEAST:({DIR_SOUTH,DIR_EAST,DIR_NORTHEAST,DIR_SOUTHWEST,}),
    DIR_SOUTHWEST:({DIR_WEST,DIR_SOUTH,DIR_SOUTHEAST,DIR_NORTHWEST,}),
  ]);
}

string query_shorten(string dir)
{
  if (undefinedp(shorten[dir]))
    return "" + dir;
  return "" + shorten[dir];
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
  
  if (sscanf(str,"%s %s",s1,s2) == 2)  
    return s1;
  return str;
}  /* expand_direc() */

mixed* add_exit(mapping door_control, mapping exit_map,
                varargs mixed *dest_other, string *dest_direc, object *hidden_objects, 
                string direc, mixed dest, string type, string material)
{
  mixed *stuff;
  // string exit_string, 
  // short_exit_string;

  // tell_object(find_living("neverbot"), "Direccion: "+direc+" Tipo: "+type+"\n");

  if (!material) {
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

  if (!exit_map) 
    exit_map = ([ ]);
  
  exit_map[direc] = ({ dest, type, material });
  
  if (member_array(direc, dest_other) != -1)
      return ({ });

  // In stuff we store:
  // ({ dest (destination room) + 
  //    ({ array with data about the exit type }) +
  //    back direction (north <-> south)
  //  })
  stuff = ({ dest }) + ROOM_HAND->query_exit_type(type, direc);

  dest_other += ({ direc, stuff });
  dest_direc += ({ expand_direc(direc) });
  // exit_string = 0;
  // short_exit_string = 0;
  if ((stuff = (mixed)ROOM_HAND->query_door_type(type, direc, dest)))
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
  if ((i = member_array(direc, dest_other)) == -1)
          return ({ });
 
  for (j = 0; j < sizeof(data); j += 2)
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
  return ({ door_control, dest_other, hidden_objects });
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

  // mounts cannot use exits with types rope nor door
  if (ob->query_ride() && 
      (room_ob->query_ex_type(dest_direc[i]) == "rope" || 
       room_ob->query_ex_type(dest_direc[i]) == "door" ) )
      return 0;

  door = room_ob->query_door_ob(dest_direc[i]);

  if (door && !door->is_open()) 
  {    
    string fail_msg;
    fail_msg = "";

    // door customization, neverbot 10/03
    if (door->query_reset_message())
      notify_fail(_LANG_EXITS_IS_CLOSED);
    else if (door->query_known_exit(dest_direc[i]))
      notify_fail(_LANG_EXITS_IS_CLOSED_KNOWN);
    else       
      notify_fail(_LANG_EXITS_IS_CLOSED_CUSTOM);
    
    return 0;
  }

  // check if the door ghas guards
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
        // if guard is present and is protecting that exit
        if (list[k] && (list[k+1] == dest_direc[i]) &&
            (environment(list[k]) == room_ob))
        {  
          // npc does not allow
          if (!list[k]->guardian_check(ob) )
          {
            if (ob->query_timed_property(file_name(list[k]) + "_guard_lock") )
            {
                notify_fail(list[k]->query_short() + " ya se ha dado cuenta de lo que intentas.\n");
                return 0;
            }
            
            // random guard perception agains character dexterity
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
      
      // if we avoid _one_ guard, do not check other guards
      // so we don't see 'you sneak' followed by a 'you have been caught'
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

  // visited places, neverbot 08/2010
  if (result)
  {
    // prototype:
    // void add_visited(string file_name, string direction, int is_stealth, 
    //        int is_mounted, string mount_name)
    ob->add_visited(
      file_name(room_ob), // visited room
      dest_direc[i],      // direction taken
      0,                  // flag was sneaking
      ob->query_riding(), // flag was mountes
      (ob->query_riding()?ob->query_mount()->query_name():"") // name of the used mount
      );
  }

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

  for (i=0; i < size; i+=2)
  {
    // same look as with the short_exit_string, neverbot 6/03
    door = room_ob->query_door_ob(dest_other[i]);
    if (door) {
      if (door->is_open()) 
         dirs+= ({ "-" + dest_other[i] + "-" });
      else 
         dirs+= ({ "(" + dest_other[i] + ")" });
    }
    else {
      add = 0;
      if (dest_other[i+1][ROOM_OBV]) {
        no += 1;
        add = 1;
      } else if (stringp(dest_other[i+1][ROOM_OBV])) {
        nostore = 1;
        add = (int)call_other(this_object(), dest_other[i+1][ROOM_OBV]);
      } else if (pointerp(dest_other[i+1][ROOM_OBV])) {
        nostore = 1;
        add = (int)call_other(dest_other[i+1][ROOM_OBV][0], dest_other[i+1][ROOM_OBV][1]);
      }
      if (!add)
        continue;
      dirs += ({ dest_other[i] });
    }
  }

  if (sizeof(dirs) == 0) {
    if (nostore)
      return exit_color + _LANG_EXITS_CANNOT_SEE_EXITS + "%^RESET%^";
    exit_string = exit_color + _LANG_EXITS_CANNOT_SEE_EXITS + "%^RESET%^";
    return exit_string;
  }

  if (sizeof(dirs) == 1) {
    if (nostore)
      return exit_color + _LANG_EXITS_ONE_EXIT + dirs[0] + "%^RESET%^";
    exit_string = exit_color + _LANG_EXITS_ONE_EXIT + dirs[0] + "%^RESET%^";
 
    return exit_string;
  }

  ret = _LANG_EXITS_AND + dirs[sizeof(dirs)-1] + "";
  dirs = delete(dirs,sizeof(dirs)-1, 1);
  if (nostore)
    return exit_color + _LANG_EXITS_MULTIPLE_EXITS + implode(dirs,", ") + ret + "%^RESET%^";
  exit_string = exit_color + _LANG_EXITS_MULTIPLE_EXITS + implode(dirs,", ") + ret + "%^RESET%^";

  return exit_string;
} /* query_dirs_string() */

string query_reduced_exit_name(string str){
  if (shorten[str])
    return shorten[str];
  else
    return str;
}
