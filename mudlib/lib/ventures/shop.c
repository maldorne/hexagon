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

inherit room "/lib/room.c";
inherit attendable "/lib/room/attendable.c";

#include <basic/money.h>
#include <basic/move.h>
#include <room/ventures.h>
#include <room/storage.h>
#include <mud/secure.h>
#include <language.h>

#define MIN_PRICE 10
// change, you die
#define FILE_SIZE 30000

// Nuevo sistema de almacenamiento, neverbot 7/03
static string save_file, log_file;

// Objetos que siempre debe tener la tienda
static mapping permanent_goods;

static mixed buy_mess, sell_mess, list_mess, value_mess, browse_mess;

// int amount_sold, amount_bought;
// string shop_type;
// int stolen_modifier;  // This will be a percent value
static int only_sell;
static string sell_func, buy_func;

// Lista de administradores de la tienda (pueden ver logs)
string * shop_admin;

void set_admins(string *admin) { if (sizeof(admin)) shop_admin = admin; }
string *query_shop_admins() { return shop_admin; }

/* adaptive price functions, Anirudh */
int do_weight_min(int amt, int mass, int material, int sellbuy);
int do_cha_adjust(int amt, int cha, int updown);
int do_race_adjust(int amt, string race, int updown);

// Mantenida para compatibilidad
int add_thing(string file, int amount);
int add_permanent_goods(string file, int amount);
mapping query_permanent_goods(){ return permanent_goods; }

private int scaled_value(int n);
int check_inv(object ob, string str);
string shop_parse(string str, mixed ob, object client, string money, string extra);
void do_parse(mixed arr, mixed ob, object client, string money, string extra);

private string get_save_file_name();
private string get_log_file_name();

void create_sign()
{
	string text;
  text = _LANG_SHOP_SIGN_INFO;

  if (!only_sell)
    text += _LANG_SHOP_SIGN_INFO_SELL;
    
  add_sign(_LANG_SHOP_SIGN_DESC, text);  
}

void create() 
{
  only_sell = 0;
  shop_admin = ({ });
  buy_mess = ({
    "Compras $ob$ por $money$.\n",
    "$client$ compra $ob$.\n" });
  sell_mess = ({
    "Vendes $ob$ por $money$.\n",
    "$client$ vende $ob$.\n" });
  list_mess = "$extra$";
  value_mess = "$ob$ está valorado en $money$.\n";
  browse_mess = "$ob$ cuesta $money$, descripción:\n$extra$";
  // shop_type = "general";
  // stolen_modifier = 33;  /* 33% of normal value */

  save_file = get_save_file_name();
  log_file = get_log_file_name();
  
  permanent_goods = ([ ]);

  ::create();
  
  create_sign();

  // Incluimos la tienda en la lista general de tiendas
  if (base_name(this_object()) != "/lib/ventures/shop")
    handler("ventures")->include_shop(base_name(this_object()));
}

// backwards compatibility
int add_thing(string file, int amount)
{
  return add_permanent_goods(file, amount);
}

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
    return 0;
  }
  
  permanent_goods[base_name(ob)] = amount;
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

/* These initialize sell_func and buy_func, which are special functions
 * that can be set to run when something is bought or sold.
 */
void set_sell_func(string str) { sell_func = str; }
void set_buy_func(string str) { buy_func = str; }
void sell_only() { only_sell = 1; }
nomask int query_shop(){ return 1; }
// int query_stolen_modifier(){ return stolen_modifier; }
// int set_stolen_modifier(int amt) {
//   if (amt > 100) amt = 100;
//   if (amt < 0) amt = 0;
//   stolen_modifier = amt;
//   return amt;
// }

string query_save_file() { return(save_file); }
string query_log_file() { return log_file; }

void init() 
{
  ::init();

  add_action("do_list", _LANG_SHOP_VERBS_LIST);

  add_action("sell", "vender");
  add_action("buy", "comprar");
  add_action("browse", "examinar");
  add_action("value", "valorar");
  
  add_action("view_shop_log", "logs");
}

