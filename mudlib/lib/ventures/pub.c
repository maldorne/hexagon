/*
 * Old pub review for CcMud, neverbot 7/03
 * Added attendable by Dimmak@Rl (and updated), neverbot 4/7/03
 * Remove moving prices, neverbot 05/2009
 *
 * Changes for Hexagon, neverbot 01/2025
 * - pub functions refactored in several files, to be inherited both
 *   from the pub room (/lib/ventures/pub.c) and the pub component
 *   for locations (/lib/location/components/pub.c). 
 *   
 */

inherit room       "/lib/room.c";
inherit attendable "/lib/ventures/attendable.c";
inherit sign       "/lib/ventures/pub-sign.c";
inherit menu       "/lib/ventures/pub-menu.c";
inherit actions    "/lib/ventures/pub-actions.c";
inherit food     "/lib/ventures/food.c";
// inherit prices     "/lib/ventures/prices.c";

#include <living/food.h>
#include <basic/money.h>
// #include <timestuff.h>
// #include <money_adjust.h>
#include <living/consents.h>
#include <language.h>

int last_adjust, pts_healed;

// Taniwha 1995 , block clean_up() from removing items
int clean_up(int flag) { return 1; }
int clean_up_room(int flag) {  return 1; }

int query_pub(){ return 1; }

void create()
{
  attendable::create();
  menu::create();
  room::create();

  create_sign();

  // include the pub in the ventures handler
  if (base_name(this_object()) != "/lib/ventures/pub")
    handler("ventures")->include_pub(base_name(this_object()));
}

void init() 
{
  room::init();
  actions::init();
}

void event_fight_in_progress(object who, object * fighters) 
{
  mixed * dest;
  object ob;
  int i;

  dest = query_dest_dir();

  // we call the event_pub_brawl in all the rooms around
  // useful if we want to organize a good fight
  for (i = 1; i < sizeof(dest); i += 2)
    if (objectp(dest[i]))
      event(dest[i], "pub_brawl", this_object());
    else
      if (ob = find_object(dest[i]))
        event(ob,"pub_brawl", this_object());
}

mixed * stats()
{
  return room::stats() +
    attendable::stats() +
    menu::stats();
}
