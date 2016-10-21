/* ******************************************************
 *
 *     Base migration
 *
 *     Every player migration must inherit from this
 *        neverbot 10/2016
 *
 * ****************************************************** */

inherit "/lib/core/object";

#include <user/migration.h>

void create()
{
  ::create();
}

int query_migration() { return 1; }

int update_player(object player)
{
  return 1;
}

// mixed * stats() 
// {
//   return ({ 
//           });
// }
