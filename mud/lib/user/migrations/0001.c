/* ******************************************************
 *
 *  Test migration, you can take this as an example
 *     neverbot 10/2016
 * 
 *  Name your new migration files as consecutive numbers
 *   with four digits (0001.c, 0002.c, 0003.c and so on)
 *  Edit in /include/user/migration.h the #define 
 *   LAST_MIGRATION to be the last 
 *   available migration (1, 2, 3, etc)
 *
 * ****************************************************** */

#include <user/migration.h>

inherit BASE_MIGRATION;

void create()
{
  ::create();
}

int update_player(object player)
{
  tell_object(player, "Welcome to " + mud_name() + "\n");
  
  return 1; // <-- important
}
