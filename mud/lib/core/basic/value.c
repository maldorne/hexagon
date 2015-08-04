
#include <basic/money.h>

// this will also handle value...
int value;
// int resale_value; /* Hamlet */
// int stolen_modifier; /* Hamlet */

void create() 
{
  value = 0;
  // resale_value = 0;
  // stolen_modifier = 0;
}

int adjust_money(mixed amt, varargs string type) 
{
  if (pointerp(amt)) 
  {
    value += (int)MONEY_HAND->query_total_value(amt);
    if (value < 0)
      value = 0;
    return value;
  }

  amt = (int)MONEY_HAND->query_total_value( ({ type, amt }) );
  value += amt;
  
  if (value < 0)
    value = 0;
  return value;
}

int adjust_value(int i) { return (value += i); }

mixed *query_money_array() { 
  return (mixed *)MONEY_HAND->create_money_array(value);
}

int query_money(string type) { 
  int i;
  mixed *m_a;

  m_a = (mixed *)MONEY_HAND->create_money_array(value); 
  if ((i=member_array(type, m_a)) == -1)
    return 0;
  return m_a[i+1];
}

void set_value(int i) { value = i; }
int query_value() { return value; }


/* resale value functions by Hamlet, August 1995 */
/* These are the price a shop will offer for an object. */
/*
void set_resale_value(int i) { resale_value = i; }
int adjust_resale_value(int i) { return( resale_value += i); }
int query_resale_value() { return resale_value; }

void prevent_resale() {  resale_value = -1;  }
void allow_resale() {  resale_value = 0;  }
*/
/* These are for how much the shop will offer for the item if it has
   been stolen. -- Hamlet
*/
/*
int set_stolen_modifier(int i) {
  if(i > 100)   i = 100;
  if(i < -1)    i = -1;

  stolen_modifier = i;
  return i;
}

int query_stolen_modifier() { return stolen_modifier; }
void no_sell_if_stolen() { stolen_modifier = -1; }
*/

mixed stats()
{
  return ({
            ({"Value", value, }),
            // ({"Resale Value", resale_value, }),
            // ({"Stolen Modifier", stolen_modifier, }),
          });
}