
#include <room/room.h>
#include <language.h>

static string exit_string,
              short_exit_string,
              exit_color,
              * aliases,
              * dest_direc;
              // * dig_where,
              // * dig_exit,

static mixed * dest_other;

static mapping exit_map,
               door_locks,
               door_control;

// function prototypes
object query_door_ob(string dir);
object add_door(string dir);


void create()
{
  dest_direc = ({ });
  dest_other = ({ });
  aliases = ({ });

  door_locks = ([ ]);
  door_control = ([ ]);

  exit_string = "";
  short_exit_string = "";  

  exit_map = ([ ]);

  // dig_where = ({ });
  // dig_exit = ({ });

  exit_color = "%^BOLD%^%^CYAN%^";
}

void init()
{
  int i, j;
  mapping done;

  if (!dest_direc)
    return;

  done = ([ ]);
  for (i = 0; i < sizeof(dest_direc); i++)
  {
    if (!done[dest_direc[i]])
      add_action("do_exit_command", dest_direc[i]);

    done[dest_direc[i]] = 1;

    if ((j = member_array(dest_direc[i], aliases)) != -1)
    {
      string *al;
      al = aliases;

      do
      {
        if (!(j%2))
        {
          if (!done[al[j+1]])
          {
            add_action("do_exit_command", al[j+1]);
            done[al[j+1]] = 1;
          }
        }
        else
            j++;

        al = al[j+2..sizeof(al)];

      } while ((j=member_array(dest_direc[i], al)) != -1);
    }
  }

  // add_action("do_dig", "cavar");
}

string *query_direc() { return dest_direc; }
mapping query_exit_map() { return exit_map; }
string * query_aliases() { return aliases; }

// It IS useful to get directions + where they go to
// BTW this is not a frog.
nomask mixed *query_dest_dir()
{
  int i;
  string *retval;

  retval = ({ });
  for (i = 0; i < sizeof(dest_other); i += 2)
    retval += ({ dest_other[i], dest_other[i+1][ROOM_DEST] });

  return  retval;
}

mixed *query_dest_other() { return dest_other; }

string query_where_dir(string direc)
{
  int i;
  for (i = 0; i < sizeof(dest_other); i += 2)
    if (dest_other[i] == direc)
      return (string)dest_other[i+1][ROOM_DEST];
  return "";
}

string query_dir_where(string where)
{
  int i;
  for (i = 0; i < sizeof(dest_other); i += 2)
    if (dest_other[i+1][ROOM_DEST] == where)
      return (string)dest_other[i];
  return "";
}

string expand_alias(string str)
{
  str = EXIT_HAND->expand_alias(aliases, str);
  return str;
}

//  Thanks to viewers like you this function [insert name here]
//     has been moved to your local exit_handler [Piper 1/5/96]
string query_dirs_string()
{
  object room_ob;

  room_ob = this_object();
  exit_string = EXIT_HAND->query_dirs_string(dest_direc,
  dest_other, room_ob, exit_color);
  return exit_string;
}

void reset_short_exit_string() { short_exit_string = ""; }

