/* 
 * Altered August 1995 - January 1996 by Hamlet Added weight and shortcut ways
 * to say 'platinum coins'. Added the cute little 'look coin' thingy. 
 */

inherit "/lib/core/object";

#include <basic/money.h>
#include <basic/move.h>
#include <areas/common.h>

#define WEIGHTDIV 2

mixed * money_array;
static int dont_join;
static int move_call_out_handle;

int query_number_coins();

// do not save this item in inventories!! 
// each living will have a mixed * money_array variable
// that will be saved instead
// if this function returns a non-empty mapping, it won't be saved
mixed query_auto_load() { return ([ "don't save" : 1 ]); }

void create() 
{
  money_array = ({ });
  move_call_out_handle = 0;

  ::create();

  set_name("moneda");
  add_alias(MONEY_NAME);
  set_main_plural("monedas");

  add_property("no one",1);
  add_property("money",1);

  // add_alias("coin");
  // add_plural("monedas");
  // add_plural("coins");
}

void init() 
{
  add_action("do_help", "ayuda");
}

int do_help(string str) 
{
  mixed *values;
  int i;

  if (!strlen(str) || ((str != "monedas") && (str != "moneda")))
    return 0;

  write("Valores monetarios:\n");
  values = (mixed *)MONEY_HAND->query_values();
  str = "";

  for (i=0;i<sizeof(values);i+=2)
    str += sprintf("%10-s : %4d %s\n", values[i], values[i+1], values[0]);

  write(printf("%-*s\n", this_player()->query_cols(), str));
  return 1;
}

int adjust_money(mixed amt, varargs string type) 
{
  int i, val;

  if (pointerp(amt)) 
  {
    for (i=0;i<sizeof(amt);i+=2)
      adjust_money(amt[i+1], amt[i]);
    return 1;
  }
  
  if (!stringp(type) || !intp(amt))
    return 0;

  /* Let's not allow unknown money types - Hamlet */
  if (member_array(type,(mixed *)MONEY_HAND->query_values()) == -1)
    return 0;

  if (environment(this_object()))
    if (interactive(environment(this_object()))) 
    {
      val = (int)MONEY_HAND->query_value(type);
      catch(MONEY_TRACKER->update_statistics(val*amt,
        environment(this_object())));
    }

  if ((i = member_array(type, money_array)) == -1) 
  {
    // add_adjective(type);
    add_plural("monedas");
    add_plural(type+"s");
    add_plural("monedas de "+type);
    add_alias("moneda de "+type); 
    add_alias("moneda");
    add_alias(type);  /* 'give 10 platinum to Hamlet' */

    /* order - highest value to the left of money_array */
    for (i = 0; i < sizeof(money_array); i+=2) 
    {
      val = (int)MONEY_HAND->query_value(money_array[i]);
      if ((int)MONEY_HAND->query_value(type) > val) 
        break;
    }

    money_array = money_array[0..(i-1)] + ({ type, amt }) +
        money_array[i..sizeof(money_array)-1];
  } 
  else 
  {
    money_array[i+1] += amt;
    if (money_array[i+1] <= 0) 
    {
      // remove_adjective(type);
      /* More Hamlet junk */
      remove_plural(type+"s");
      remove_plural("monedas de "+type);
      remove_alias(type);
        // remove_alias("moneda");
      remove_alias("moneda de "+type);
      money_array = delete(money_array,i,2);
      set_weight(query_number_coins()/WEIGHTDIV); /* Hamlet */

      return 0;
    }
    else
    {
      add_plural("monedas");
      add_plural(type+"s");
      add_plural("monedas de "+type);
      add_alias("moneda de "+type); 
      add_alias("moneda");
      add_alias(type);  /* 'give 10 platinum to Hamlet' */          
    }
  }

  set_weight(query_number_coins()/WEIGHTDIV); /* Hamlet */
    return money_array[i+1];
}

mixed *query_money_array() { return money_array; }

void set_money_array(mixed *arr) 
{ 
  int i;
  mixed *newarr;

  newarr = ({ });

  if (stringp(arr))
    return;

  for (i = 0; i < sizeof(arr); i+=2)
    if (member_array(arr[i],(mixed *)MONEY_HAND->query_values()) != -1)
      newarr += ({ arr[i], arr[i+1] });

  money_array = newarr;
  // adjectives = ({ }); /* clear them... */
  alias = ({ MONEY_NAME }); /* clear these too */
  plurals = ({ "monedas" }); /* clear these too */

  for (i = 0; i < sizeof(money_array); i+=2) 
  {
    // add_adjective(money_array[i]);
    add_plural("monedas");
    add_plural(money_array[i]+"s");
    add_plural("monedas de "+money_array[i]);
    add_alias(money_array[i]); /* Give 30 platinum to someone - Hamlet */
    add_alias("moneda de "+money_array[i]);
  }
  set_weight(query_number_coins()/WEIGHTDIV); /* Hamlet */
}

