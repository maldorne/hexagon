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
 */

#include <room/ventures.h>

inherit "/lib/core/object.c";

#define HOME "/room/weather_room.c"
#define SAVE_FILE "/save/ventures/ventures"

// Las actualizaciones de tiendas se llaman cada minuto, cuando la lista de tiendas
// termina vuelve a empezar por el principio. Si hemos hecho la lista completa en menos de 
// este tiempo, no volvemos a empezar
// 60 segundos * 60 minutos * 2 horas = 7200 segundos
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

// debug
string * query_shops_array() { return shops; }
mixed * query_pending_clones() { return pending_clones; }
void reset_pending_clones() { pending_clones = ({ }); }
void reset_stock(string shop);

void create()
{
  shops = ({ });
  pending_clones = ({ });
  next_shop = 0;
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
  new_object;
  obs = ({ });

  debug("ventures", "update_ventures...\n");

  if (!sizeof(shops))
    return 0;

  if (next_shop >= sizeof(shops))
    next_shop = 0;

  current_shop = shops[next_shop];

  debug("ventures", "updating content of " + current_shop + "\n");

  obs = ({ });
  
  catch(current_shop_ob = load_object(current_shop));
  
  if (!current_shop_ob)
  {
    // if it does not load, remove
    shops -= ({ current_shop });
  }
  else
  {
    permanent_goods = current_shop_ob->query_permanent_goods();
    permanent_goods_paths = keys(permanent_goods);
    
    // every item to add in this shop
    for (i = 0; i < sizeof(permanent_goods_paths); i++)
    {
      current_good = permanent_goods_paths[i];
    
      // check if something is wrong with the item
      if (intp(permanent_goods[current_good]) && 
         (load_object(current_good)))
      {
        new_object = clone_object(current_good);

        how_many_in_shop = current_shop_ob->query_how_many_in_inventory(new_object->query_main_plural());
        
        // if the max amount is returned, the shop is busy, we can't do anything
        if (how_many_in_shop == MAX_AMOUNT_OF_THINGS)
        {
          new_object->dest_me();
          break;
        }

        // how many needed (must have - already have)
        how_many_to_clone = (int)permanent_goods[current_good] - how_many_in_shop;

stderr("---------> " + how_many_to_clone +" "+(int)permanent_goods[current_good] +" "+ how_many_in_shop +"\n");

        // update a list of pending actions, to add everything at the same time
        if (how_many_to_clone > 0)
        {
          mixed * action;
          action = ({ current_shop, current_good, how_many_to_clone });          
          pending_clones += ({ action });
        }

        new_object->dest_me();
      
      } // if the item loads
    } // for every item to add to the shop

    next_shop++;

  } // if the shop loads

  // call_out after checking every object
  if (sizeof(pending_clones))
    call_out("update_pending_clones", 5);
  
  save_handler();
  
  // de-stocking every once in a while 
  // items which are not permanent_goods will be removed
  if (random(2) == 1)
    call_out("reset_stock", 15, current_shop);

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
  
  shop_ob->reset_stock();  
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
  
  stderr("--------------> " + to_string(pending_clones) + "\n");
  
  // every item of the same shop
  while (sizeof(pending_clones) &&
        (action_info = pending_clones[0]) &&
        (sizeof(action_info) == 3))
  {
    // first action sets the shop
    if (!strlen(shop_name))
      shop_name = action_info[0];

    // if it is a different shop, do not update
    if (shop_name != action_info[0])
      break;

    // something does not load
    if (!(shop = load_object(action_info[0])) || !load_object(action_info[1]))
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
  
stderr("--------------> " + to_string(list) + "\n");

  // move every item in a single call
  if (shop && sizeof(list))
    shop->load_objects_from_handler(list);        
  
  save_handler();
  
  // queue next shop updated
  if (sizeof(pending_clones))
    call_out("update_pending_clones", 5);
}
