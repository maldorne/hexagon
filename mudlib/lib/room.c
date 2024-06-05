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
inherit desc     "/lib/core/basic/desc";
inherit events   "/lib/core/basic/events";

inherit contents   "/lib/room/contents";
inherit exits      "/lib/room/exits";
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


static mixed * room_clones;
static mapping items;

static string room_zone,
              dark_mess;
              // * dig_where,
              // * dig_exit,

object * destables;

static string loginroom;
static string quit_destination;

string query_quit_destination() { return quit_destination; }
void set_quit_destination(string str) { quit_destination = str; }

int query_room() { return 1; }

void set_dark_mess(string str) { dark_mess = str; }
void start_clean_up();

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
  room_clones = ({ });

  // dig_where = ({ });
  // dig_exit = ({ });

  items = ([ ]);
  destables = ({ });
  room_zone = "nowhere";
  set_dark_mess(_LANG_ROOM_TOO_DARK);

  // seteuid(SECURE->creator_file(file_name(this_object())));
  // seteuid(ROOM_EUID);

  light::create();
  property::create();
  contents::create();
  desc::create();
  events::create();

  exits::create();
  contents::create();
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
          if (flags == 1) contents::add_hidden_object(room_clones[i]);
        }
      }
    }
  }
  guard::reset();
}

void add_clone(string the_file, int how_many, varargs int flags)
{
  if (!how_many)
    return;

  if (!stringp(the_file))
  {
    log_file("room.log", "(add_clone) bad clone file: " + the_file
    + ", " + file_name(this_object())
    + " [" + ctime(time()) + "]\n");
    return;
  }

  // don't make array unless we have something to put in it!
  if (!room_clones)
    room_clones = ({ });

  while (how_many-- > 0)
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
  
  switch(lvl)
  {
    default:
      return _LANG_ROOM_LIGHT_DEF;
    case 1: /* Total blackout */
      return dark_mess;
    case 2: /* pretty damn dark */
      return _LANG_ROOM_LIGHT_2;
    case 3: /* getting dim */
      return _LANG_ROOM_LIGHT_3;
    case 4: /* slightly dazzled */
      return _LANG_ROOM_LIGHT_4;
    case 5: /* very bright */
      return _LANG_ROOM_LIGHT_5;
    case 6:
      return _LANG_ROOM_LIGHT_6;
  }
}

string long(string str, int dark)
{
  if (dark)
    return query_dark_mess(dark) + "\n";

  // commented by neverbot 6/03
  // is slower, but we need exit_string to be updated, because now
  // we can see open/closed doors in the long description
  // if (!exit_string)
  exit_string = query_dirs_string();

  if (!strlen(str))
    return wrap(::long(str, dark), 
               (this_user() ? this_user()->query_cols() : 80), 1) + 
                exit_string + "\n" + query_contents("");

  str = expand_alias(str);
  return items[str];
}

void init()
{
  exits::init();
  // add_action("do_dig", "cavar");

  contents::init();
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
  // if (!sizeof(hidden_objects))
  //  return (object *)all_inventory(this_object()) + m_values(items);

  return (object *)all_inventory(this_object()) + 
         (object *)contents::query_hidden_objects() + 
         m_values(items);
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

mixed stats()
{
  return ({
    ({ "Move Zone", room_zone }),
          }) + 
      exits::stats() +
      property::stats() +
      light::stats() +
      desc::stats() +
      guard::stats() +
      navigation::stats() +
      diplomacy::stats();
}
