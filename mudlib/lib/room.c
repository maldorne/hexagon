/*
* Taniwha 1995 uncommented clean_up
* Baldrick started some lobotomizing. sept '96
* Will reduce the size of this beast.
* CcMud revision, neverbot 6/03
*/

#include <room/room.h>
#include <basic/light.h>
#include <mud/secure.h>
#include <areas/common.h>
#include <common/frames.h>
#include <language.h>

inherit light    "/lib/core/basic/light";
inherit property "/lib/core/basic/property";
inherit contents "/lib/core/basic/contents";
inherit desc     "/lib/core/basic/desc";
inherit events   "/lib/core/basic/events";

inherit senses     "/lib/room/senses";
inherit guard      "/lib/room/guard";
inherit navigation "/lib/room/navigation";
inherit diplomacy  "/lib/room/diplomacy";

// debugging logs
//#define CLEAN_UP_LOG "clean_up_room"
//#define DOOR_LOG "room_doors.cloned"
#define FAST_CLEAN_UP 10
#define SLOW_CLEAN_UP 480

private static int room_create_time;    // time of creation
private static int room_init_time;      // time of previous init
private static int room_stabilize;      // don't bother call_out
private static int clean_up_handle;

object *query_hidden_objects();
string query_dirs_string();

object query_door_ob(string dir);
object add_door(string dir);

static mixed *room_clones;
static mapping items,
               exit_map,
               door_locks,
               door_control;

static string exit_string,
              short_exit_string,
              room_zone,
              dark_mess,
              // * dig_where,
              // * dig_exit,
              * dest_direc,
              * aliases;

static mixed * dest_other;
object * destables, * hidden_objects;

static string exit_color;
static string loginroom;
static string quit_destination;

string query_quit_destination() { return quit_destination; }
void set_quit_destination(string str) { quit_destination = str; }

string query_dir_where(string where);
string query_where_dir(string direc);
int modify_door(string direc, mixed *data);

int query_room() { return 1; }
void set_dark_mess(string str) { dark_mess = str; }
void start_clean_up();
string query_short_exit_string();
object * query_hidden_objects() { return hidden_objects; }

// moved glance code to the command (: Radix 1996
string short(varargs int dark)
{
  return ::short(dark);
}

int id(string str)
{
  return 0;
  // str = expand_alias(str);
  // return items[str];
}

void create()
{
  string *inh;
  dest_other = ({ });
  dest_direc = ({ });
  door_locks = ([ ]);
  room_clones = ({ });

  // dig_where = ({ });
  // dig_exit = ({ });

  exit_map = ([ ]);
  items = ([ ]);
  aliases = ({ });
  destables = ({ });
  hidden_objects = ({ });
  door_control = ([ ]);
  room_zone = "nowhere";
  exit_color = "%^BOLD%^%^CYAN%^";
  set_dark_mess("Está demasiado oscuro para poder ver");

  exit_string = "";
  short_exit_string = "";

  // seteuid(SECURE->creator_file(file_name(this_object())));
  // seteuid(ROOM_EUID);

  light::create();
  property::create();
  contents::create();
  desc::create();
  events::create();

  property::create();
  senses::create();
  guard::create();
  navigation::create();
  diplomacy::create();

  // default light value for every room, will be changed
  // in the setup() if needed
  set_light(BASE_ROOM_LIGHT_VALUE);

  add_property("location", "inside");
  this_object()->setup();
  reset();

  // if (replaceable(this_object()))
  // {
  //   inh = inherit_list(this_object());
  //   if (sizeof(inh) == 1)
    room_create_time = time();
    clean_up_handle = 0;
    start_clean_up();
  //   replace_program(inh[0]);
  // }
}

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

string set_login_room(string room)
{
  loginroom = room;
  return loginroom;
}

void event_login(object ob, varargs mixed avoid)
{
  ::event_login(ob, avoid);

  if (stringp(loginroom) && ob)
    ob->move(loginroom);
}

int test_add(object ob, int flag) { return 1; }
int test_remove(object ob, int flag) { return 1; }
int add_weight(int n) { return 1; }

object *add_hidden_object(object ob)
{
  hidden_objects += ({ob});
  return hidden_objects;
}

string query_contents(varargs string str, object *ob)
{
  if (!hidden_objects)
    hidden_objects = ({ });
  if (ob)
    ob -= hidden_objects;
  else
    ob = all_inventory(this_object()) - hidden_objects;
  return ::query_contents(str, ob);
}

