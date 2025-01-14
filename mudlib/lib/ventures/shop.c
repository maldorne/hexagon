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
 */

inherit room       "/lib/room.c";
inherit attendable "/lib/ventures/attendable.c";
inherit sign       "/lib/ventures/shop-sign.c";
inherit inventory  "/lib/ventures/shop-inventory.c";

#include <basic/money.h>
#include <basic/move.h>
#include <room/ventures.h>
#include <room/storage.h>
#include <mud/secure.h>
#include <language.h>

#define MIN_PRICE 10
// change, you die
#define FILE_SIZE 30000

// new storeroom system, neverbot 7/03
static string save_file, log_file;

static mixed buy_mess, sell_mess, list_mess, value_mess, browse_mess;

// int amount_sold, amount_bought;
static int only_sell;
static string sell_func, buy_func;

// list of shop admins (can see logs)
string * shop_admins;

void set_admins(string * admins) { if (sizeof(admins)) shop_admins = admins; }
string *query_shop_admins() { return shop_admins; }

// adaptive price functions, Anirudh
int do_cha_adjust(int amt, int cha, int updown);

private int scaled_value(int n);
private string shop_parse(string str, mixed ob, object client, string money, string extra);
private void do_parse(mixed arr, mixed ob, object client, string money, string extra);

private string get_save_file_name();
private string get_log_file_name();

void create() 
{
  only_sell = 0;
  shop_admins = ({ });
  buy_mess = _LANG_SHOP_DEFAULT_BUY_MSGS;
  sell_mess = _LANG_SHOP_DEFAULT_SELL_MSGS;
  list_mess = "$extra$";
  value_mess = _LANG_SHOP_DEFAULT_VALUE_MSG;
  browse_mess = _LANG_SHOP_DEFAULT_BROWSE_MSG;

  save_file = get_save_file_name();
  log_file = get_log_file_name();

  room::create();
  inventory::create();
  
  create_sign();

  // include the shop in the ventures handler
  if (base_name(this_object()) != "/lib/ventures/shop")
    handler("ventures")->include_shop(base_name(this_object()));
}

private string get_save_file_name()
{
  return "/save/ventures/" + implode(explode(base_name(this_object()), "/"), "-") + "-save.o";
}

private string get_log_file_name()
{
  return "ventures/" + implode(explode(base_name(this_object()), "/"), "-") + "-log";
}

/* These make spiffy messages for various actions. */
void set_sell_message(mixed str) { sell_mess = str; }
void set_buy_message(mixed str) { buy_mess = str; }
void set_value_message(mixed str) { value_mess = str; }
void set_browse_message(mixed str) { browse_mess = str; }
void set_list_message(mixed str) { list_mess = str; }

mixed query_sell_mess() { return sell_mess; }
mixed query_list_mess() { return list_mess; }
mixed query_value_mess() { return value_mess; }
mixed query_buy_mess() { return buy_mess; }
mixed query_browse_mess() { return browse_mess; }

/* 
 * These initialize sell_func and buy_func, which are special functions
 * that can be set to run when something is bought or sold.
 */
void set_sell_func(string str) { sell_func = str; }
void set_buy_func(string str) { buy_func = str; }
void sell_only() { only_sell = 1; }
int query_only_sell() { return only_sell; }
nomask int query_shop(){ return 1; }

string query_save_file() { return(save_file); }
string query_log_file() { return log_file; }

void init() 
{
  ::init();

  add_action("do_list", _LANG_SHOP_VERBS_LIST);
  add_action("do_sell", _LANG_SHOP_VERBS_SELL);
  add_action("do_browse", _LANG_SHOP_VERBS_BROWSE);
  add_action("do_value", _LANG_SHOP_VERBS_VALUE);
  add_action("do_buy", _LANG_SHOP_VERBS_BUY);
  add_action("view_shop_log", _LANG_SHOP_VERBS_LOGS);
}

// returns 1 if the object can be sold to the shop, 0 if can not
int check_sell(object ob) { return 1; }

