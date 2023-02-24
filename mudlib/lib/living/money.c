
//
// Money matters in living objects
//

#include <basic/money.h>
#include <translations/money.h>

mixed * money_array;

void create()
{
  money_array = ({ });
}

void start_money()
{
  object money;

  if (!money_array)
    money_array = ({ });

  // Done here after encumberance is set up
  money = clone_object(MONEY_OBJECT);
  money->set_money_array(money_array);
  money->move(this_object());
}

// This function is the easiest way of giving or taking money to/from a
// player/npc. It's easier than having to check if the player/npc
// has got a money object and talk to that directly.
//
// NOTE! The money object (/obj/money.c) itself is partially screwed
//       and should be fixed. It doesn't stop you from adding illegal
//       money types.
//
// See /obj/handlers/money_handler.c for valid money types.
//
int adjust_money(mixed i, varargs string type) 
{
  object ob;
  int val, tmp;

  // This is the actual money object that keeps track of how
  // much money the living has.
  ob = present(MONEY_NAME, this_object());

  // We didn't find it, clone one.
  if (!ob) 
  {
    ob = clone_object(MONEY_OBJECT);
    ob->move(this_object());
  }

  // This is just to find out how much money were actually given to/taken
  // from the player/npc. We find this out by noting how much was in
  // the money object before the transaction, make the transaction
  // and find out how much it has changed.
  val = (int)ob->query_value();
  ob->adjust_money(i, type);
  val = (int)ob->query_value()-val;

  if (ob->query_npc()) 
  {
    catch(tmp = val * MONEY_TRACKER->query_adj_fact(MONFLAG)/1000);

    if (tmp && tmp != val) 
    {
      ob->adjust_money(tmp-val, BASE_COIN);
      val = tmp;
    }
  }

  // Return how much was actually given to/taken from the player/npc
  return val;
}

// Simply a cover function that calls query_money_array() in the
// money object of this player/npc. Clones it if it doesn't exist.
//
// Why it does this I don't know, it's a waste of memory.
//
mixed * query_money_array() 
{
  object ob;

  ob = present(MONEY_NAME, this_object());

  if (!ob) 
  {
    ob = clone_object(MONEY_OBJECT);
    ob->move(this_object());
  }

  return (mixed *)ob->query_money_array();
}

// Simply a cover function that calls set_money_array() in the
// money object of this player/npc. Clones it if it doesn't exist.
//
// The code assumes whoever calls this function knows what he is doing
// as there are no validity checks.
//
void set_money_array(mixed *arr) 
{
  object ob;

  ob = present(MONEY_NAME, this_object());

  if (!ob) 
  {
    ob = clone_object(MONEY_OBJECT);
    ob->move(this_object());
  }

  ob->set_money_array(arr);
}

// Simply a cover function that calls query_value() in the
// money object of this player/npc. Clones it if it doesn't exist.
//
// Why it does this I don't know, it's a waste of memory.
//
int query_value() 
{
  object ob;

  ob = present(MONEY_NAME, this_object());

  if (!ob) 
  {
    ob = clone_object(MONEY_OBJECT);
    ob->move(this_object());
  }

  return (int)ob->query_value();
}

// Simply a cover function that calls query_money() in the
// money object of this player/npc. Clones it if it doesn't exist.
//
// Why it does this I don't know, it's a waste of memory.
//
int query_money(string type) 
{
  object ob;

  ob = present(MONEY_NAME, this_object());

  if (!ob) 
  {
    ob = clone_object(MONEY_OBJECT);
    ob->move(this_object());
  }

  return (int)ob->query_money(type);
}

// This is what most of the game uses, including the guilds, shops etc.
//
// The name of the function says it all doesn't it?
int pay_money(mixed *m_array) 
{
  int i, j, val;
  mixed *m_a;
  object ob;

  // Make sure the money object is present
  ob = present(MONEY_NAME, this_object());

  // Beats me why they want to load it. He obviously can't pay if he doesn't
  // have a moeny object to start with...
  if (!ob) 
  {
    ob = clone_object(MONEY_OBJECT);
    ob->move(this_object());
  }

  // Note the previous value.
  val = (int)ob->query_value();

  // Make the payment
  for (i=0;i<sizeof(m_array);i+=2) 
  {
    m_a = (mixed *)MONEY_HAND->make_payment(m_array[i], m_array[i+1],
                                            this_object());
    for (j=0;j<sizeof(m_a[0]);j+=2)
      ob->adjust_money(-m_a[0][j+1], m_a[0][j]);
    for (j=0;j<sizeof(m_a[1]);j+=2)
      ob->adjust_money(m_a[1][j+1], m_a[1][j]);
  }

  // Find out how much was actually paid
  val = (int)ob->query_value()-val;

  // Return that, just in case someone wants to know
  return val;
}

mixed stats()
{
  return ({ 
      ({ "Money Array", money_array }),
        });
}