void reset()
{
  int flags;

  if ( room_clones ) // From Laggard RoD
  {
    string thing;
    int i;

    i = sizeof(room_clones);
    // processing backwards yields room inventory in order of add_clones.
    // (for those who care)
    flags = 0;
    while ( i-- > 0 )
    {
      // See if it's a tag, if so we use it for now
      if ( stringp(room_clones[i]) )
      {
        thing = room_clones[i];
        flags = 0;
      }
      else if (room_clones[i])
      {
        flags = room_clones[i];
      }
      else if ( !room_clones[i] ) // fill up any empties using the tag
      {
        // Taniwha, clone, then TEST then move, so if it's screwed
        //  it still works
        room_clones[i] = clone_object(thing);
        if (room_clones[i])
        {
          room_clones[i]->move(this_object());
          if (flags == 1) add_hidden_object(room_clones[i]);
        }
      }
    }
  }
  guard::reset();
}

void set_exit_color(string which)
{
  exit_color = ROOM_HAND->exit_string_color(which);
}

void add_clone(string the_file, int how_many, varargs int flags)
{
  // if ( !how_many ) how_many = 1;
  if ( !how_many )
    return;

  if ( !stringp(the_file) )
  {
    log_file("room.log", "(add_clone) bad clone file: " + the_file
    + ", " + file_name(this_object())
    + " [" + ctime(time()) + "]\n");
    return;
  }

  // don't make array unless we have something to put in it!
  if ( !room_clones )
    room_clones = ({ });

  while ( how_many-- > 0 )
  {
    // make space for objects in array
    room_clones += ({ nil });
  }
  
  // last, for backward processing
  if (flags)
    room_clones += ({ flags });

  room_clones += ({ the_file });
}

mixed *query_room_clones() { return room_clones; }

string query_dark_mess(int lvl)
{
  if (!exit_string)
      exit_string = query_dirs_string();
  switch(lvl)    {
    default:
        return "Hmm, no ves bien. " + dark_mess + "\n";
    case 1: /* Total blackout */
        return dark_mess;
    case 2: /* pretty damn dark */
        return "No puedes ver mucho.\n"+ exit_string;
    case 3: /* getting dim */
        return "Es demasiado difícil ver en esta oscuridad.\n"+
        // ::short(1)+"\n" + // neverbot
        exit_string;
    case 4: /* slightly dazzled */
        return "Estás deslumbrado por la luz.\n" +
        // ::short(0)+"\n" + // neverbot
        exit_string;
    case 5: /* very bright */
        return "La luz es realmente cegadora.\n"+ exit_string;
    case 6:
        return "Estás cegado por la luz.\n";
  }
}

string expand_alias(string str)
{
  str = EXIT_HAND->expand_alias(aliases,str);
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
      return exit_color+" [ninguna]%^RESET%^";

    return short_exit_string = exit_color + " [ninguna]%^RESET%^";
  }

  if (nostore)
    return exit_color + " [" + implode(dirs, ",") + "]%^RESET%^";

  return short_exit_string = exit_color + " [" + implode(dirs, ",") + "]%^RESET%^";
}

string long(string str, int dark)
{
  if (dark)
    return "   "+query_dark_mess(dark)+"\n";

  // commented by neverbot 6/03
  // is slower, but we need exit_string to be updated, because now
  // we can see open/closed doors in the long description
  // if (!exit_string)
  exit_string = query_dirs_string();

  if (!strlen(str))
  {
    return(
           sprintf("\n   %-=*s\n", (this_user()?this_user()->query_cols()-3:76),
                   "   "+ ::long(str, dark)) +
           exit_string + "\n" +
           query_contents("")
        );
  }

  str = expand_alias(str);
  return items[str];
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

  hidden_objects -= ({ nil });

  for (i = 0; i < sizeof(hidden_objects); i++)
    hidden_objects[i]->init();

  senses::init();

  start_clean_up();
}