// This creates the exits message you keep getting
string query_short_exit_string()
{
  string ret, *dirs;
  int no, i, nostore, add;
  object door;

  if (strlen(short_exit_string))
    return short_exit_string;

  if (!dest_direc || sizeof(dest_direc) == 0)
    dest_direc = ({ });

  dirs = ({ });

  for (i = 0; i < sizeof(dest_other); i += 2)
  {
    door = query_door_ob(dest_other[i]);

    if (door)
    {
      if (door->is_open())
      {
        if (ret = EXIT_HAND->query_shorten(dest_other[i]))
          dirs += ({ "-"+ret+"-" });
        else
          dirs += ({ "-"+dest_other[i]+"-" });
      }
      else
      {
        if (ret = EXIT_HAND->query_shorten(dest_other[i]))
          dirs += ({ "("+ret+")" });
        else
          dirs += ({ "("+dest_other[i]+")" });
      }
    }
    else
    {
      add = 0;
      if (dest_other[i+1][ROOM_OBV])
      {
        no += 1;
        add = 1;
      }
      else if (stringp(dest_other[i+1][ROOM_OBV]))
      {
        nostore = 1;
        add = (int)call_other(this_object(), dest_other[i+1][ROOM_OBV]);
      }
      else if (pointerp(dest_other[i+1][ROOM_OBV]))
      {
        nostore = 1;
        add = (int)call_other(dest_other[i+1][ROOM_OBV][0], dest_other[i+1][ROOM_OBV][1]);
      }

      if (!add)
        continue;

      if (ret = EXIT_HAND->query_shorten(dest_other[i]))
        dirs += ({ ret });
      else
        dirs += ({ dest_other[i] });
    }
  }

  if (sizeof(dirs) == 0)
  {
    if (nostore)
      return exit_color + " [" + _LANG_NO_EXITS + "]%^RESET%^";

    return short_exit_string = exit_color + " [" + _LANG_NO_EXITS + "]%^RESET%^";
  }

  if (nostore)
    return exit_color + " [" + implode(dirs, ",") + "]%^RESET%^";

  return short_exit_string = exit_color + " [" + implode(dirs, ",") + "]%^RESET%^";
}

void set_exit_color(string which)
{
  exit_color = ROOM_HAND->exit_string_color(which);
}

//  Externalized exit portions of room.c (Piper 12/24/95)
//      Now look in exit_handler.c
mixed add_exit(string direc, mixed dest, string type,
               varargs string material)
{
  mixed *m;
  object door;

  if (!arrayp(dest_other) || !sizeof(dest_other)) 
    dest_other = ({ });

  m = EXIT_HAND->add_exit(door_control, exit_map,                  // mappings
    dest_other, dest_direc, this_object()->query_hidden_objects(), // arrays
    direc, dest, type, material);                                  // & data

  short_exit_string = "";
  exit_string = "";

  if (sizeof(m) > 0)
  {
    door_control = m[0];
    exit_map = m[1];
    dest_other = m[2];
    dest_direc = m[3];
    this_object()->set_hidden_objects(m[4]);
    exit_string = query_dirs_string();
    short_exit_string = query_short_exit_string();

    // new door system for ccmud, just with calling add_exit
    // with door exit type, the door object will be added
    if ((type == "door") || (type == "gate"))
    {
      door = add_door(direc);
      return door;
    }

    return 1;
  }
  // the only case the exit handler returns ({ }) is 
  // because the exit already existed 
  else
  {
    // return the door object if it already existed
    if (door = query_door_ob(direc))
      return door;
  }

  return 0;
}

// Query for exit type... [Piper 12/24/95]
string query_ex_type(string direc)
{
  if (!exit_map[direc])
    return nil;
  return exit_map[direc][1];
}

int query_special_exit(string direc)
{
  int i;
  for (i = 0; i < sizeof(dest_other); i+=2)
    if (dest_other[i] == direc)
      if (dest_other[i+1][ROOM_FUNC])
        return 1;

  if (query_ex_type(direc) == "door" || query_ex_type(direc) == "gate")
    return 2;

  return 0;
}

// Query for the exit material [Piper 12/24/95]
string query_ex_material(string direc)
{
  if (!exit_map[direc])
    return nil;

  return exit_map[direc][2];
}

// modify_exit... "now showing in an exit_handler near you"
//  [Piper 12/30/95]
int modify_exit(string direc, mixed *data)
{
  mixed *m;

  m = EXIT_HAND->modify_exit(door_control, dest_other, 
                             this_object()->query_hidden_objects(),
                             direc, data);

  if (sizeof(m) > 0)
  {
    if (sizeof(m) > 0 )
    {
      door_control = m[0];
      dest_other = m[1];
      this_object()->set_hidden_objects(m[2]);
      exit_string = query_dirs_string();
      return(1);
    }
  }
  return 0;
}