// returns 1 if the object can be sold to the shop, 0 if can not
int check_sell(object ob)
{
  return 1;
}

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
    // list = (string *)vault->list_contents();

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














/* 
   Funcion cambiada, neverbot 7/03
   Ahora cuando vendes a la tienda un objeto, este se almacena del mismo modo
   en que se almacenan los objetos en los vault_room
*/
int sell(string str) 
{

  object vault;
  object *obs, *selling, *cannot;
  object *prev; // Lista de objetos iguales que YA estan en la tienda
  mixed *m_array;
  int i, amt, total_amt;

  if (!check_open_condition())
    return 0;

  if (only_sell) {
    tell_object(this_player(),"Esta tienda no compra mercancías.\n");
    return 1;
  }

  if (!str || str == "") {
    notify_fail("Sintaxis: vender <objetos>\n");
    return 0;
  }

  obs = find_match(str, this_player());
  if (!sizeof(obs)) {
    notify_fail("Nada que vender.\n");
    return 0;
  }
  
  // Novedad: aliminamos los objetos de tipos que no se acepten en esta tienda, neverbot 10/09
  obs = filter_array(obs, "check_sell" );

  if (!sizeof(obs)) {
    notify_fail("Esta tienda no admite ese tipo de objetos.\n");
    return 0;
  }

  // Dejamos el mismo sistema de seguridad de los vault_rooms
  if (query_property(VAULT_USE_PROP)){
    notify_fail("El almacén está siendo utilizado, inténtalo de nuevo en unos segundos.\n");
    return 0;
  }

  // Objeto con el inventario de la tienda
  vault = clone_object(VAULT_FILES_PATH + "vault_obj.c");
  vault->move(this_object());
  vault->set_save_file(save_file);

  // Pequeño arreglo, las tiendas no compran mas objetos
  // hasta que no se deshagan de los anteriores (exceso de stock!!)
  prev = find_match(obs[0]->query_main_plural(), vault);
  if (sizeof(prev) >= MAX_OBS){
    notify_fail("Parece que ya tienen suficientes "+obs[0]->query_plural()+" en el almacén.\n");
    vault->dest_me();
    return 0;
  }

  if (sizeof(obs) > MAX_OBS) {
    write("El tendero no puede cargar con todos esos objetos.\n");
    obs = obs[0..MAX_OBS - 1];
  }

  selling = cannot = ({ });
  amt = total_amt = 0;

  for (i = 0; i < sizeof(obs); i++) {
    if ( (obs[i]->query_value() > 0) && !obs[i]->do_not_sell() &&
        (obs[i]->query_resale_value() != -1) &&
        // ((obs[i]->query_stolen_modifier() != -1) || !obs[i]->query_property("stolen")) && 
        !obs[i]->query_in_use()) {
      /* O.K. this SHOULD check to see if the item IS in the
      inventory before we move it */
      if (check_inv(obs[i],str)) 
      {
        string log;
        log = "";
        /* the call other (buried) in the below is so that we can update
           /std/shop and have all the shops prices fall in line
           consistently */
        amt = obs[i]->query_resale_value();
        if (!amt) {
          amt = scaled_value((int)obs[i]->query_value());
          if (amt > MAX_AMOUNT)
            amt = MAX_AMOUNT;
        }

        /* Let's not let something sell back for more than it was bought
           for */
        if (amt > obs[i]->query_value())
          amt = obs[i]->query_value();

        /* "hot" goods lose value!  */
        /*
        if (obs[i]->query_property("stolen")) {
          if (obs[i]->query_stolen_modifier() == 0)
            amt = amt * stolen_modifier / 100;
          else
            amt = amt * obs[i]->query_stolen_modifier() / 100;
        }
        */
        
        /* Adjusts price by item's adaptive multiplier (in 1/10 %) */
        /* Adjust by mudwide scale factor */
        // PENDIENTE: Ajustes de valor eliminados temporalmente, neverbot 05/06
        /*
        amt = amt * MONEY_TRACKER->query_adj_fact(SBFLAG)/1000;
        amt = do_weight_min(amt, obs[i]->query_weight(),
                            obs[i]->query_material(), 1);
        amt = do_cha_adjust(amt,this_player()->query_cha(),1);

        if ((amt = do_race_adjust(amt,this_player()->query_race(),1))== -101) {
          notify_fail("Esta tienda no trata con los de tu raza.\n");
          amt = 0;
          vault->dest_me();
          return 0;
        }
        */

        /* Counts how many of each item the shop buys
        if (!(this_player()->query_coder()) &&
             strsrch(this_player()->query_name(), "test") == -1)
          add_bought(obs[i]);
        */

        // Almacenamos el objeto segun el nuevo sistema, neverbot 7/03

        if (file_size(query_save_file()+".o") > FILE_SIZE) {
          write("El almacén ya está lleno, no se puede guardar nada más.\n");
          vault->dest_me();
          return 1;
        }

        if ((int)obs[i]->avoid_auto_load() || (int)obs[i]->query_no_save_object()) {
          write("No puedes vender tu "+obs[i]->short()+", no pueden " +
          "venderse este tipo de objetos.\n");
          vault->dest_me();
          return 1;
        }

        obs[i]->move(vault);
        log = "[COMPRA - " + ctime(time(),4) + "] " +
                   this_player()->query_cap_name() +
                   " - "+obs[i]->query_short()+" - Precio: " +
                   amt + "\n";
        log_file(query_log_file(), log);

        // Fin de nuevo sistema de almacenamiento

        total_amt += amt;
        selling += ({ obs[i] });
        // obs[i]->being_sold();
      }
      else if (obs[i]->short())
        cannot += ({ obs[i] });
      // end of test if exists, not that this DOESN'T do the cannot that it should
    }
    else if (obs[i]->short())
    cannot += ({ obs[i] });
  }

  if (!sizeof(selling)) {
    if (sizeof(cannot))
      notify_fail("No puedes vender "+query_multiple_short(cannot)+", tal vez lo lleves puesto, " +
        "o simplemente sea algo que aquí no aceptan.\n");
    else
      notify_fail("Nada que vender.\n");
    vault->dest_me();
    return 0;
  }

  if (sizeof(cannot))
    write("No puedes vender "+query_multiple_short(cannot)+", tal vez lo lleves puesto, o simplemente no lo tengas.\n");

  m_array = (mixed *)MONEY_HAND->create_money_array(total_amt);
  this_player()->adjust_money(m_array);
  do_parse(sell_mess, selling, this_player(), 
    (string)MONEY_HAND->money_string(m_array), "");
  if (sell_func)
    call_other(this_object(), sell_func, selling, total_amt);

  vault->dest_me();

  return 1;
}

