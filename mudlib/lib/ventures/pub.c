/*
 * Old pub review for CcMud, neverbot 7/03
 * Added attendable by Dimmak@Rl (and updated), neverbot 4/7/03
 * Remove moving prices, neverbot 05/2009
 */

inherit "/lib/room.c";
inherit "/lib/ventures/attendable.c";

#define AUTOMATIC 1

#include <living/food.h>
#include <basic/money.h>
// #include <timestuff.h>
// #include <money_adjust.h>
#include <living/consents.h>
#include <language.h>

static mapping menu_items, menu_aliases;
static string menu_header;
static object menu_object;
int display_headings, last_adjust, pts_healed, last_sign;

// Taniwha 1995 , block clean_up() from removing items
int clean_up(int flag) { return 1; }
int clean_up_room(int flag) {  return 1; }

void set_menu_header(string header) { menu_header = header; }
string query_menu_header() { return menu_header; }
void set_display_headings(int value) { display_headings = value; }
int query_display_headings() { return display_headings; }
void add_menu_alias(string alias, string actual) { menu_aliases[alias] = actual; }
mapping query_menu_aliases() { return menu_aliases; }
int remove_menu_alias(string alias) { menu_aliases = map_delete(menu_aliases, alias); }

// remove charisma price adjustment, the prices are written in a static menu, 
// and it is the same for every player
// int do_cha_adjust(int amt, int cha);

int * query_data() { return ({ last_adjust, pts_healed, 0});}
int query_pub(){ return 1; }
private string menu_text();

void create_sign()
{
  menu_object = add_sign(_LANG_PUB_SIGN_DESC, menu_text());

  // menu_object = clone_object("/obj/misc/menu");
  // menu_object->set_pub(this_object());
  // menu_object->move(this_object());
  // menu_object->reset_get();
}

void create()
{
  menu_items = ([ ]);
  menu_aliases = ([ ]);
  menu_header = "En el menú hay:";
  display_headings = 1;

  ::create();

  create_sign();

  // include the pub in the ventures handler
  if (base_name(this_object()) != "/lib/ventures/pub")
    handler("ventures")->include_pub(base_name(this_object()));
}

void init() 
{
  ::init();

  add_action("do_buy", _LANG_PUB_BUY_CMDS);
}

int get_min_per_heal(string type, int volume) 
{
  int min_per_heal;

  min_per_heal = NORMAL_WCOST;

  if (type == ALCOHOL)
    min_per_heal = (1000 - volume) * min_per_heal / 1000;
  else if (type == SOFTDRINK) 
    min_per_heal = (1000 + volume) * min_per_heal / 1000;

  if (min_per_heal < 1) 
    min_per_heal = 1;
  return min_per_heal;
}

mapping query_menu_items() { return menu_items; }

void add_menu_item(string name, string type, 
                   int basecost, int heal, int volume,
                   int intox, varargs string drmess, string othmess) 
{
  int cost, min_per_heal;

  cost = basecost;

  min_per_heal = get_min_per_heal(type, volume);

  if (cost < heal * min_per_heal + 1) 
    cost = heal * min_per_heal + 1;

  if (undefinedp(drmess))
    drmess = type == FOOD ? _LANG_PUB_DEFAULT_FOOD_MESS_ME : _LANG_PUB_DEFAULT_DRINK_MESS_ME;
  if (undefinedp(othmess))
    othmess = type == FOOD ? _LANG_PUB_DEFAULT_FOOD_MESS_OTHERS : _LANG_PUB_DEFAULT_DRINK_MESS_OTHERS;

  menu_items[name] = ({ type, cost, heal, volume, intox,
    drmess, othmess, basecost });
}

int remove_menu_item(string name) 
{
  if (!menu_items[name])
    return 0;
  menu_items = m_delete(menu_items, name); 
  return 1;
}

private string _string_menu(string * items) 
{
  int i, temp;
  string str;
  
  str = ""; 
  
  for (i = 0; i < sizeof(items); i++)
  {
    temp = menu_items[items[i]][DR_COST];
    // temp = menu_items[items[i]][DR_COST]*MONEY_TRACKER->
    //  query_adj_fact(TAVFLAG)/1000;
    // temp = do_cha_adjust(temp, this_player()->query_cha());

    str += sprintf("  %-18s :  %s\n", capitalize(items[i]),
      MONEY_HAND->money_value_string(temp));
  }

  return str;
}