int remove_exit(string direc)
{
  mixed *m;
  object door;

  // Radix
  // is an idiot.  Checked door_map.  Doormap contains nothing.
  // Fixed by Wonderflug.
  if ( member_array(direc, dest_other) == -1 )
    return 0;

  m = EXIT_HAND->remove_exit(door_control, exit_map, dest_other,
                             dest_direc, this_object()->query_hidden_objects(), 
                             direc);

  // destroy the door object if it the exit is removed
  // neverbot 6/03
  door = query_door_ob(direc);
  if (door)
    door->dest_me();

  if (sizeof(m))
  {
    door_control = m[0];
    exit_map = m[1];
    dest_other = m[2];
    dest_direc = m[3];
    this_object()->set_hidden_objects(m[4]);
  }

  exit_string = query_dirs_string(); // Update the exit string
  reset_short_exit_string();  //so glance works, Anirudh

  return 1;
}

int query_exit(string direc)
{
  return (member_array(direc, dest_other) != -1);
}

int query_size(string direc)
{
  int size;
  object room_ob;

  room_ob = this_object();
  size = EXIT_HAND->query_size(dest_other,direc,room_ob);
  return size;
}

int do_exit_command(string str, varargs mixed verb, object ob)
{
  int zip;
  object room_ob;

  room_ob = this_object();

  if (!verb)
    verb = query_verb();

  if (!ob)
    ob = this_player();

  zip = EXIT_HAND->do_exit_command(door_control,
    exit_map, dest_direc, dest_other,
    aliases, str, verb, ob, room_ob);

  // update clean_up handle
  this_object()->update_clean_up();

  // the movement shows no prompt message, the look or glance commands
  // will be queued and one of them will show it
  if (zip)
    this_player()->set_no_prompt();

  return zip;
}


// This is the function to include IF you add_exit with a
// add_action, while other players are in the same room as
// the add_action triggerer...  Piper (9/29/95)
int renew_exits()
{
  int i;
  object * olist;

  olist = all_inventory(this_object());

  for (i = 0; i < sizeof(olist); i++)
  {
    olist[i]->move(this_object());
  }
  exit_string = query_dirs_string();
  reset_short_exit_string();
}

// Here lies the dig stuff... much smaller and little used
// Piper [1/26/96]
/*
int add_dig_exit(string direc, mixed dest){
  mixed *m;

  m = DIG_HAND->add_dig_exit(dig_exit,dig_where,direc,dest);

  dig_exit = m[0];
  dig_where = m[1];

  return(1);
}

string *query_dig_dest() { return dig_where; }

string *query_pos_dig() { return dig_exit; }

int do_dig(string direc){
  object room_ob = this_object();
  DIG_HAND->do_dig(dig_exit,dig_where,room_ob,direc);

  return(1);
}
*/

// new functions for the door system, based in an original idea from Iolo@Rl
object add_door(string dir)
{
  object door;
  string opposite;

  if (!query_exit(dir))
    return nil;

  door = clone_object(DOOR_OBJECT);
  door->move(this_object());

  door->set_dest(dir);
  opposite = ROOM_HAND->query_opposite(dir);

  if (opposite != "")
    door->set_dir_other_side(opposite);

  exit_string = query_dirs_string();
  reset_short_exit_string();

  return door;
}

object query_door_ob(string dir)
{
  object *aux;
  int i;

  aux = all_inventory(this_object());

  for (i = 0; i < sizeof(aux); i++)
    if (aux[i]->query_door() && aux[i]->query_dest() == dir)
      return (aux[i]);

  return nil;
}

mapping query_door_control() { return door_control; }

mixed query_door(mixed dest)
{
  mixed bing;

  if ((bing = door_control[dest]))
    if (!pointerp(bing))
      return bing;

  if (objectp(dest))
    if ((bing = door_control[file_name(dest)]))
      if (!pointerp(bing))
        return bing;

  return 0;
}

mixed stats()
{
  mixed * exits;
  int i;

  exits = ({ });
  for (i = 0; i < sizeof(dest_other); i += 2)
    exits += ({ ({ "Direction", dest_other[i], }),
                ({ "Destination", dest_other[i+1][ROOM_DEST] }) });

  return ({
    ({ "Location (property)", this_object()->query_property("location"), }),
          }) +
      exits;
}