int do_list(string str) 
{
  object vault;
  object * all;
  string tmp;
  mapping list;
  string * name_list;
  int i;

  tmp = "";
  list = ([ ]);
  name_list = ({ });

  if (!check_open_condition())
    return 0;

  // same safety system as in the vault_rooms
  if (query_property(VAULT_USE_PROP)) 
  {
    notify_fail(_LANG_SHOP_STORE_IN_USE);
    return 0;
  }

  // shop inventory object
  vault = clone_object(VAULT_FILES_PATH + "vault_obj.c");
  vault->move(this_object());
  vault->set_save_file(save_file);

  all = all_inventory(vault);

  if (sizeof(all))
  {
    write(_LANG_SHOP_LIST_SUMMARY);

    // build a mapping like ([ name: ({ how_many, ob }) ])
    // we need only one copy of the item in ob, just to check gender, etc
    for (i = 0; i < sizeof(all); i++)
    {
      if (all[i]->short()){
        if (undefinedp(list[all[i]->short()]))
          list[all[i]->short()] = ({ 1, all[i] });
        else
          list[all[i]->short()][0] += 1;
      }
    }

    name_list = keys(list);

    for (i = 0; i < sizeof(name_list); i++)
    {
      switch(list[name_list[i]][0]) {
        case 1:
          tmp += "   " + _LANG_SHOP_LIST_JUST_ONE + 
            (this_player()->query_coder() ? " (" + list[name_list[i]][0] + ")" : "") + "\n";
          break;
        case 2..5:
          tmp += "   " + capitalize(query_num(list[name_list[i]][0], 10)) + " " +
            (string)list[name_list[i]][1]->query_plural() + 
            (this_player()->query_coder() ? " (" + list[name_list[i]][0] + ")" : "") + "\n";
          break;
        default:
          tmp += "   " + _LANG_SHOP_LIST_LOTS + 
            (this_player()->query_coder() ? " (" + list[name_list[i]][0] + ")" : "") + "\n";
      }
    }

    tmp += "\n";

    this_user()->more_string(tmp);

    if (this_player()->query_coder())
      write("Max amount: " + FILE_SIZE + ", current amount: " + file_size(query_save_file()) + "\n");
  }
  else
    write(_LANG_SHOP_LIST_EMPTY);
  
  vault->dest_me();
  return 1;
}

/*
 * Now shops store items the same way as vault_rooms 
 * neverbot 7/03
 */