int buy(string str) 
{
  int i, amt, ob_amt, total_cost;
  object *obs, *to_buy, *cannot, *too_much;
  string s;
  object vault;
  mixed aux;

  if (!check_open_condition())
    return 0;

  if (!str || str == "") {
    notify_fail("Sintaxis: comprar <objetos>\n");
    return 0;
  }

  // Dejamos el mismo sistema de seguridad de los vault_rooms
  if (query_property(VAULT_USE_PROP)){
    notify_fail("El almacén está siendo utilizado, inténtalo de nuevo en unos segundos.\n");
    return 0;
  }

  // Objeto con el inventario de la tienda
  vault = clone_object(VAULT_FILES_PATH + "vault_obj.c");
  vault->move(this_object());
  vault->set_save_file(save_file);

  obs = find_match(str, vault);

  if (!sizeof(obs)) {
    notify_fail("No encuentro "+str+".\n");
    vault->dest_me();
    return 0;
  }

  if (sizeof(obs) > MAX_OBS) {
    write("El tendero no puede cargar con todos los objetos.\n");
    obs = obs[0..MAX_OBS-1];
  }

  to_buy = too_much = cannot = ({ });

  amt = (int)this_player()->query_value();

  while (i < sizeof(obs)) {

    ob_amt = obs[i]->query_value();
    
    /* PENDIENTE:
    ob_amt = ob_amt * MONEY_TRACKER->query_adj_fact(SSFLAG)/1000;
    ob_amt = do_weight_min(ob_amt, obs[i]->query_weight(),
                           obs[i]->query_material(),0);
    ob_amt = do_cha_adjust(ob_amt,this_player()->query_cha(),0);

    if ((ob_amt = do_race_adjust(ob_amt,this_player()->query_race(),0))== -101) {
      notify_fail("Esta tienda no trata con los de tu raza.\n");
      vault->dest_me();
      return 0;
    }
    */

    if (ob_amt < 1) 
    {
      ob_amt = MIN_PRICE;
      log_file("shops", "[" + ctime(time(),4) + "]" + file_name(this_object())+": vende "+
                      file_name(obs[i]) + " por valor 0 a " +
                      this_player()->query_name()+" (se le asigna precio minimo = " +
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
      string log;
      log = "";
      amt -= ob_amt;
      total_cost += ob_amt;
      to_buy += ({ obs[i] });
      log = "[VENTA  - " + ctime(time(),4) + "] " +
           this_player()->query_cap_name() +
           " - "+obs[i]->query_short()+" - Precio: " +
           ob_amt + "\n";
          
      log_file(query_log_file(), log);
    }
    i++;
  }

  s = "";
  if (sizeof(cannot))
    s += "No puedes coger "+query_multiple_short(cannot)+".\n";
  if (sizeof(too_much))
    s += capitalize(query_multiple_short(too_much))+" cuesta demasiado.\n";
  if (!sizeof(to_buy)) {
    if (s != "")
      notify_fail(s);
    else
      notify_fail("Nada que comprar.\n");
    vault->dest_me();
    return 0;
  } else {
    write(s);
  }

  vault->dest_me();

  this_player()->pay_money( aux = (int)MONEY_HAND->create_money_array(total_cost));
  do_parse(buy_mess, to_buy, this_player(), (string)MONEY_HAND->money_string(aux), "");
  if (buy_func)
    call_other(this_object(), buy_func, to_buy, total_cost);

  return 1;
}

int browse(string str) 
{
  object *obs;
  int i, ob_amt;
  object vault;

  if (!check_open_condition())
    return 0;

  if (!str || str == "") {
    notify_fail("Sintaxis: "+query_verb()+" <objetos>\n");
    return 0;
  }

  // Dejamos el mismo sistema de seguridad de los vault_rooms
  if (query_property(VAULT_USE_PROP)){
    notify_fail("El almacén está siendo utilizado, inténtalo de nuevo en unos segundos.\n");
    return 0;
  }

  // Objeto con el inventario de la tienda
  vault = clone_object(VAULT_FILES_PATH + "vault_obj.c");
  vault->move(this_object());
  vault->set_save_file(save_file);

  obs = find_match(str, vault);

  if (!sizeof(obs)) {
    notify_fail("No encuentro "+str+".\n");
    vault->dest_me();
    return 0;
  }

  for (i = 0; i < sizeof(obs); i++) {
    ob_amt = obs[i]->query_value();
    
    /* PENDIENTE:
    ob_amt = ob_amt * MONEY_TRACKER->query_adj_fact(SSFLAG)/1000;
    ob_amt = do_weight_min(ob_amt, obs[i]->query_weight(), obs[i]->query_material(), 0);
    ob_amt = do_cha_adjust(ob_amt,this_player()->query_cha(),0);

    if ((ob_amt = do_race_adjust(ob_amt,this_player()->query_race(),0)) == -101) {
      notify_fail("Esta tienda no trata con los de tu raza.\n");
      vault->dest_me();
      return 0;
    }
    */

    if (ob_amt < 1)
      ob_amt = MIN_PRICE;

    do_parse(browse_mess, obs[i], this_player(), 
      (string)MONEY_HAND->money_value_string(ob_amt),
      (string)obs[i]->long());
    /*
    write("Examinas "+obs[i]->short()+", cuesta "+
    MONEY_HAND->money_string(obs[i]->query_money_array())+"\n"+
    obs[i]->long());
    */
  }
  vault->dest_me();
  return 1;
}

int value(string str) {
  object *obs;
  int i, val;

  if (!check_open_condition())
    return 0;

  if (only_sell) {
    tell_object(this_player(),"Esta tienda no compra bienes.\n");
    return 1;
  }

  if (!str || str =="") {
    notify_fail("Sintaxis: valorar <objetos>\n");
    return 0;
  }

  obs = find_match(str, this_player());

  if (!sizeof(obs)) {
    notify_fail("No encuentro "+str+".\n");
    return 0;
  }

  for (i = 0; i < sizeof(obs); i++) {
    /* the call other is so that we can change the PAY_RATES array, and
      then just update /std/shop to immediately and consistently effect
      all shops */
    val = obs[i]->query_resale_value();

    if (!val) {
      val = scaled_value((int)obs[i]->query_value());
      if (val > MAX_AMOUNT)
        val = MAX_AMOUNT;
    }

    if (val > obs[i]->query_value())
      val = obs[i]->query_value();

    /* "hot" goods lose value!  */
    /*
    if (obs[i]->query_property("stolen")) {
      if (obs[i]->query_stolen_modifier() == 0)
        val = val * stolen_modifier / 100;
      else
        val = val * obs[i]->query_stolen_modifier() / 100;
    }
    */
    
    /* adjust price offered by the adaptive multiplier of the item */
    /* PENDIENTE:
    val = val * MONEY_TRACKER->query_adj_fact(SBFLAG)/1000;
    val = do_weight_min(val, obs[i]->query_weight(), obs[i]->query_material(), 1);
    val = do_cha_adjust(val, this_player()->query_cha(), 1);

    if ((val = do_race_adjust(val,this_player()->query_race(),1))==-101) {
      notify_fail("Esta tienda no trata con los de tu raza.\n");
      return 0;
    }
    */

    do_parse(value_mess, obs[i], this_player(),
      (string)MONEY_HAND->money_value_string(val),
      (string)(obs[i]->do_not_sell() || (obs[i]->query_resale_value() == -1)) );
  }
  return 1;
}

// Funciones auxiliares

void do_parse(mixed arr, mixed ob, object client, string money, string extra) 
{
  if (stringp(arr))
    write(shop_parse(arr, ob, client, money, extra));
  else {
    write(shop_parse(arr[0], ob, client, money, extra));
    say(shop_parse(arr[1], ob, client, money, extra));
  }
}

string shop_parse(string str, mixed ob, object client, string money, string extra) 
{
  string s1, s2, s3, rest;

  rest = "";
  while(sscanf(str,"%s$%s$%s", s1, s2, s3) == 3){
    switch (s2) {
      case "ob" :
        if (pointerp(ob))
          str = s1+query_multiple_short(ob)+s3;
        else
          str = s1+ob->short()+s3;
        break;
      case "client" :
        str = s1+client->query_cap_name()+s3;
        break;
      case "extra" :
        str = s1+extra+s3;
        break;
      case "money" :
        str = s1+money+s3;
        break;
      default :
        rest = s1+"$"+s2+"$";
        str = s3;
        break;
    }
  }
  return rest+str;
}

/* The new adaptive pricing functions, keep track of buys and sells,
   return multipliers, figure out when and how to change multipliers,
   save and recall the shop's data */
/*
int do_weight_min(int amt,int mass, int material, int buysell) {
  if (buysell) {
    if (material == 2 && amt < mass) amt = mass;
    else if (material == 1 && amt < mass/4) amt = mass/4;
    else if (material == 3 && amt < mass/7) amt = mass/7;
    else if (amt < mass/10) amt = mass/10;
  }
  else {
    if (material == 2 && amt < mass*3/2) amt=mass*3/2;
    else if (material == 1 && amt < mass*3/8 ) amt=mass*3/8;
    else if (material == 3 && amt < mass*3/14) amt=mass*3/14;
    else if (amt < mass*3/20) amt = mass*3/20;
  }
  return amt;
}

int do_cha_adjust(int amt, int cha, int updown) {
  int temp;
  if (!cha) return amt;
  if (cha > 19) cha = 19;
  if (cha > AVG_CHA) {
    temp = 10*(cha - AVG_CHA)*CHA_AD/(18-AVG_CHA);
    if (updown) {
      temp = amt*(1000+temp)/1000-1;
      if (temp < amt) return amt;
    }
    else {
      temp = amt*1000/(1000+temp)+1;
      if (temp > amt) return amt;
    }
    return temp;
  }
  if (cha < AVG_CHA) {
    temp = 10*(AVG_CHA-cha)*CHA_AD/(AVG_CHA-3);
    if (updown) {
      temp = amt*1000/(1000+temp)+1;
      if (temp > amt) return amt;
    }
    else {
      temp = amt*(1000+temp)/1000-1;
      if (temp < amt) return amt;
    }
    return temp;
  }
  return amt;
}
*/

/* clean up the storeroom when we die */
/*
void dest_me(){
  ::dest_me();
}
*/

/* This checks the player to make sure they have the item.
   I *think* this is a paranoia check, but I'll leave it.
*/
int check_inv(object thing, string str)
{
  int i;
  object * inv;
  inv = find_match(str, this_player());

  for (i = 0; i < sizeof(inv); i++) {
    if (inv[i] == thing) return(1);
  }
  return(0);
}

/* This determines how much the shopkeeper will offer for an item, according
to its value.  It only gets called if set_no_resell() has not been called
and resale_value has not been hand-set to something.
*/
private int scaled_value(int n)
{
  int *prates;
  int i, tot;

  tot = 0;
  prates = PAY_RATES;
  for (i = 0; ( (i < sizeof(prates)) && (n > prates[i]) ); i+=2);

  if (i > 0)
    i-=2;
  /* prates[i] is now the rate-increment directly below the value. */

  tot = (n - prates[i]) * ((prates[i+2] / prates[i+3]) -
    (prates[i] / prates[i+1]));
  tot /= (prates[i+2] - prates[i]);
  tot += (prates[i] / prates[i+1]);

  /* For those curious, we just defined a line segment.  Basically, we
     used  y - y1 = m(x - x1).  And found the y value for x = n.
     Read shop.h for better explanation.
  */
  return tot;
}

int view_shop_log(string str)
{
   string name;
   string tmp;
 
   if (str) 
   {
      notify_fail("Sintaxis: 'logs'\n");
      return(0);
   }

   name = this_player()->query_name();
   
   // Sólo si es programador, si es admin, o si admin esta puesto a 'all'
   if (this_player()->query_coder() ||
      member_array(name, shop_admin) != -1 ||
      !sizeof(shop_admin) || 
      (shop_admin[0] == "all") )
   {
    tmp = read_file("/log/" + query_log_file());

    if (!tmp || tmp == "")
    {
      tell_object(this_player(), "El registro de actividades está vacío.\n");
      return 1;
    }

    this_player()->more_string(tmp);
    return 1;
   }
   
   notify_fail("No tienes permiso para ver el registro de actividades.\n");
   return 0;
}
