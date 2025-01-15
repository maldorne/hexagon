/*
 * Handle shops in the mud, neverbot 08/08/2008
 *
 * To avoid deep recursions through lists of shops, lists of items and clones,
 * the update_ventures function will update just one shop at a time, and will leave 
 * information in a pending_clones array.
 *
 *  calls to update_ventures will be done from /lib/handlers/cron.c (old timed.c)
 *
 * At the end of update_ventures, five seconds later a call_out will check
 * the pending_clones and will clone every item _for the same shop_. If there are
 * pending_clones remaining, another call_out with 5 seconds, and so on, but always
 * one shop at a time.
 * 
 * The clones will be created from here, and will be passed to the shop with a call 
 * to load_objects_from_handler (see /lib/ventures/shop.c)
 *
 * Modified for Hexagon, made generic to handle every type 
 * of venture if needed, neverbot 10/2021
 * 
 */

#include <mud/config.h>
#include <room/ventures.h>
#include <room/location.h>

inherit "/lib/core/object.c";

#define HOME "/room/weather_room.c"
#define SAVE_FILE "/save/ventures/ventures"

// Shop updates are called every minute, when the list of shops ends, it starts again
// If we have done the complete list in less time, we do not start again
// 60 seconds * 60 minutes * 2 hours = 7200 seconds
#define UPDATE_TIME 7200

// class action
// {
//   string shop;
//   int how_many;
//   string item;
// }

// list of shop file paths
string * shops;
int next_shop;
static mixed * pending_clones;

// list of pub file paths
string * pubs;

string * query_shops_array() { return shops; }
mixed * query_pending_clones() { return pending_clones; }
void reset_pending_clones() { pending_clones = ({ }); }
void reset_stock(string shop);

string * query_pubs_array() { return pubs; }

void create()
{
  shops = ({ });
  pending_clones = ({ });
  next_shop = 0;
  pubs = ({ });

  restore_object(SAVE_FILE, 1);

  ::create();
}

void setup()
{
  // anticloning like obj/shut, neverbot 6/03
  if (file_name(this_object()) != "/lib/handlers/ventures") 
  {
    write("This object cannot be cloned.\n");
    dest_me();
    return;
  }
  
  reset_get();
  set_name("ventures handler");
  set_short("Ventures handler");
  set_long("This is the ventures handler!\n");
  add_alias("handler");
  add_plural("handlers");
  
  // move masked, always move the object to HOME
  move("bing");
}

int move(mixed dest, varargs mixed messin, mixed messout) 
{
  ::move(HOME, messin, messout);
}

void save_handler() 
{ 
  // tell_object(find_living("admin"), "(VENTURES) Salvando controlador...\n");
  save_object(SAVE_FILE, 1); 
}

void dest_me() 
{
  save_object(SAVE_FILE, 1);
  ::dest_me();
}

int include_shop(string path)
{
  if (member_array(path, shops) == -1)
  {
    shops += ({ path });
    save_handler();
  }
  return 1;
}

int include_pub(string path)
{
  if (member_array(path, pubs) == -1)
  {
    pubs += ({ path });
    save_handler();
  }
  return 1;
}