int do_sell(string str) 
{
  object vault;
  object *obs, *selling, *cannot;
  object *prev; // list of same items already in the shop
  mixed *m_array;
  int i, amt, total_amt;

  if (!check_open_condition())
    return 0;

  if (only_sell)
  {
    tell_object(this_player(), _LANG_SHOP_ONLY_SELL);
    return 1;
  }

  if (!strlen(str))
  {
    notify_fail(_LANG_SHOP_SELL_SYNTAX);
    return 0;
  }

  // same safety system as in the vault_rooms
  if (query_property(VAULT_USE_PROP)) 
  {
    notify_fail(_LANG_SHOP_STORE_IN_USE);
    return 0;
  }

  if (file_size(query_save_file()) > FILE_SIZE)
  {
    notify_fail(_LANG_SHOP_STORE_FULL);
    return 0;
  }  

  obs = find_match(str, this_player());
  if (!sizeof(obs))
  {
    notify_fail(_LANG_SHOP_NOTHING_TO_VERB);
    return 0;
  }
  
  // filter items by type, removing types not accepted by this, neverbot 10/09
  obs = filter_array(obs, "check_sell");

  if (!sizeof(obs))
  {
    notify_fail(_LANG_SHOP_NOT_THIS_TYPE);
    return 0;
  }

  // shop inventory object
  vault = clone_object(VAULT_FILES_PATH + "vault_obj.c");
  vault->move(this_object());
  vault->set_save_file(save_file);

  // little fix, shops do not buy items until they sell some stock
  prev = vault->simple_find_match(obs[0]->query_main_plural());
  
  if (sizeof(prev) >= MAX_OBS)
  {
    notify_fail(_LANG_SHOP_SELL_NOT_MORE);
    vault->dest_me();
    return 0;
  }

  if (sizeof(obs) > MAX_OBS)
  {
    write(_LANG_SHOP_SELL_TOO_MANY);
    obs = obs[0..MAX_OBS - 1];
  }

  selling = cannot = ({ });
  amt = total_amt = 0;

  for (i = 0; i < sizeof(obs); i++)
  {
    if ( (obs[i]->query_value() > 0) && 
        !obs[i]->do_not_sell() &&
        (obs[i]->query_resale_value() != -1) &&
        // ((obs[i]->query_stolen_modifier() != -1) || !obs[i]->query_property("stolen")) && 
        !obs[i]->query_in_use() && 
        (environment(obs[i]) == this_player()) )
    {
      amt = scaled_value(obs[i]->query_value());
      if (amt > MAX_AMOUNT)
        amt = MAX_AMOUNT;

      // Let's not let something sell back for more than it was bought for
      if (amt > obs[i]->query_value())
        amt = obs[i]->query_value();

      /* Adjusts price by item's adaptive multiplier (in 1/10 %) */
      /* Adjust by mudwide scale factor */
      // TODO: value adjustments removed temporarily, neverbot 05/06
      /*
      amt = amt * MONEY_TRACKER->query_adj_fact(SBFLAG)/1000;
      */
      amt = do_cha_adjust(amt, this_player()->query_cha(), 1);

      if (obs[i]->avoid_auto_load() || obs[i]->query_no_save_object())
      {
        write(_LANG_SHOP_SELL_NOT_THIS_TYPE);
        vault->dest_me();
        return 1;
      }

      obs[i]->move(vault);

      log_file(query_log_file(), "[shop buys - " + ctime(time(), 4) + "] " +
                 this_player()->query_cap_name() +
                 " - " + obs[i]->query_short() +
                 " - " + base_name(obs[i]) + ", price: " + amt + "\n");

      total_amt += amt;
      selling += ({ obs[i] });
    }
    else if (obs[i]->short())
      cannot += ({ obs[i] });
  }

  if (!sizeof(selling))
  {
    if (sizeof(cannot))
      notify_fail(_LANG_SHOP_SELL_CANNOT_SELL);
    else
      notify_fail(_LANG_SHOP_SELL_NOTHING);

    vault->dest_me();
    return 0;
  }

  if (sizeof(cannot))
    write(_LANG_SHOP_SELL_CANNOT_SELL);

  m_array = (mixed *)MONEY_HAND->create_money_array(total_amt);
  this_player()->adjust_money(m_array);

  do_parse(sell_mess, selling, this_player(), (string)MONEY_HAND->money_string(m_array), "");

  if (sell_func)
    call_other(this_object(), sell_func, selling, total_amt);

  vault->dest_me();

  // save the player inventory
  this_player()->save_me();

  return 1;
}

int do_browse(string str) 
{
  object *obs;
  int i, ob_amt;
  object vault;

  if (!check_open_condition())
    return 0;

  if (!strlen(str))
  {
    notify_fail(_LANG_SHOP_BROWSE_SYNTAX);
    return 0;
  }

  // same safety system as in the vault_rooms
  if (query_property(VAULT_USE_PROP)) 
  {
    notify_fail(_LANG_SHOP_STORE_IN_USE);
    return 0;
  }

  // shop inventory object
  vault = clone_object(VAULT_FILES_PATH + "vault_obj.c");
  vault->move(this_object());
  vault->set_save_file(save_file);

  obs = vault->simple_find_match(str);

  if (!sizeof(obs))
  {
    notify_fail(_LANG_SHOP_NOT_FOUND);
    vault->dest_me();
    return 0;
  }

  for (i = 0; i < sizeof(obs); i++) 
  {
    ob_amt = obs[i]->query_value();
    
    // TODO: value adjustments removed temporarily, neverbot 05/06
    // ob_amt = ob_amt * MONEY_TRACKER->query_adj_fact(SSFLAG)/1000;
    
    ob_amt = do_cha_adjust(ob_amt, this_player()->query_cha(), 0);

    if (ob_amt < 1)
      ob_amt = MIN_PRICE;

    do_parse(browse_mess, obs[i], this_player(), 
      (string)MONEY_HAND->money_value_string(ob_amt),
      (string)obs[i]->long());
  }

  vault->dest_me();
  return 1;
}