void start_clean_up()
{
  int old_call_out;
#ifdef FAST_CLEAN_UP
  // when folk in room, update timers {Laggard}
  old_call_out = remove_call_out( clean_up_handle );
#endif
  room_init_time = time();
#ifdef FAST_CLEAN_UP
  if ( old_call_out < FAST_CLEAN_UP
  &&  (room_init_time - room_create_time) < FAST_CLEAN_UP )
  {
    // was merely passing through {Laggard}
    clean_up_handle = call_out( "clean_up_room", FAST_CLEAN_UP, 0 );
  }
  else if ( !room_stabilize )
  {
    clean_up_handle = call_out( "clean_up_room", (old_call_out < 0 ? 0 : old_call_out), 0 );
  }
#endif
}

// not particularly useful.
// But here just the same, great for debugging purposes.
string *query_direc() { return dest_direc; }
mapping query_exit_map() { return exit_map; } // neverbot, debugging
string * query_aliases() { return aliases; } // neverbot, debugging

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

void set_zone(string str) { room_zone = str; }

string query_zone()
{
#ifdef FAST_CLEAN_UP
  // monsters call this to move, but may not actually come here,
  // so potential clean_up_room {Laggard}
  if ( !room_stabilize  &&  !room_init_time )
  {
    room_init_time = time();
    clean_up_handle = call_out( "clean_up_room", FAST_CLEAN_UP, 0 );
  }
#endif

  return room_zone;
}

