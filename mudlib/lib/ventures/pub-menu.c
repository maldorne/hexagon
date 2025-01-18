/*
 * Changes for Hexagon, neverbot 01/2025
 * 
 * - pub functions refactored in several files, to be inherited both
 *   from the pub room (/lib/ventures/pub.c) and the pub component
 *   for locations (/lib/location/components/pub.c). More info about
 *   previous changes in the header comments of the original pub file.
 *
 */

#include <living/food.h>
#include <basic/money.h>
#include <language.h>

static mapping menu_items, menu_aliases;
static string menu_header;
int display_headings;

void add_menu_alias(string alias, string actual) { menu_aliases[alias] = actual; }
mapping query_menu_aliases() { return menu_aliases; }
int remove_menu_alias(string alias) { menu_aliases = map_delete(menu_aliases, alias); }

void set_menu_header(string header) { menu_header = header; }
string query_menu_header() { return menu_header; }
void set_display_headings(int value) { display_headings = value; }
int query_display_headings() { return display_headings; }

string query_menu_text();

mapping query_menu_items() { return menu_items; }

void create()
{
  menu_items = ([ ]);
  menu_aliases = ([ ]);
  menu_header = _LANG_PUB_DEFAULT_MENU_HEADER;
  display_headings = 1;
}

void add_menu_item(string name, string type, 
                   int basecost, int heal, int volume,
                   int intox, varargs string drmess, string othmess) 
{
  int cost, min_per_heal;

  stderr("🔥    add_menu_item: " + name + " " + type + " " + basecost + " " + heal + " " + volume + " " + intox + "\n");


  cost = basecost;

  min_per_heal = this_object()->get_min_per_heal(type, volume);

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

string query_menu_text()
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
    else if (menu_items[stuff[i]][DR_TYPE] == DRINK) 
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

mixed * stats()
{
  return ({
    ({ "Menu Items", menu_items }),
    ({ "Menu Aliases", menu_aliases }),
  });
}