int do_value(string str)
{
  object *obs;
  int i, amt;

  if (!check_open_condition())
    return 0;

  if (!strlen(str))
  {
    notify_fail(_LANG_SHOP_VALUE_SYNTAX);
    return 0;
  }

  if (only_sell)
  {
    tell_object(this_player(), _LANG_SHOP_ONLY_SELL);
    return 1;
  }

  obs = find_match(str, this_player());

  if (!sizeof(obs))
  {
    notify_fail(_LANG_SHOP_NOT_FOUND);
    return 0;
  }

  for (i = 0; i < sizeof(obs); i++)
  {
    amt = scaled_value(obs[i]->query_value());
    if (amt > MAX_AMOUNT)
      amt = MAX_AMOUNT;

    // Let's not let something sell back for more than it was bought for
    if (amt > obs[i]->query_value())
      amt = obs[i]->query_value();

    /* Adjusts price by item's adaptive multiplier (in 1/10 %) */
    /* Adjust by mudwide scale factor */
    // TODO: value adjustments removed temporarily, neverbot 05/06
    /*
    amt = amt * MONEY_TRACKER->query_adj_fact(SBFLAG)/1000;
    */
    amt = do_cha_adjust(amt, this_player()->query_cha(), 1);

    do_parse(value_mess, obs[i], this_player(),
      (string)MONEY_HAND->money_value_string(amt),
      (string)(obs[i]->do_not_sell() || (obs[i]->query_resale_value() == -1)) );
  }

  return 1;
}

int do_buy(string str) 
{
  int i, amt, ob_amt, total_cost;
  object *obs, *to_buy, *cannot, *too_much;
  string s;
  object vault;
  mixed aux;

  if (!check_open_condition())
    return 0;

  if (!strlen(str))
  {
    notify_fail(_LANG_SHOP_BUY_SYNTAX);
    return 0;
  }

  // same safety system as in the vault_rooms
  if (query_property(VAULT_USE_PROP)) 
  {
    notify_fail(_LANG_SHOP_STORE_IN_USE);
    return 0;
  }

  // shop inventory object
  vault = clone_object(VAULT_FILES_PATH + "vault_obj.c");
  vault->move(this_object());
  vault->set_save_file(save_file);

  obs = vault->simple_find_match(str);

  if (!sizeof(obs))
  {
    notify_fail(_LANG_SHOP_NOT_FOUND);
    vault->dest_me();
    return 0;
  }

  if (sizeof(obs) > MAX_OBS)
  {
    write(_LANG_SHOP_BUY_TOO_MANY);
    obs = obs[0..MAX_OBS - 1];
  }

  to_buy = too_much = cannot = ({ });

  amt = this_player()->query_value();

  while (i < sizeof(obs))
  {
    ob_amt = obs[i]->query_value();
    
    /* Adjusts price by item's adaptive multiplier (in 1/10 %) */
    /* Adjust by mudwide scale factor */
    // TODO: value adjustments removed temporarily, neverbot 05/06
    /*
    ob_amt = ob_amt * MONEY_TRACKER->query_adj_fact(SBFLAG)/1000;
    */

    ob_amt = do_cha_adjust(ob_amt, this_player()->query_cha(), 0);

    if (ob_amt < 1) 
    {
      ob_amt = MIN_PRICE;
      log_file("shops", "[" + ctime(time(), 4) + "]" + file_name(this_object()) + ": sells " +
                      file_name(obs[i]) + " for 0 value to " +
                      this_player()->query_name() + " (min price assigned of " + 
                      MIN_PRICE + ")\n");
    }

    if (ob_amt > amt)
    {
      if (obs[i]->short())
        too_much += ({ obs[i] });

      obs = delete(obs, i, 1);
      continue;
    }

    if (obs[i]->move(this_player())) 
    {
      cannot += ({ obs[i] });
    }
    else
    {
      amt -= ob_amt;
      total_cost += ob_amt;
      to_buy += ({ obs[i] });

      log_file(query_log_file(), "[shop sells - " + ctime(time(), 4) + "] " +
                 this_player()->query_cap_name() +
                 " - " + obs[i]->query_short() +
                 " - " + base_name(obs[i]) + ", price: " + ob_amt + "\n");
    }

    i++;
  }

  s = "";

  if (sizeof(cannot))
    s += _LANG_SHOP_BUY_CANNOT_MOVE;
  
  if (sizeof(too_much))
    s += _LANG_SHOP_BUY_TOO_EXPENSIVE;
  
  if (!sizeof(to_buy))
  {
    if (s != "")
      notify_fail(s);
    else
      notify_fail(_LANG_SHOP_BUY_NOTHING);

    vault->dest_me();
    return 0;
  } 
  else 
    write(s);

  this_player()->pay_money( aux = MONEY_HAND->create_money_array(total_cost));

  do_parse(buy_mess, to_buy, this_player(), (string)MONEY_HAND->money_string(aux), "");
  
  if (buy_func)
    call_other(this_object(), buy_func, to_buy, total_cost);

  vault->dest_me();

  // save the player inventory
  this_player()->save_me();

  return 1;
}

