/* 
 * functions to say if a venture is open for business, Dimmak@Rl '99
 * reviewed for CcMud, neverbot '03
 * reviewed for Hexagon, neverbot '21
 */

#include <language.h>

string open_condition;
string attender_name;

// from shop.c
static int only_sell;

void create() 
{
  open_condition = "";
  attender_name = "";
  only_sell = 0;
}

void set_open_condition(string str) { open_condition = str; }
string query_open_condition() { return open_condition; }

int check_open_condition()
{
  if (!open_condition || !strlen(open_condition)) 
    return 1;
 
  notify_fail(_LANG_ATT_NON_ATTENDABLE);
  return call_other(this_object(), open_condition);
}

void set_attender(string str)
{
  attender_name = str;
  set_open_condition("standard_open_condition");
}

int standard_open_condition()
{
  object *list;
  object who;
  int i;
   
  // TODO check in the location if this is a component

  list = find_match(attender_name, this_object());
  if (!sizeof(list)) 
    return 0;

  who = list[0];
  list = who->query_attacker_list();
  for (i = 0; i < sizeof(list); i++)
    if (environment(list[i]) == this_object()) 
      return 0;
  
  return 1;
}

// from shop.c

void sell_only() { only_sell = 1; }
int query_only_sell() { return only_sell; }

mixed * stats()
{
  return ({
    ({ "Open Condition", open_condition }),
    ({ "Attender Name", attender_name }),
    ({ "Only Sell", only_sell }),
  });
}

