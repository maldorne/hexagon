/*
 *
 * Edited 11 SEP 94 by Timion and Taniwha to fix the sell item bug.
 *
 * Original made who knows when.
 * Modified by bil to make the list a lot nicer.
 * 
 * Reworked by Hamlet July/August 1995.
 * New features:
 *      1)  DOCUMENTATION!
 *      2)  A more sensical scaled_value() (makes more sense to me at least)
 *      3)  Ability to force-set the price the shop will offer for an item;
 *          set_resale_value(), adjust_resale_value(), query_resale_value(),
 *          prevent_resale(), and allow_resale()  all added to
 *          /std/basic/misc.c (so, /std/object).
 *      4)  Stolen items are worth less now. % modifier and
 *          set_stolen_modifier() to alter that modifier.  Also
 *          set_stolen_modifier(), query_stolen_modifier(), and
 *          no_sell_if_stolen() in /std/basic/misc.c
 *      5)  A shop can be sell-only. The commands "value" and "sell"
 *          are turned off for that. Put sell_only() in setup();
 *      6)  You can specify a special function in your shop to be run
 *          ever time something is sold or bought.  call is
 *          set_sell_func("func") and set_buy_func("func");
 *          func should look like: void func(object *stuff);
 *          Yes, this means that stuff is an ARRAY of items being bought
 *          or sold.
 *
 * I am not sure whose code is whose any more. This *is* an extensive
 * rewrite, and a lot of it is now mine. Credits stay, though, since
 * I don't know which is which. - Hamlet
 * Note: rewrite is not done. A lot of the code is original still.
 *       Don't expect that to be true for long (muhahaha)
 *
 * Important changes for CcMud, neverbot 20/7/03
 * - remove the storeroom files (no need to use two different files and rooms per 
 *   shop), using the vault_room system. The shop 'saves' the items in inventory.
 * - remove the 'fluctuating' prices, maybe in the future will be restored.
 * - everything reviewed and modified... (is mostly a new shop.c)
 * 
 * Changes for CcMud, neverbot 08/08/2008
 * - shop handler to store a list of every shop in the mud.
 * - the items that the shop must always have in inventory are not cloned from the store, 
 *   we just have a list and the shop handler updates the inventories..
 * - stolen_modifier removed.
 *
 * Changes for Hexagon, neverbot 01/2025
 * - shop functions refactored in several files, to be inherited both
 *   from the shop room (/lib/ventures/shop.c) and the shop component
 *   for locations (/lib/location/components/shop.c). 
 *   
 */

inherit room       "/lib/room.c";
inherit attendable "/lib/ventures/attendable.c";
inherit sign       "/lib/ventures/shop-sign.c";
inherit inventory  "/lib/ventures/shop-inventory.c";
inherit actions    "/lib/ventures/shop-actions.c";
inherit messages   "/lib/ventures/shop-messages.c";
inherit prices     "/lib/ventures/prices.c";

#include <basic/money.h>
#include <basic/move.h>
#include <room/ventures.h>
#include <room/storage.h>
#include <mud/secure.h>
#include <language.h>


// int amount_sold, amount_bought;
static string sell_func, buy_func;

// list of shop admins (can see logs)
string * shop_admins;

void set_admins(string * admins) { if (sizeof(admins)) shop_admins = admins; }
string *query_shop_admins() { return shop_admins; }

string query_save_file_name();
string query_log_file_name();

void create() 
{
  shop_admins = ({ });
  sell_func = nil;
  buy_func = nil;

  inventory::create();
  attendable::create();
  messages::create();
  room::create();
  
  create_sign();

  // include the shop in the ventures handler
  if (base_name(this_object()) != "/lib/ventures/shop")
    handler("ventures")->include_shop(base_name(this_object()));
}

string query_save_file_name()
{
  return "/save/ventures/" + implode(explode(base_name(this_object()), "/"), "-") + "-save.o";
}

string query_log_file_name()
{
  return "/save/ventures/" + implode(explode(base_name(this_object()), "/"), "-") + "-log";
}

/* 
 * These initialize sell_func and buy_func, which are special functions
 * that can be set to run when something is bought or sold.
 */
void set_sell_func(string str) { sell_func = str; }
string query_sell_func() { return sell_func; }
void set_buy_func(string str) { buy_func = str; }
string query_buy_func() { return buy_func; }

nomask int query_shop(){ return 1; }

// string query_save_file() { return(save_file); }
// string query_log_file() { return log_file; }

void init() 
{
  room::init();
  actions::init();
}

void dest_me() 
{
  inventory::dest_me();
}

// returns 1 if the object can be sold to the shop, 0 if can not
int check_sell(object ob) { return 1; }

mixed * stats()
{
  return room::stats() +
    inventory::stats() +
    attendable::stats();
}
