/* 
 *  Diplomacia, ciudadanias, leyes, etc.
 *  Functions to be included in every room
 *
 *  Folken 10/08/2013
 */

#include <diplomacy.h>

void create()
{

}

void dest_me()
{

}

mixed * stats() 
{
  return ({ }); 
} 

object query_diplomacy_info()
{
  object handler;
  handler = load_object(DIPLOMACY_HANDLER);
  
  if (!handler)
    return nil;

  // Only rooms and directories in domains will have diplomacy data
  if (!is_in_domain(this_object()))
    return nil;

  return handler->query_diplomacy_info_by_path(base_name(this_object()));
}

string query_diplomacy_name()
{
  object handler;
  handler = load_object(DIPLOMACY_HANDLER);
  
  if (!handler)
    return "";

  // Only rooms and directories in domains will have diplomacy data
  if (!is_in_domain(this_object()))
    return "";

  return handler->query_diplomacy_name(base_name(this_object()));  
}

// POI_TOWN_SQUARE 1
// POI_TOWN_DOOR 2
// POI_GUARD_CREATION 3
// POI_GUARD_POINT 4

void set_point_of_interest(int type)
{
  object handler;
  handler = load_object(DIPLOMACY_HANDLER);
  
  if (!handler)
    return;

  // Only rooms and directories in domains will have diplomacy data
  if (!is_in_domain(this_object()))
    return;

  switch(type)
  {
    case POI_TOWN_SQUARE :
      break;
    case POI_TOWN_DOOR :
      break;
    case POI_GUARD_CREATION :
      handler->add_diplomacy_poi(query_diplomacy_name(), POI_GUARD_CREATION, base_name(this_object()));
      break;
    case POI_GUARD_POINT :
      break;
  }
}