int view_shop_log(string str)
{
  string name;
  string tmp;

  if (strlen(str)) 
  {
    notify_fail(_LANG_SHOP_LOGS_SYNTAX);
    return(0);
  }

  name = this_player()->query_name();
  
  // only coders, shop admins, or anybody if admins == "all"
  if (this_player()->query_coder() ||
     (member_array(name, shop_admins) != -1) ||
     !sizeof(shop_admins) || 
     (shop_admins[0] == "all") )
  {
   tmp = read_file("/log/" + query_log_file());

   if (!tmp || tmp == "")
   {
     tell_object(this_player(), _LANG_SHOP_LOGS_EMPTY);
     return 1;
   }

   this_player()->more_string(tmp);
   return 1;
  }
  
  notify_fail(_LANG_SHOP_LOGS_NO_PERMISSION);
  return 0;
}

/* 
 * This determines how much the shopkeeper will offer for an item, according
 * to its value.  It only gets called if set_no_resell() has not been called
 * and resale_value has not been hand-set to something.
 */
private int scaled_value(int n)
{
  int *prates;
  int i, tot;

  tot = 0;
  prates = PAY_RATES;
  for (i = 0; ( (i < sizeof(prates)) && (n > prates[i]) ); i+=2);

  if (i > 0)
    i -= 2;
  // prates[i] is now the rate-increment directly below the value.

  tot = (n - prates[i]) * ((prates[i+2] / prates[i+3]) -
    (prates[i] / prates[i+1]));
  tot /= (prates[i+2] - prates[i]);
  tot += (prates[i] / prates[i+1]);

  // For those curious, we just defined a line segment.  Basically, we
  // used  y - y1 = m(x - x1).  And found the y value for x = n.
  // Read ventures.h for better explanation.
  return tot;
}

int do_cha_adjust(int amt, int cha, int updown)
{
  int temp;

  if (!cha) 
    return amt;
  if (cha > 19) 
    cha = 19;

  if (cha > AVG_CHA)
  {
    temp = 10 * (cha - AVG_CHA) * CHA_AD / (18 - AVG_CHA);
    if (updown) 
    {
      temp = amt * (1000 + temp) / 1000 - 1;
      if (temp < amt) 
        return amt;
    }
    else 
    {
      temp = amt * 1000 / (1000 + temp) + 1;
      if (temp > amt) 
        return amt;
    }
    return temp;
  }

  if (cha < AVG_CHA)
  {
    temp = 10 * (AVG_CHA - cha) * CHA_AD / (AVG_CHA - 3);
    if (updown)
    {
      temp = amt * 1000 / (1000 + temp) + 1;
      if (temp > amt) 
        return amt;
    }
    else
    {
      temp = amt * (1000 + temp) / 1000 - 1;
      if (temp < amt) 
        return amt;
    }

    return temp;
  }

  return amt;
}

