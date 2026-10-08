
#include <basic/move.h>

inherit light  "/lib/core/basic/light";
inherit move   "/lib/core/basic/move";
inherit weight "/lib/core/basic/weight";
inherit value  "/lib/core/basic/value";


void create() 
{
  move::create();
  weight::create();
  light::create();
  value::create();
}

int move(mixed dest, varargs mixed messin, mixed messout) 
{
  int i;
  object from;

  if (stringp(dest))
    dest = load_object(dest);

  if (!objectp(dest))
    return MOVE_EMPTY_DEST;  

  from = environment();

  if (!(dest->add_weight(weight)))
    return MOVE_TOO_HEAVY;

  i = move::move(dest, messin, messout);

  // the light moves with the object inside move::move, before the
  // destination hears of the arrival
  if (i == MOVE_OK) 
  {
    if (from) 
      from->add_weight(-weight);
  } 
  else
    dest->add_weight(-weight);
  
  return i;
}

void dest_me() 
{
  object *olist;
  int i;

  // the light and weight this object took from its environment are given
  // back by destruct itself, which every way of going away ends in
  olist = all_inventory(this_object());
  
  for (i = 0; i < sizeof(olist); i++)
  {
    if (olist[i]) 
      olist[i]->dest_me();
  }
  
  ::dest_me();
}

mixed * stats()
{
  return move::stats() + 
         weight::stats() +
         light::stats() +
         value::stats();
}
