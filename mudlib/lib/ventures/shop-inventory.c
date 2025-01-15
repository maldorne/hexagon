/*
 * Changes for Hexagon, neverbot 01/2025
 * 
 * - shop functions refactored in several files, to be inherited both
 *   from the shop room (/lib/ventures/shop.c) and the shop component
 *   for locations (/lib/location/components/shop.c). More info about
 *   previous changes in the header comments of the original shop file.
 * - new create_vault() and destroy_vault() to allow batch operations
 *   without having to recreate the vault_obj every time. Mainly used from
 *   the ventures handler.
 * - IMPORTANT: if you create a vault with create_vault, the shop won't
 *   be available to use while it exists (the same as when a player 
 *   executes an action, the shop is blocked from start to the end of the
 *   action). BUT with create_vault, the vault won't be destroyed automatically
 *   until you call destroy_vault(). Check the ventures handler to check how
 *   it has been done if any doubts.
 *
 */

#include <room/storage.h>
#include <room/ventures.h>

// items the shop _must_ have
static mapping permanent_goods;

// temporary vault object
static object vault;

// prototypes
// backwards compatibility
int add_thing(string file, int amount);
int add_permanent_goods(string file, int amount);
mapping query_permanent_goods(){ return permanent_goods; }

void create() 
{
  permanent_goods = ([ ]);
  vault = nil;
}

// backwards compatibility
int add_thing(string file, int amount) { return add_permanent_goods(file, amount); }

//  with the new shop system, the items bought and sold are _always_ saved, so
//  we need a way to restock some basic objects if we want them to always be
//  available
int add_permanent_goods(string file, int amount)
{
  object ob;
  ob = load_object(file);
  
  if (!ob)
  {
    debug("shops", " invalid add_permanent_goods in " + file_name(this_object()) + " : " + file + "\n");
  
    // create new issue, same message as npcs equipment
    if (package("issues"))
      package("issues")->add_issue("Cannot clone " + file, this_object());
  
    return 0;
  }
  
  permanent_goods[base_name(ob)] = amount;
}

// the vault object will normally be created when needed, every time we 
// execute any of the actions in shop-actions.c

// important!!! we need to destroy the object after we finish doing whatever
// we are doing, because while there is a vault obj in a shop, other
// actions are blocked

object query_vault_ob() { return vault; }

object create_vault()
{
  if (vault)
    return vault;

  // has already a vault in the inventory
  if (this_object()->query_property(VAULT_USE_PROP)) 
    return nil;

  vault = clone_object(VAULT_FILES_PATH + "vault_obj.c");
  vault->move(this_object());
  vault->set_save_file(this_object()->query_save_file_name());

  return vault;
}

void destroy_vault()
{
  if (vault)
    vault->dest_me();
}

void dest_me()
{
  if (vault)
    vault->dest_me();
}

// ************************************************************
//  start of ventures handler related functions
//  
//  important!: functions in here require create_vault had been
//  called before using them, and destroy_vault after
// ************************************************************

// called only from the venture handlers
int load_objects_from_handler(object * obs)
{
  int i; 

  if (!sizeof(obs)) 
    return 0;

  debug("shops", base_name(this_object()) + " : " + sizeof(obs) + " new items\n");

  if (!vault)
  {
    for (i = 0; i < sizeof(obs); i++)
      if (obs[i])
        obs[i]->dest_me();
    return 0;
  }

  // shop inventory object
  vault = create_vault();

  for (i = 0; i < sizeof(obs); i++)
  {
    // Extra check for security!!!!!!
    // By some problem when checking the content of the shops from the shop_handler, 
    // sometimes the shop reports that it does not have an object when it DOES have it.
    // We check again here in case that error has occurred.
    // if (sizeof(find_match(obs[i]->query_main_plural(), vault)) >= permanent_goods[base_name(obs[i])])
    // {
    //   tell_object(find_living("folken"), "(SHOP) Pillado error de objetos de mas!!!!!\n");
    //     obs[i]->dest_me();
    //     continue;
    // }
      
    if (obs[i])
      obs[i]->move(vault);
  }

  return 1;
}

int query_how_many_in_inventory(string ob_main_plural)
{
  int res;
    
  if (!vault)
  {
    // We do not return zero, this function is called from the ventures handler to repopulate the list
    // of objects for sale, if we return zero it will try to clone new objects
    return MAX_AMOUNT_OF_THINGS;
  }

  // shop inventory object
  vault = create_vault();

  return sizeof(vault->simple_find_match(ob_main_plural));
}

// remove non permanent_goods
// will be called every once in a while from the ventures handler, 
// although we can call it by our own if we want
void reset_stock()
{
  object * obs;
  int i, j;
  string * permanents;
  string name;
  int found;

  found = 0;
    
  if (!vault)
    return;

  // shop inventory object
  vault = create_vault();

  obs = all_inventory(vault);
  
  permanents = keys(permanent_goods);
  
  for (i = 0; i < sizeof(obs); i++)
  {
    name = base_name(obs[i]);
    
    // non basic obs stay in the shop
    if (member_array("baseobs", explode(name, "/")) == -1)
      continue;
    
    found = 0;

    // test if our item is a permanent_good
    for (j = 0; j < sizeof(permanents); j++)
    {
      if (base_name(load_object(permanents[j])) == name)
      {
        found = 1;
        break;
      }
    }
    
    if (found)
      continue;
    
    debug("shops", base_name(this_object()) + " : reset_stock, removing " + 
      base_name(obs[i]) + "\n");
      
    // here, obs[i] is a basic object and not a permanent_good... 
    // we can remove it
    obs[i]->dest_me();    
  }
}

// ************************************************************
//  end of ventures handler related functions
// ************************************************************

mixed * stats()
{
  return ({
    ({ "Permanent Goods", permanent_goods }),
  });
}