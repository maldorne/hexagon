/*
* Taniwha 1995 uncommented clean_up
* Baldrick started some lobotomizing. sept '96
* Will reduce the size of this beast
*
* CcMud revision, neverbot 6/03
*
* For hexagon, big refactor extracting from here exits, zone, contents, cleanup,
*   to files in /lib/room, neverbot 06/24
*/

#include <room/room.h>
#include <basic/light.h>
#include <mud/secure.h>
#include <areas/common.h>
#include <common/frames.h>
#include <language.h>

inherit obj      "/lib/core/object.c";
inherit light    "/lib/core/basic/light.c";

inherit cleanup    "/lib/room/cleanup.c";
inherit contents   "/lib/room/contents.c";
inherit exits      "/lib/room/exits.c";
inherit zone       "/lib/room/zone.c";
inherit senses     "/lib/room/senses.c";
inherit guard      "/lib/room/guard.c";
inherit navigation "/lib/room/navigation.c";
inherit diplomacy  "/lib/room/diplomacy.c";
inherit sign       "/lib/room/sign.c";
inherit dark       "/lib/room/dark.c";

static mixed * room_clones;
static mapping items;

object * destables;

static string loginroom;
static string quit_destination;

string query_quit_destination() { return quit_destination; }
void set_quit_destination(string str) { quit_destination = str; }

nomask int query_room() { return 1; }
nomask int query_location() { return 0; }

int id(string str)
{
  return 0;
  // str = expand_alias(str);
  // return items[str];
}

void create()
{
  room_clones = ({ });

  items = ([ ]);
  destables = ({ });

  // seteuid(SECURE->creator_file(file_name(this_object())));
  // seteuid(ROOM_EUID);

  light::create();
  dark::create();
  contents::create();

  exits::create();
  contents::create();
  zone::create();
  senses::create();
  navigation::create();
  diplomacy::create();
  guard::create();

  obj::create();

  // default light value for every room, will be changed
  // in the setup() if needed
  set_light(BASE_ROOM_LIGHT_VALUE);

  add_property("location", "inside");

  reset();
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
          if (flags == 1) 
            contents::add_hidden_object(room_clones[i]);
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

mixed * query_room_clones() { return room_clones; }

string short(varargs int dark)
{
  if (dark)
    return query_dark_mess(dark);
  else
    return ::short();
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

  contents::init();
  senses::init();

  cleanup::init();
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
void add_destable(object ob) { destables += ({ ob }); }

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
  arr -= ({ nil });

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

object * find_inv_match(string str)
{
  // if (!sizeof(hidden_objects))
  //  return (object *)all_inventory(this_object()) + m_values(items);

  return (object *)all_inventory(this_object()) + 
         (object *)contents::query_hidden_objects() + 
         m_values(items);
}

// to avoid conflict, this function is inherited both from object and cleanup
int clean_up(varargs int flag)
{
  return cleanup::clean_up(flag);
}

mixed * stats()
{
  return zone::stats() +
      exits::stats() +
      light::stats() +
      dark::stats() +
      guard::stats() +
      navigation::stats() +
      diplomacy::stats();
}