// show messages after buying or selling
private void do_parse(mixed arr, mixed ob, object client, string money, string extra) 
{
  if (stringp(arr))
    write(shop_parse(arr, ob, client, money, extra));
  else
  {
    write(shop_parse(arr[0], ob, client, money, extra));
    say(shop_parse(arr[1], ob, client, money, extra));
  }
}

private string shop_parse(string str, mixed ob, object client, string money, string extra) 
{
  string s1, s2, s3, rest;
  rest = "";

  while (sscanf(str,"%s$%s$%s", s1, s2, s3) == 3)
  {
    switch (s2) 
    {
      case "ob":
        if (pointerp(ob))
          str = s1 + query_multiple_short(ob) + s3;
        else
          str = s1 + ob->short() + s3;
        break;
      case "client":
        str = s1 + client->query_cap_name() + s3;
        break;
      case "extra":
        str = s1 + extra + s3;
        break;
      case "money":
        str = s1 + money + s3;
        break;
      default:
        rest = s1 + "$" + s2 + "$";
        str = s3;
        break;
    }
  }

  return rest + str;
}

// ************************************************************
//  start of ventures handler related functions
// ************************************************************

// called only from the venture handlers
int load_objects_from_handler(object * obs)
{
  object vault;
  int i; 

  i = 0;

  if (!sizeof(obs)) 
    return 0;

  debug("shops", base_name(this_object()) + " : " + sizeof(obs) + " new items\n");

  // same safety system as in the vault_rooms
  if (query_property(VAULT_USE_PROP))
  {
    for (i = 0; i < sizeof(obs); i++)
      if (obs[i])
        obs[i]->dest_me();
    return 0;
  }

  // shop inventory object
  vault = clone_object(VAULT_FILES_PATH + "vault_obj.c");
  vault->move(this_object());
  vault->set_save_file(save_file);

  for (i = 0; i < sizeof(obs); i++)
  {
    // Extra comprobacion de seguridad!!!!!!
    // Por algun problema a la hora de comprobar el contenido de las tiendas desde el shop_handler, 
    // en ocasiones la tienda informa de que no tiene un objeto cuando SI lo tiene.
    // Volvemos a comprobar aqui por si se ha producido ese error.
    // if (sizeof(find_match(obs[i]->query_main_plural(), vault)) >= permanent_goods[base_name(obs[i])])
    // {
    //   tell_object(find_living("folken"), "(SHOP) Pillado error de objetos de mas!!!!!\n");
    //     obs[i]->dest_me();
    //     continue;
    // }
      
    if (obs[i])
      obs[i]->move(vault);
  }

  vault->dest_me();
  return 1;
}

int query_how_many_in_inventory(string ob_main_plural)
{
  object vault;
  int res;
    
  // same safety system as in the vault_rooms
  if (query_property(VAULT_USE_PROP))
  {
    // No devolvemos cero (esta funcion se llama desde el handler de tiendas para repoblar la lista
    //   de objetos en venta, si devolvemos cero intentara clonar nuevos objetos)
    return MAX_AMOUNT_OF_THINGS;
  }

  // shop inventory object
  vault = clone_object(VAULT_FILES_PATH + "vault_obj.c");
  vault->move(this_object());
  vault->set_save_file(save_file);

  res = sizeof(vault->simple_find_match(ob_main_plural));
  
  vault->dest_me();
  
  return res;
}

// remove non permanent_goods
// will be called every once in a while from the ventures handler, 
// although we can call it by our own if we want
void reset_stock()
{
  object vault;
  object * obs;
  int i, j;
  string * permanents;
  string name;
  int found;

  found = 0;
    
  // same safety system as in the vault_rooms
  if (query_property(VAULT_USE_PROP))
    return;

  // shop inventory object
  vault = clone_object(VAULT_FILES_PATH + "vault_obj.c");
  vault->move(this_object());
  vault->set_save_file(save_file);

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
  
  vault->dest_me();
  
  return;
}

// ************************************************************
//  end of ventures handler related functions
// ************************************************************

mixed * stats()
{
  return inventory::stats();
}
