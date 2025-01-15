/*
 * Changes for Hexagon, neverbot 01/2025
 * 
 * - shop functions refactored in several files, to be inherited both
 *   from the shop room (/lib/ventures/shop.c) and the shop component
 *   for locations (/lib/location/components/shop.c). More info about
 *   previous changes in the header comments of the original shop file.
 *
 */

#include <room/ventures.h>
#include <room/storage.h>
#include <language.h>

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

  if (!this_object()->check_open_condition())
    return 0;

  // same safety system as in the vault_rooms
  if (this_object()->query_property(VAULT_USE_PROP)) 
  {
    notify_fail(_LANG_SHOP_STORE_IN_USE);
    return 0;
  }

  // shop inventory object
  vault = clone_object(VAULT_FILES_PATH + "vault_obj.c");
  vault->move(this_object());
  vault->set_save_file(this_object()->query_save_file_name());

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
      write("Max amount: " + FILE_SIZE + ", current amount: " + 
            file_size(this_object()->query_save_file_name()) + "\n");
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

  if (!this_object()->check_open_condition())
    return 0;

  if (this_object()->query_only_sell())
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
  if (this_object()->query_property(VAULT_USE_PROP)) 
  {
    notify_fail(_LANG_SHOP_STORE_IN_USE);
    return 0;
  }

  if (file_size(this_object()->query_save_file_name()) > FILE_SIZE)
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
  vault->set_save_file(this_object()->query_save_file_name());

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
      amt = this_object()->scaled_value(obs[i]->query_value());
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
      amt = this_object()->do_cha_adjust(amt, this_player()->query_cha(), 1);

      if (obs[i]->avoid_auto_load() || obs[i]->query_no_save_object())
      {
        write(_LANG_SHOP_SELL_NOT_THIS_TYPE);
        vault->dest_me();
        return 1;
      }

      obs[i]->move(vault);

      log_file(this_object()->query_log_file_name(), "[shop buys - " + ctime(time(), 4) + "] " +
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

  m_array = (mixed *)handler("money")->create_money_array(total_amt);
  this_player()->adjust_money(m_array);

  this_object()->do_parse(this_object()->query_sell_mess(), selling, this_player(), 
                          (string)handler("money")->money_string(m_array), "");

  if (this_object()->query_sell_func())
    call_other(this_object(), this_object()->query_sell_func(), selling, total_amt);

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

  if (!this_object()->check_open_condition())
    return 0;

  if (!strlen(str))
  {
    notify_fail(_LANG_SHOP_BROWSE_SYNTAX);
    return 0;
  }

  // same safety system as in the vault_rooms
  if (this_object()->query_property(VAULT_USE_PROP)) 
  {
    notify_fail(_LANG_SHOP_STORE_IN_USE);
    return 0;
  }

  // shop inventory object
  vault = clone_object(VAULT_FILES_PATH + "vault_obj.c");
  vault->move(this_object());
  vault->set_save_file(this_object()->query_save_file_name());

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
    
    ob_amt = this_object()->do_cha_adjust(ob_amt, this_player()->query_cha(), 0);

    if (ob_amt < 1)
      ob_amt = MIN_PRICE;

    this_object()->do_parse(this_object()->query_browse_mess(), obs[i], this_player(), 
      (string)handler("money")->money_value_string(ob_amt),
      (string)obs[i]->long());
  }

  vault->dest_me();
  return 1;
}

int do_value(string str)
{
  object *obs;
  int i, amt;

  if (!this_object()->check_open_condition())
    return 0;

  if (!strlen(str))
  {
    notify_fail(_LANG_SHOP_VALUE_SYNTAX);
    return 0;
  }

  if (this_object()->query_only_sell())
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
    amt = this_object()->scaled_value(obs[i]->query_value());
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
    amt = this_object()->do_cha_adjust(amt, this_player()->query_cha(), 1);

    this_object()->do_parse(this_object()->query_value_mess(), obs[i], this_player(),
      (string)handler("money")->money_value_string(amt),
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

  if (!this_object()->check_open_condition())
    return 0;

  if (!strlen(str))
  {
    notify_fail(_LANG_SHOP_BUY_SYNTAX);
    return 0;
  }

  // same safety system as in the vault_rooms
  if (this_object()->query_property(VAULT_USE_PROP)) 
  {
    notify_fail(_LANG_SHOP_STORE_IN_USE);
    return 0;
  }

  // shop inventory object
  vault = clone_object(VAULT_FILES_PATH + "vault_obj.c");
  vault->move(this_object());
  vault->set_save_file(this_object()->query_save_file_name());

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

    ob_amt = this_object()->do_cha_adjust(ob_amt, this_player()->query_cha(), 0);

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

      log_file(this_object()->query_log_file_name(), "[shop sells - " + ctime(time(), 4) + "] " +
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

  this_player()->pay_money( aux = handler("money")->create_money_array(total_cost));

  this_object()->do_parse(this_object()->query_buy_mess(), to_buy, this_player(), 
                          (string)handler("money")->money_string(aux), "");
  
  if (this_object()->query_buy_func())
    call_other(this_object(), this_object()->query_buy_func(), to_buy, total_cost);

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
    return 0;
  }

  name = this_player()->query_name();
  
  // only coders, shop admins, or anybody if admins == "all"
  if (this_player()->query_coder() ||
     (member_array(name, this_object()->query_shop_admins()) != -1) ||
      member_array("all", this_object()->query_shop_admins()) != -1)
  {
   tmp = read_file(this_object()->query_log_file_name());

   if (!tmp || !strlen(tmp))
   {
     tell_object(this_player(), _LANG_SHOP_LOGS_EMPTY);
     return 1;
   }

   this_user()->more_string(tmp);
   return 1;
  }
  
  notify_fail(_LANG_SHOP_LOGS_NO_PERMISSION);
  return 0;
}
