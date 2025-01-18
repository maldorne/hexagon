/*
 * Changes for Hexagon, neverbot 01/2025
 * 
 * - pub functions refactored in several files, to be inherited both
 *   from the pub room (/lib/ventures/pub.c) and the pub component
 *   for locations (/lib/location/components/pub.c). More info about
 *   previous changes in the header comments of the original pub file.
 *
 */

#include <living/consents.h>
#include <living/food.h>
#include <translations/money.h>
#include <language.h>

#define AUTOMATIC 1

void init() 
{
  ::init();

  add_action("do_buy", _LANG_PUB_BUY_CMDS);
}

int do_buy(string str)
{
  int volume, intox, heal, type,forplay;
  int i;
  // mixed *cost;
  int cost;
  // I hope cost didn't need to be mixed :)   Anirudh
  string *list;
  string drmess, othmes;
  object forwho;
  mapping menu_items, menu_aliases;

  // neverbot
  if (!strlen(str))
  {
    notify_fail(_LANG_PUB_BUY_WHAT);
    return 0;
  }

  list = explode(str, " ");
  
  if (!sizeof(list))
  {
    notify_fail(_LANG_PUB_BUY_SYNTAX);
    return 0;
  }

  if (!this_object()->check_open_condition()) 
    return 0;

  forplay = member_array(_LANG_PUB_BUY_PREPOSITION, list);

  if (forplay > 0)
  {
    if (sizeof(list) == forplay)
    {
      notify_fail(_LANG_PUB_BUY_SYNTAX);
      return 0;
    }

    notify_fail(_LANG_BUY_FOR_NOT_HERE);

    forwho = present(list[forplay + 1]);
    
    if (!forwho)
      return 0;

    if (!present(forwho) || forwho->query_hide_shadow()) 
      return 0;

    if (!forwho->query_consent(DRINK_CONS))
    {
      write(_LANG_PUB_BUY_FOR_REJECT_MSG_ME);
      tell_object(forwho, _LANG_PUB_BUY_FOR_REJECT_MSG_THEIR);
      tell_room(this_object(), _LANG_PUB_BUY_FOR_REJECT_MSG_ENV, ({ this_player(), forwho}) );
      return 1;
    }
    
    str = "";
    
    for (i = 0;i < forplay;i++) 
    {
      str += list[i];
      if (i != (forplay-1))
        str += " ";
    }
  }
  else 
    forwho = this_player();

  menu_aliases = this_object()->query_menu_aliases();
  menu_items = this_object()->query_menu_items();

  if (menu_aliases[str])
    str = menu_aliases[str];
  
  if (!menu_items[str]) 
  {
    notify_fail(_LANG_PUB_BUY_ITEM_NOT_FOUND);
    return 0;
  }
  
  heal = menu_items[str][DR_HEAL];
  cost = menu_items[str][DR_COST];

  switch (menu_items[str][DR_TYPE]) 
  {
    case FOOD: 
      type = D_FOOD;
      break;
    // both alcoholic and non-alcoholic drinks are treated as drinks
    case ALCOHOL:
    case DRINK: 
      type = D_DRINK;
    break;
  }

  volume = menu_items[str][DR_VOLUME];
  
  // if (menu_items[str][DR_TYPE]==DRINK)
  //   volume =- volume;
  
  intox = menu_items[str][DR_INTOX];
  drmess = menu_items[str][DR_DRMESS];
  othmes = menu_items[str][DR_OTHMES];

  // cost = cost * MONEY_TRACKER->query_adj_fact(TAVFLAG)/1000;
  // remove charisma price adjustment, the prices are written in a static menu, 
  // and it is the same for every player  
  // cost = this_object()->do_cha_adjust(cost, this_player()->query_cha(), 1);

  if (this_player()->query_value() < cost)
  {
    notify_fail(_LANG_PUB_BUY_CANNOT_AFFORD);
    return 0;
  }

  cost -= heal * this_object()->get_min_per_heal(menu_items[str][DR_TYPE], volume);

#ifdef AUTOMATIC
  this_player()->pay_money( ({ BASE_COIN, cost }) );

  forwho->adjust_volume(type, volume);
  forwho->adjust_hp(heal, this_player());
  forwho->adjust_intox(intox);
  
  if (forwho != this_player())
  {
    write(_LANG_PUB_BUY_FOR_REJECT_MSG_ME);
    tell_object(forwho, _LANG_PUB_BUY_FOR_REJECT_MSG_THEIR);
    tell_room(this_object(), _LANG_PUB_BUY_FOR_REJECT_MSG_ENV, ({ this_player(), forwho}) );
  }
  else
  {
    tell_object(forwho, drmess + ".\n");
    tell_room(this_object(), forwho->query_cap_name() + " " + othmes + ".\n", ({ forwho }));    
  }
#else

  // food = clone_object("/obj/food");
  // food->set_type(type);
  // food->set_name(str);
  // food->set_short(str);
  // food->set_heal(heal);
  // food->set_intox(intox);
  // food->set_volume(volume);
  // food->set_weight(5);
  // food->set_consume_write(drmess);
  // food->set_consume_say(othmes);
  // error = (int)food->move(forwho);
  // if (error) 
  // {
  //   write("No puedes cargar con algo así.\n");
  //   food->dest_me();
  //   return 1;
  // }
  // this_player()->pay_money( ({"cobre", cost }) );
#endif

  return 1;
}