// this function puts the directions into the thingy list
// I am sure you know what I mean
string expand_direc(string str)
{
  string s1,s2;

  if (sscanf(str,"%s %s",s1,s2) == 2)
    return s1;
  return str;
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

  m = EXIT_HAND->add_exit(door_control, exit_map, // mappings
    dest_other, dest_direc, hidden_objects,       // arrays
    direc, dest, type, material);                 // & data

  short_exit_string = "";
  exit_string = "";

  if (sizeof(m) > 0)
  {
    door_control = m[0];
    exit_map = m[1];
    dest_other = m[2];
    dest_direc = m[3];
    hidden_objects = m[4];
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
  for(i = 0; i < sizeof(dest_other); i+=2)
    if (dest_other[i]==direc)
      if (dest_other[i+1][ROOM_FUNC])
        return 1;

  if (query_ex_type(direc)=="door" || query_ex_type(direc)=="gate")
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

  m = EXIT_HAND->modify_exit(door_control,dest_other,hidden_objects,
                direc,data);

  if (sizeof(m) > 0)
  {
    if (sizeof(m) > 0 )
    {
      door_control = m[0];
      dest_other = m[1];
      hidden_objects = m[2];
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

  m = EXIT_HAND->remove_exit(door_control,exit_map,dest_other,
      dest_direc,hidden_objects,direc);

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
    hidden_objects = m[4];
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
  int zip, old_call_out;
  object room_ob;

  room_ob = this_object();

  if (!verb)
    verb = query_verb();

  if (!ob)
    ob = this_player();

  zip = EXIT_HAND->do_exit_command(door_control,
    exit_map, dest_direc, dest_other,
    aliases, str, verb, ob, room_ob);

#ifdef FAST_CLEAN_UP
  old_call_out = remove_call_out( clean_up_handle );  // multiple folks in room

  if ( old_call_out > 0 &&
       old_call_out < FAST_CLEAN_UP &&
      (time() - room_create_time) < FAST_CLEAN_UP )
  {
    // was merely passing through {Laggard}
    clean_up_handle = call_out( "clean_up_room", FAST_CLEAN_UP, 0 );
  }
  else if ( !room_stabilize )
  {
    clean_up_handle = call_out( "clean_up_room", SLOW_CLEAN_UP, 0 );
  }
#endif

  // the movement shows no prompt message, the look or glance commands
  // will be queued and one of them will show it
  if (zip)
    this_player()->set_no_prompt();

  return zip;
}

// Here is the add_item stuff, what I don't understand is why this is objects..
// could it help if we used just a mapping? not too sure tho.
// Baldrick.

// Ok we have done all the exit junk, now for the item bits and pieces
// share and enjoy your plastic pal who is fun to be with

int add_item(mixed id, string desc)
{
  object ob;

  if (!items)
  items = ([ ]);

  ob = items["The object"];
  if (!ob)
  {
    // Object must call seteuid() prior to calling clone_object()
    // seteuid("tmp");
    ob = clone_object(ITEM_OBJECT);
    items["The object"] = ob;
  }

  // Flag final para evitar el pluralize automatico, neverbot 05/09
  ob->setup_item(id, desc, 1);
  return 1;
}

// Ok.  Remove item is nasty :(  But....  We will add it.
// It will remove all things that reference the given desc.
int remove_item(string str)
{
  object ob;

  if (!items)
    items = ([ ]);

  ob = items["The object"];
  if (!ob)
    return 0;
  return (int)ob->remove_item(str);
}

// Change all descs...  It does a match and goes wimble.
int modify_item(string str, string new_desc)
{
  object ob;

  if (!items)
    items = ([ ]);

  ob = items["The object"];
  if (!ob)
    return 0;
  return (int)ob->modify_item(str, new_desc);
}

mapping query_items() { return items; }

// The alias junk
int add_alias(mixed name,string str)
{
  int i;

  if (!aliases)
    aliases = ({ });

  if (pointerp(name)) {
    for (i = 0; i < sizeof(name); i++)
      add_alias(name[i], str);
    return 1;
  }

  aliases += ({ str, name });
  return 1;
}

int modify_alias(string str,string name)
{
  int i;

  if (!aliases)
    return 0;

  if ((i=member_array(str, aliases))==-1 || !(i%2))
    return 0;

  aliases[i+1] = name;
  return 1;
}

int remove_alias(string str)
{
  int i;

  if (!aliases)
    return 0;

  if ((i=member_array(str, aliases))==-1 || !(i%2))
    return 0;

  aliases = delete(aliases, i, 2);
  return 1;
}

int query_no_writing() { return 1; }

object * query_destables() { return destables; }
static int empty_room(object ob)
{
  object *olist;
  int i;

  if (!environment(ob))
    return 1;

  // TEMP fix I think, - Radix
  if (!environment(ob)->query_property("location"))
    return 0;

  olist = all_inventory(environment(ob));

  for( i = 0; i < sizeof(olist) ; i++)
  {
    if (interactive(olist[i]))
      return 0; // Player in room
  }

  return 1; // dest it
}

void dest_me()
{
  object *arr;
  int i;

  if (!destables)
      destables = ({ });
  if (mappingp(items))
      destables += m_values(items);
  for (i = 0; i < sizeof(destables); i++)
      if (destables[i])
          destables[i]->dest_me();

  // Isthar@Aurora 10-dec-1994, destruct everything here
  arr = all_inventory(this_object());
  arr -= ({ 0});

  for (i = 0; i < sizeof(arr); i++)
  {
    if (interactive(arr[i]))
        arr[i]->move(ROOM_VOID);
    else
        arr[i]->dest_me();
  }

  // Wandering NPC hunt, Taniwha 1996
  for( i = 0; i < sizeof(room_clones); i++)
  {
    if (objectp(room_clones[i]))
      if (empty_room(room_clones[i]))
          room_clones[i]->dest_me();
  }

  guard::dest_me();
  navigation::dest_me();
  diplomacy::dest_me();

  destruct(this_object());
}

// function called by the driver before swapping.
int clean_up( int flag )
{
  object * arr;
  int i, elapsed_time;

  arr = deep_inventory( this_object() );
  i = sizeof( arr );
  elapsed_time = time() - room_create_time;

  if (this_object()->query_property(NO_CLEAN_UP_PROP))
    return 1;
  if (this_object()->query_property("corpse_here"))
    return 1;

  // check for inherited room
  if ( flag )
  {
#ifdef CLEAN_UP_LOG
    log_file( CLEAN_UP_LOG, ctime( time() )
      + " inherited "
      + (room_init_time ? time() - room_init_time : "*0*")
      + "/"
      + elapsed_time
      + " seconds ("
      + memory_info( this_object() )
      + " bytes) "
      + file_name( this_object() )
      + ".\n" );
#endif
    return 0;
  }

  // Loop to find if user inside the room somehow {Begosh}
  // Also check for longer term room usage {Laggard}
  while( i-- )
  {
    if ( userp( arr[i] )
    ||  elapsed_time > SLOW_CLEAN_UP )
    {
#ifdef CLEAN_UP_LOG
      log_file( CLEAN_UP_LOG, ctime( time() )
        + " stabilized "
        + (room_init_time ? time() - room_init_time : "*0*")
        + "/"
        + elapsed_time
        + " seconds ("
        + memory_info( this_object() )
        + " bytes) "
        + file_name( this_object() )
        + ".\n" );
#endif
      // room is frequently used, stop cleaning up
      room_stabilize = 1;
      return 1;
    }
  }

#ifdef CLEAN_UP_LOG
  log_file( CLEAN_UP_LOG, ctime( time() )
    + " clean_up "
    + (room_init_time ? time() - room_init_time : "*0*")
    + "/"
    + elapsed_time
    + " seconds ("
    + memory_info( this_object() )
    + " bytes) "
    + file_name( this_object() )
    + ".\n" );
#endif

  dest_me();
  return 0; // don't call back
}

#ifdef FAST_CLEAN_UP
// use call_out to attempt faster clean up {Laggard}
// this is almost but not quite the same as the preceeding function.
// the differences are not subtle.
int clean_up_room( int flag )
{
  object * arr;
  int i, elapsed_time;

  if (this_object()->query_property(NO_CLEAN_UP_PROP))
    return 1;
  if (this_object()->query_property("corpse_here"))
    return 1;

  stderr(" ~ clean_up_room check " + object_name(this_object()) + "\n");

  elapsed_time = time() - room_init_time;
  arr = deep_inventory( this_object() );
  i = sizeof( arr );

  if (room_stabilize)
    return 0;

  remove_call_out( clean_up_handle );

  // Loop to find if user inside the room somehow {Begosh}
  // Also check for recent living (monster?) arrival {Laggard}
  while( i-- )
  {
    if ( userp( arr[i] ) || (arr[i]->query_property(NO_CLEAN_UP_PROP))
    ||  (living( arr[i] ) && elapsed_time < SLOW_CLEAN_UP) )
    {
      // we do a call_out to kill the room later if we can ;)
      clean_up_handle = call_out( "clean_up_room", SLOW_CLEAN_UP, 0 );
      return 1;
    }
  }

#ifdef CLEAN_UP_LOG
  log_file( CLEAN_UP_LOG, ctime( time() )
    + " "
    + elapsed_time
    + "/"
    + (time() - room_create_time)
    + " seconds ("
    + memory_info( this_object() )
    + " bytes) "
    + file_name( this_object() )
    + ".\n" );
#endif

  stderr(" ~ clean_up_room " + object_name(this_object()) + " (done)\n");

  dest_me();
  return 0; // don't call back
}
#endif

object * find_inv_match(string str)
{
  if (!sizeof(hidden_objects))
    return (object *)all_inventory(this_object()) + m_values(items);

  return (object *)all_inventory(this_object()) + (object *)hidden_objects + m_values(items);
}

/* 
 * add_sign(string long, string read_mess, 
 *          string lang, string frame_style, string short, string name)
 *   [lang, frame_style, short and name are optional]
 * This function will return a sign that can be used by a room in any way it sees fit.
 * This function was the brainchild of Wyrm - 7 Feb '92
 * Added language, neverbot 01/2021
 */
object add_sign(string long, string mess, 
  varargs string lang, string frame_style, string short, string sname)
{
  object sign;

  sign = clone_object("/lib/item.c");

  if (!sname)
    sname = _LANG_DEFAULT_SIGN_NAME;

  sign->set_name(sname);
  sign->set_main_plural(pluralize(sname));

  if (!short)
    short = capitalize(_LANG_DEFAULT_SIGN_NAME);
  
  if (!frame_style)
    frame_style = DEFAULT_FRAME_STYLE;

  sign->set_short(short);
  sign->set_long(long);
  sign->reset_get();
  //if (short && short != "")
  sign->move(this_object());
  // hack, set messages after moving, so the item is IN a game
  // and knows what language handler to use
  sign->set_read_mess(mess, lang, 0, frame_style);
  //else
  //  hidden_objects += ({ sign });
  destables += ({ sign });
  return sign;
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
  mixed *exits;
  int i;

  exits = ({ });
  for (i = 0; i < sizeof(dest_other); i += 2)
    exits += ({ ({ "Direction", dest_other[i], }),
                ({ "Destination", dest_other[i+1][ROOM_DEST] }) });

  return ({
    ({ "Location (property)", query_property("location"), }),
    ({ "Move Zone", room_zone }),
          }) + exits +
      property::stats() +
      light::stats() +
      desc::stats() +
      guard::stats() +
      navigation::stats() +
      diplomacy::stats();
}

int query_decay() { return 10; }

// Number of move points used by an attack...
int attack_speed() { return 15; }

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