// called from /lib/handlers/cron.c
int update_ventures()
{
  int i; //, j, k;
  int how_many_to_clone, how_many_in_shop;
  mapping permanent_goods;
  object current_shop_ob;
  string current_shop, current_good;
  string * permanent_goods_paths;
  object new_object;
  object * obs;

  i = 0;
  how_many_to_clone = 0;
  how_many_in_shop = 0;
  permanent_goods = ([ ]);
  current_shop_ob = nil;
  current_shop = "";
  current_good = "";
  permanent_goods_paths = ({ });
  new_object = nil;
  obs = ({ });

  debug("ventures", "update_ventures...\n");

  if (!sizeof(shops))
    return 0;

  if (next_shop >= sizeof(shops))
    next_shop = 0;

  current_shop = shops[next_shop];

  // code for locations
  // check if the current_shop is a location (file ends with .o)
  if (current_shop[strlen(current_shop) - 2..strlen(current_shop) - 1] == ".o")
  {
    current_shop_ob = load_object(LOCATION_HANDLER)->load_location(current_shop);

    // shop location does not load, should not happen, just continue
    if (!current_shop_ob)
      debug("ventures", "location object for " + current_shop + " didn't load\n");
    else
    {
      // get its shop component
      current_shop_ob = current_shop_ob->query_component_by_type(LOCATION_COMPONENT_SHOP);

      // shop component missing... what the hell?
      if (!current_shop_ob)
        debug("ventures", "shop component for location " + current_shop + " is missing\n");
    }
  }
  else // is a room, not a location
  {
    string location_file_name;

    catch(current_shop_ob = load_object(current_shop));

    // do this only if we are using the location system, because it will
    // remove rooms from the venture handler, changing them for the locations
    if (current_shop_ob && CONFIG_USE_LOCATIONS)
    {
      // what would be the location if this room would have one?
      location_file_name = load_object(LOCATION_HANDLER)->get_location_file_name_from_room(load_object(current_shop));

      // the save file for the alternative location already exists
      if (file_size(location_file_name) > 0)
      {
        // add the location if not already added
        if (member_array(location_file_name, shops) == -1)
        {
          debug("ventures", "location shop " + location_file_name + " added as alternative to " + current_shop + "\n");
          shops += ({ location_file_name });
        }

        // remove the old room shop
        debug("ventures", "shop " + current_shop + " removed from handler\n");        
        shops -= ({ current_shop });

        current_shop_ob = load_object(LOCATION_HANDLER)->load_location(location_file_name);
      }
    }
  }

  debug("ventures", "updating content of " + current_shop + " " + trim(to_string(current_shop_ob)) + "\n");

  obs = ({ });
  
  if (!current_shop_ob)
  {
    // if it does not load, remove
    shops -= ({ current_shop });
  }
  else
  {
    // create a temporary vault so every operation here
    // uses the same vault without having to recreate it every
    // query_how_many_in_inventory, etc
    current_shop_ob->create_vault();

    permanent_goods = current_shop_ob->query_permanent_goods();
    permanent_goods_paths = keys(permanent_goods);
    
    // every item to add in this shop
    for (i = 0; i < sizeof(permanent_goods_paths); i++)
    {
      current_good = permanent_goods_paths[i];
    
      debug("ventures", "checking " + current_good + "\n");

      // check if something is wrong with the item
      if (intp(permanent_goods[current_good]) && 
         (new_object = load_object(current_good)))
      {
        // new_object = clone_object(current_good);

        how_many_in_shop = current_shop_ob->query_how_many_in_inventory(new_object->query_main_plural());
        debug("ventures", "we have " + how_many_in_shop + " " + current_good + "\n");

        // if the max amount is returned, the shop is busy, we can't do anything
        if (how_many_in_shop >= MAX_AMOUNT_OF_THINGS)
        {
          // new_object->dest_me();
          break;
        }

        // how many needed (must have - already have)
        how_many_to_clone = (int)permanent_goods[current_good] - how_many_in_shop;

        // update a list of pending actions, to add everything at the same time
        if (how_many_to_clone > 0)
        {
          mixed * action;
          action = ({ current_shop, current_good, how_many_to_clone });          
          pending_clones += ({ action });
        }

        // new_object->dest_me();
      
      } // if the item loads
    } // for every item to add to the shop

    current_shop_ob->destroy_vault();

    next_shop++;
  } // if the shop loads

  // call_out after checking every object
  if (sizeof(pending_clones))
  {
    debug("ventures", "pending clones: " + to_string(pending_clones) + "\n");
    call_out("update_pending_clones", 5);
  }
  
  save_handler();
  
  // de-stocking every once in a while 
  // items which are not permanent_goods will be removed
  // if (random(2) == 1)
  //   call_out("reset_stock", 15, current_shop);

  return 1;
}

// remove items which are not permanent_goods
void reset_stock(string shop)
{
  object shop_ob;

  debug("ventures", "removing stock in " + shop + "\n");
  
  catch(shop_ob = load_object(shop));
  
  if (!shop_ob)
    return;
  
  shop_ob->create_vault();
 
  shop_ob->reset_stock();

  shop_ob->destroy_vault();
}

void update_pending_clones()
{
  int i;
  object shop;
  string shop_name;
  object * list;
  mixed * action_info;

  shop_name = "";
  list = ({ });

  debug("ventures", "update pending clones ...\n");

  // every item of the same shop
  while (sizeof(pending_clones) &&
        (action_info = pending_clones[0]) &&
        (sizeof(action_info) == 3))
  {
    // first action sets the shop
    if (!strlen(shop_name))
    {
      shop_name = action_info[0];

      // if the shop name ends in .o, its a location shop component
      if (shop_name[strlen(shop_name) - 2..strlen(shop_name) - 1] == ".o")
      {
        shop = load_object(LOCATION_HANDLER)->load_location(shop_name);
        if (shop)
          shop = shop->query_component_by_type(LOCATION_COMPONENT_SHOP);
      }
      else
        shop = load_object(shop_name);
    }
    // if it is a different shop, do not update
    else if (shop_name != action_info[0])
      break;

    // something does not load
    if (!shop || !load_object(action_info[1]))
    {
      pending_clones -= ({ pending_clones[0], });
      continue;
    }

    // every copy
    for (i = 0; i < action_info[2]; i++)
      list += ({ clone_object(action_info[1]), });

    // remove the pending action
    pending_clones -= ({ pending_clones[0], });
  }

  if (shop && sizeof(list))
  {
    shop->create_vault();

    // move every item in a single call
    shop->load_objects_from_handler(list);        

    shop->destroy_vault();
  }
  
  save_handler();
  
  // queue next shop updated
  if (sizeof(pending_clones))
    call_out("update_pending_clones", 5);
}