string *half_short() 
{
  string *retval;
  int i;

  retval = ({ });
  for (i=0;i<sizeof(money_array);i+=2)
    if (money_array[i+1] > 1)
        if (this_player() != environment() && money_array[i+1] > 10)
          retval += ({ "Varias monedas de "+money_array[i] });
        else
          retval += ({ money_array[i+1]+" monedas de "+ money_array[i] });
    else 
      if (money_array[i+1] == 1)
        retval += ({ "una moneda de "+money_array[i] });
  return retval;
}

string short(varargs int dark) 
{
  mixed retval;

  retval = half_short();
  if (!sizeof(retval))
    return "";
  if (sizeof(retval) == 1)
    return retval[0];

  return implode(retval[0..sizeof(retval)-2], ", ")+
      " y "+retval[sizeof(retval)-1];
}

/* 
string *pretty_short(int dark) 
{
  return half_short();
} 
*/

string long(string str, int dark) 
{
  /*
  string ret = "Tu miras a la moneda y ves: ";

  ret += ({ "some description", 
            "some description", 
            "some description", 
            "some description", 
            "some description", 
            "some description", 
            "some description", 
            "some description", 
            "some description", 
            "some description"
    })[random(10)];
  ret += "\n";
  return ret;
  */
  return "Son monedas comunes aceptadas en varias ciudades.\n";
}

object new_money_object(int num, string type) 
{
  int i;
  object ob;

  if (!sizeof(money_array))
    return nil;

  if (!type)
    i = 0;
  else if ((i = member_array(type, money_array)) == -1)
    return nil;

  if (num > money_array[i+1])
    num = money_array[i+1];

  ob = clone_object(MONEY_OBJECT);
  ob->adjust_money(num, type);
  if (type)
  {
    ob->add_plural("monedas");
    ob->add_plural(type+"s");
    ob->add_plural("monedas de "+type);
    ob->add_alias("moneda de "+type); 
    ob->add_alias("moneda");
    ob->add_alias(type);    
  }
  
  money_array[i+1] -= num;

  if (money_array[i+1] <= 0)
    money_array = delete(money_array, i, 2);

  /* Weight stuff -- Hamlet */
  set_weight(query_number_coins()/WEIGHTDIV);

  return ob;
}

int move(mixed dest, varargs mixed messin, mixed messout) 
{
  int j;
  object ob, mon;
  object from_where;
  from_where = environment();

  if (!from_where) 
    from_where = find_player(query_property("clonedin"));

  if (environment() ) 
    environment()->fix_my_loc_weight_later();

  j = (int)::move(dest, messin, messout);

  if (j != MOVE_OK)
    return j;

  if (objectp(dest))
    ob = dest;
  else 
  {
    ob = find_object(dest);
    if (!ob) 
    {
      catch(call_other(dest, "??"));
      ob = find_object(dest);
    }
  }
  
  remove_alias(MONEY_NAME);
  remove_call_out( move_call_out_handle );
  
  if (ob && (mon = present(MONEY_NAME, ob)) && !dont_join) 
  {
    for (j = 0; j < sizeof(money_array); j+=2)
      mon->adjust_money(money_array[j+1], money_array[j]);

    dont_join = 1;

    move(ROOM_VOID);

    if (from_where)
      if (interactive(from_where))
        catch(MONEY_TRACKER->update_statistics(-query_value(), from_where));

    call_out("dest_me",0);
    return MOVE_OK; 
  }
  
  add_alias(MONEY_NAME);

  if (ob && interactive(ob)) 
  {
    catch(MONEY_TRACKER->update_statistics(query_value(), ob));
  }

  if (from_where)
    if (!dont_join)
      catch(MONEY_TRACKER->update_statistics(-query_value(), from_where));

  set_weight(query_number_coins()/WEIGHTDIV);

  if (environment() ) 
    environment()->fix_my_loc_weight_later();

  return MOVE_OK;
}

