/* 
 *  Navegacion y mapeado automatico
 *  Funciones a incluir en todas las rooms
 *
 *  Folken 08/08/2013
 */

#include <areas/navigation.h>

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

object query_navigation_handler()
{
  object handler;

  handler = load_object(NAVIGATION_HANDLER);
  
  if (!handler)
    return nil;

  // Only rooms and directories in domains will have navigation data
  if (!is_in_game(this_object()))
    return nil;

  return handler->query_navigation_handler(base_name(this_object()));
}
