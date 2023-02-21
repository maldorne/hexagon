/*
    /lib/object.c
    Previous hacks by Dank and Pinkfish
    Baldrick cleaned after them.
    Hamlet fixed timed properties.
    Radix made create_me more accurate - 7/96
    neverbot, cleaning, 4/03
*/

#include <basic/id.h>
#include <kernel.h>
#include <mud/secure.h>

inherit id         "/lib/core/basic/id";
inherit property   "/lib/core/basic/property";
inherit misc       "/lib/core/basic/misc";
inherit extra_look "/lib/core/basic/extra_look";
inherit desc       "/lib/core/basic/desc";
inherit events     "/lib/core/basic/events";
inherit gender     "/lib/core/basic/gender";

string create_me;

void create()
{
  create_me = DEF_NAME;
  id::create();
  property::create();
  misc::create();
  extra_look::create();
  desc::create();
  gender::create(); // neverbot
  set_gender(0); // default in objects/items

  // done in the auto object
  // seteuid(SECURE->creator_file(file_name(this_object())));
  // Radix, July 1996
  create_me = SECURE->get_create_me(create_me);
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

/*
 * This returns a mapping of all the data that is associated with the object.
 * The mapping can then be saved in the player object, is the idea.
 * However for many derived objects we don't need all the info, eg
 * a domain object will set all the name/short/plurals etc itself;
 * so we need not save all that info for a derived object, and so don't
 * return it at all if derived is nonzero (ie it is a derived object).
 */
mapping query_auto_load_attributes() 
{
  // return 
  //   ([ 
  //     "weight" : weight, 
  //     "light" : light, 
  //     "short" : short_d, 
  //     "long" : long_d, 
  //     "alias" : alias, 
  //     "plural" : plurals, 
  //     "name" : name, 
  //     "main plural" : plural_d, 
  //     "value" : value,
  //     "properties" : map_prop,
  //     "timed"     : freeze_timed_properties(timed_prop), /* Hamlet */
  //     "cloned by" : create_me,
  //   ]);
  return
    ((create_me != DEF_NAME) ? ([ "cloned by"  : create_me ]) : ([ ])) + 
    ((map_sizeof(map_prop)) ? ([ "properties" : map_prop ]) : ([ ])) + 
    ((map_sizeof(timed_prop)) ? ([ "timed" : freeze_timed_properties(timed_prop) ]) : ([ ]));
}

/*
 * In loading, we need not discriminate between derived/underived objects;
 * either the mapping entries are there are they aren't.
 */

void init_auto_load_attributes(mapping attribute_map)
{
  // if (!undefinedp(attribute_map["weight"]))
  //   set_weight(attribute_map["weight"]);
  // if (!undefinedp(attribute_map["light"]))
  //   set_light(attribute_map["light"]);
  // if (!undefinedp(attribute_map["short"]))
  //   set_short(attribute_map["short"]);
  // if (!undefinedp(attribute_map["long"]))
  //   set_long(attribute_map["long"]);
  // if (attribute_map["alias"])
  //   set_aliases(attribute_map["alias"]);
  // if (attribute_map["plural"])
  //   set_plurals(attribute_map["plural"]);
  // if (!undefinedp(attribute_map["name"]))
  //   set_name(attribute_map["name"]);
  // if (!undefinedp(attribute_map["main plural"]))
  //   set_main_plural(attribute_map["main plural"]);
  // if (!undefinedp(attribute_map["value"]))
  //   set_value(attribute_map["value"]);

  if (attribute_map["properties"])
    map_prop = attribute_map["properties"];
  if (attribute_map["timed"])
    timed_prop = thaw_timed_properties(attribute_map["timed"]);
  if (attribute_map["cloned by"])
    create_me = attribute_map["cloned by"];
}

mixed * stats()
{
  return id::stats() +
         property::stats() +
         misc::stats() +
         extra_look::stats() +
         gender::stats() +
         desc::stats() +
            ({
              ({ "Cloned By", create_me, }),
            });
}