object query_parse_id(mixed *arr) 
{
  int i, j, num;
  string s1, *bit;
  int n1;
  object ob;
  string cn1;

  if (!sizeof(money_array)) // neverbot
    return nil;

  bit = explode(arr[1], " ");

  if (bit[0] == "monedas" && sizeof(bit)==1)
    return this_object();

  if (bit[0] == "todo" || bit[0] == "all") 
  {
    if (environment(this_object()) != this_player())
      return this_object();
    else
      return nil;
  }

  // Añadido para evitar los nuevos ids
  if ((bit[0] == "cosa") || (bit[0] == "cosas") ||
      (bit[0] == "objeto") || (bit[0] == "objetos")) 
  {
    return nil;
  }

  i = member_array(bit[0], money_array);

  if (i == -1)
  {
    // 2 cobres, 2 monedas de platino, etc...
    if (atoi(bit[0]) > 0)
      bit -= ({ bit[0] });
    
    // Probamos si hemos escrito "cobres", "platinos"
    if ((i = member_array(bit[0][0..strlen(bit[0])-2], money_array)) != -1)
    {            
    }
    // Si solo hemos usado "moneda", cogemos una al azar
    else if (sizeof(bit)==1 && bit[0]=="moneda")
    {
      i = random(sizeof(money_array)/2)*2;
    }
    // Comprobamos que no estemos poniendo monedas de plata
    else if ( sizeof(bit)==3 && 
            (bit[0]=="monedas" || bit[0]=="moneda") && 
             bit[1]=="de")
    {
      i = member_array(bit[2], money_array);
      if (i == -1) 
        return nil;
    }
    else 
      return nil;
  }

  if (arr[0] == 0) 
  {
    num = money_array[i+1];
    s1 = money_array[i];
    money_array = delete(money_array, i, 2);
    // remove_adjective(s1);
    remove_plural("monedas de "+s1);
    remove_plural(s1 + "s");
    remove_alias("moneda de "+s1);
    remove_alias(s1);
    ob = clone_object(MONEY_OBJECT);
    ob->set_money_array(({ s1, num }));
      
    /* Weight stuff.  -- Hamlet */
    set_weight(query_number_coins()/WEIGHTDIV);
      
    ob->add_property("clonedin", environment()->query_name());
    ob->do_move_call_out(environment());
    return ob;
  }

  for (j = 0; j < sizeof(money_array); j+=2)
    /* This next line looks moronic, but trust me, it skirts a known sscanf()
       bug.    */
    if (sscanf(arr[1], "%s "+money_array[j],cn1)) 
    {
      i = j;
      sscanf(cn1,"%d",n1);
      if (n1 > 0)
        arr[0] = n1;
    }

  if (i == -1) 
  {
    for (j = 0; j < sizeof(money_array); j+=2)
      if (money_array[j] == arr[1][0..strlen(money_array[j])-1])
        i = j;
  }

  if (i == -1) 
  {
    i = member_array("cobre", money_array);
    if (i == -1)
      i = 0;
  }

  if ( (arr[1][strlen(arr[1])-5..strlen(arr[1])-1] == "monedas") &&
      (n1 <= 0) )
    arr[0] = money_array[i+1];

  if (arr[0] > money_array[i+1]) 
  {
    num = money_array[i+1];
    remove_plural(money_array[i]+"s");
    remove_plural("monedas de "+money_array[i]);
    remove_alias(money_array[i]);
    remove_alias("moneda de "+money_array[i]);
    // remove_adjective(money_array[i]);
  }
  else
  {
    num = arr[0];
  }

  money_array[i+1] -= num;
  arr[0] -= num;
  ob = clone_object(MONEY_OBJECT);
  ob->set_money_array( ({ money_array[i], num }) );

  /* Weight stuff.  -- Hamlet */
  set_weight(query_number_coins()/WEIGHTDIV);

  ob->add_property("clonedin",environment()->query_name());
  ob->do_move_call_out(environment());  
  return ob;
}

/* you cannot sell money... */
int do_not_sell() { return 1; }
void do_move_call_out(object ob) { move_call_out_handle = call_out("move", 0, ob); }

int query_value() { return (int)MONEY_HAND->query_total_value(money_array); }

int query_number_coins()
{
  int i, tot;

  if (!sizeof(money_array)) 
      return 0;
  for (i=1;i<sizeof(money_array);i+=2)
    tot += money_array[i];
  return tot;
}

// neverbot 07/04
int is_money() { return 1; }