// Fix by !Aragorn
int compare(string a, string b) { return menu_items[a][DR_COST]-menu_items[b][DR_COST]; }

private string menu_text()
{
  int i;
  string * foods, * drinks, * alcohols;
  string ret, * stuff;
  string menu_foods, menu_drinks, menu_alcohols;

  foods = ({ });
  drinks = ({ });
  alcohols = ({ });
  menu_foods = "";
  menu_drinks = "";
  menu_alcohols = "";

  ret = menu_header + "\n";
  stuff = map_indices(menu_items);

  if (display_headings) 
  {
    menu_foods = _LANG_PUB_MENU_FOODS + ":\n";
    menu_drinks = _LANG_PUB_MENU_DRINKS + ":\n";
    menu_alcohols = _LANG_PUB_MENU_ALCOHOL + ":\n";
  }

  for (i = 0; i < sizeof(stuff); i++)
  {
    if (menu_items[stuff[i]][DR_TYPE] == FOOD) 
      foods += ({ stuff[i] });
    else if (menu_items[stuff[i]][DR_TYPE] == SOFTDRINK) 
      drinks += ({ stuff[i] });
    else if (menu_items[stuff[i]][DR_TYPE] == ALCOHOL)
      alcohols += ({ stuff[i] });
  } 

  if (sizeof(foods) > 0) 
  {
    foods = sort_array(foods, "compare", this_object());
    menu_foods += _string_menu(foods);
    ret += sprintf("%-=*s\n", this_user() ? this_user()->query_cols() : 79, menu_foods);
  }

  if (sizeof(drinks) > 0) 
  {
    drinks = sort_array(drinks, "compare", this_object());
    menu_drinks += _string_menu(drinks);
    ret += sprintf("%-=*s\n", this_user() ? this_user()->query_cols() : 79, menu_drinks);
  }

  if (sizeof(alcohols) > 0) 
  {
    alcohols = sort_array(alcohols, "compare", this_object());
    menu_alcohols += _string_menu(alcohols);
    ret += sprintf("%-=*s\n", this_user() ? this_user()->query_cols() : 79, menu_alcohols);
  }

  return menu_foods + "\n" + menu_drinks + "\n" + menu_alcohols + "\n" +
         _LANG_PUB_MENU_INSTRUCTIONS;
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

  if (!check_open_condition()) 
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
    case FOOD : type = D_FOOD;
    break;
    case ALCOHOL : type = D_ALCOHOL;
    break;
    case SOFTDRINK : type = D_ALCOHOL;
    break;
  }

  volume = menu_items[str][DR_VOLUME];
  
  if (menu_items[str][DR_TYPE]==SOFTDRINK)
    volume =- volume;
  
  intox = menu_items[str][DR_INTOX];
  drmess = menu_items[str][DR_DRMESS];
  othmes = menu_items[str][DR_OTHMES];

  // cost = cost * MONEY_TRACKER->query_adj_fact(TAVFLAG)/1000;
  // cost = do_cha_adjust(cost, this_player()->query_cha());

  if (this_player()->query_value() < cost)
  {
    notify_fail(_LANG_PUB_BUY_CANNOT_AFFORD);
    return 0;
  }

#ifdef AUTOMATIC
  this_player()->pay_money( ({ "cobre", cost }) );
  forwho->adjust_volume(type, volume);
  forwho->adjust_hp(heal,this_player());
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

  cost -= heal * get_min_per_heal(menu_items[str][DR_TYPE], volume);
  pts_healed += heal;

  return 1;
}

// int do_cha_adjust(int amt, int cha) 
// {
//   int temp;
//   if (!cha) 
//     return amt;
//   if (cha > 19) 
//     cha = 19;
//   if (cha > AVG_CHA) 
//   {
//     temp = 10*(cha - AVG_CHA)*CHA_AD/(18-AVG_CHA);
//     temp = amt*1000/(1000+temp)+1;
//     if (temp > amt) return amt;
//     return temp;
//   }
//   if (cha < AVG_CHA) {
//     temp = 10*(AVG_CHA-cha)*CHA_AD/(AVG_CHA-3);
//     temp = amt*(1000+temp)/1000-1;
//     if (temp < amt) return amt;
//     return temp;
//   }
//   return amt;
// }

void dest_me() 
{
  if (menu_object)
    menu_object->dest_me();
  ::dest_me();
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
