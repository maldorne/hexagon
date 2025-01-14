
#include <basic/money.h>

// this will also handle value...
int value;

void create() 
{
  value = 0;
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

mixed * query_money_array() 
{ 
  return (mixed *)MONEY_HAND->create_money_array(value);
}

int query_money(string type) 
{ 
  int i;
  mixed *m_a;

  m_a = (mixed *)MONEY_HAND->create_money_array(value); 

  if ((i = member_array(type, m_a)) == -1)
    return 0;
  return m_a[i+1];
}

void set_value(int i) { value = i; }
int query_value() { return value; }

mixed * stats()
{
  return ({
            ({ "Value", value, }),
          });
}
