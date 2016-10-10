
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

  // Taniwha 1995, maybe log this ?
  if(!dest)
    return 0;

  if (!objectp(dest))
    dest = load_object(dest);

  from = environment();

  if (!(dest->add_weight(weight)))
    return MOVE_TOO_HEAVY;

  i = move::move(dest, messin, messout);

  if (i == MOVE_OK) 
  {
    if (from) 
    {
      from->add_weight(-weight);
      from->adjust_light(-query_light());
    }

    if(environment())
      environment()->adjust_light(query_light());
  } 
  else
    dest->add_weight(-weight);
  
  return i;
}

void dest_me() 
{
  object *olist;
  int i;

  if (environment()) 
  {
    environment()->add_weight(-weight);
    set_light(0);
  }
  
  olist = all_inventory(this_object());
  
  for(i = 0; i < sizeof(olist); i++)
  {
    if(olist[i]) olist[i]->dest_me();
  }
  
  ::dest_me();
}

/*
mixed *query_init_data() {
   return light::query_init_data() +
          move::query_init_data() +
      ({ "weight", weight, "set_weight/p/",
         "value", value, "set_value/p/" });
} 
*/

mixed stats()
{
  return move::stats() + 
         weight::stats() +
         light::stats() +
         value::stats();
}
