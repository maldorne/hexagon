/*
    /std/object.c 
    Previous hacks by Dank and Pinkfish
    Baldrick cleaned after them.
    Hamlet fixed timed properties.
    Radix made create_me more accurate - 7/96
    neverbot, cleaning, 4/03
*/

#include <basic/id.h>
#include <kernel.h>

inherit id         "/lib/core/basic/id";
inherit property   "/lib/core/basic/property";
inherit misc       "/lib/core/basic/misc";
inherit extra_look "/lib/core/basic/extra_look";
inherit desc       "/lib/core/basic/desc";
inherit actions    "/lib/core/basic/actions";
inherit events     "/lib/core/basic/events";

string create_me;
 
void create() 
{
  create_me = DEF_NAME;
  id::create();
  property::create();
  misc::create(); 
  extra_look::create();
  desc::create();
  actions::create();

  seteuid(load_object(SECURE)->creator_file(file_name(this_object())));
  // Radix, July 1996
  create_me = load_object(SECURE)->get_create_me(create_me);
  this_object()->setup();
}

// moved here, be sure every object has an init function somewhere
// neverbot 8/2008
void init() 
{
  ::init();	
} 

nomask string query_create_me() { return create_me; }

void set_name(string str) 
{
  if (name && name != DEF_NAME) 
      return;
  name = str;
  if (!short_d)
      short_d = str;
}

int query_weapon() { return 0; }
int query_armour() { return 0; }
int query_shield() { return 0; }
int query_room() { return 0; }

/*
mixed *query_init_data() {
   return
      property::query_init_data() +
      extra_look::query_init_data() +
      desc::query_init_data() +
      // misc::query_init_data() +
      id::query_init_data();
} 
*/

// This is here till I can delete it...  ie the new system is functioning.
mapping int_query_static_auto_load() 
{
  return ([ 
            "weight" : weight, "light" : light, 
            "short" : short_d, 
            "long" : long_d, 
            "alias" : alias, 
            "plural" : plurals, "name" : name, 
            "main plural" : plural_d, 
            "value" : value 
          ]);
} 

mixed query_static_auto_load() 
{
  if (file_name(this_object())[0..10] == "/std/object")
    return int_query_static_auto_load();
  return ([ ]);
} 

mapping query_dynamic_auto_load() 
{
    mapping res;
    res = ([ ]);
    
    if (m_sizeof(map_prop))
        res += ([ "properties" : map_prop, ]);
    if (m_sizeof(timed_prop))
        res += ([ "timed" : freeze_timed_properties(timed_prop), ]);
    if ((create_me != DEF_NAME) && (create_me != capitalize(DEF_NAME)))
        res += ([ "cloned by" : create_me, ]);
      
    return res;
} 

void init_static_arg(mapping bing) 
{
  if (!undefinedp(bing["name"]))
    set_name(bing["name"]);
  if (!undefinedp(bing["weight"]))
    set_weight(bing["weight"]);
  if (!undefinedp(bing["light"]))
    set_light(bing["light"]);
  if (!undefinedp(bing["short"]))
    set_short(bing["short"]);
  if (!undefinedp(bing["long"]))
    set_long(bing["long"]);
  if (!undefinedp(bing["alias"]))
    set_aliases(bing["alias"]);
  if (!undefinedp(bing["plural"]))
    set_plurals(bing["plural"]);
  if (!undefinedp(bing["main plural"]))
    set_main_plural(bing["main plural"]);
  if (!undefinedp(bing["value"]))
    set_value(bing["value"]);
} 

void init_dynamic_arg(mapping args) 
{
    if (args["properties"])
        map_prop = args["properties"];
    if (args["timed"])
        timed_prop = thaw_timed_properties(args["timed"]);

    if (args["cloned by"])
        create_me = args["cloned by"];
    else
        create_me = DEF_NAME;
} 

void init_arg(mixed *bing) 
{
  if (sizeof(bing) < 10)
    return ;
  set_light(bing[1]);
  short_d = bing[2];
  long_d = bing[3];
  set_aliases(bing[5]);
  set_plurals(bing[6]);
  set_weight(bing[8]);
  set_name(bing[9]);
  map_prop = bing[10];
  value = bing[12];
}

 // The following is from the TMI-2 Lib. Asmodean Put it here
int clean_up()
{
  object env, *contents;
  int i;

  if (userp(this_object())) 
     return 1; /* don't clean_up players */
  env = environment();
  if (env && userp(env)) 
     return 1; /* carried ob */
  if (env && environment(env))
     return (int)environment(env)->clean_up(); /* recurse */
  
  contents = deep_inventory(this_object());
  if (contents) {
     for (i=0;i<sizeof(contents);i++)
           if (userp(contents[i])) 
              return 1; /* holding a user */
  }
  if (!env) {
  /* we're a room with no users inside or a lost object */
      dest_me();
      return 1;
  }
  return 1; /* try again later... */
}

mixed * stats() 
{
  return id::stats() +
         property::stats() +
         misc::stats() +
         extra_look::stats() +
         desc::stats() +
         actions::stats() +
            ({
              ({ "Cloned By", create_me, }),
            });
}

