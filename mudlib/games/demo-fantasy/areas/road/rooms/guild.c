/* Created by Lummen and Antiron 15-7-97 */
/* Removed by neverbot 01/2021, we do not need guildrooms anymore */

#include "../path.h"
#include <language.h>

inherit "/lib/room.c";

static object board;
static object door;
 
void setup()
{
 set_short(_LANG_GUILD_SHORT);
 set_long(_LANG_GUILD_LONG);

 set_light(50);

 add_exit(DIR_EAST, ROOMS + "vault.c", "door");

 door = add_exit(DIR_OUT, ROOMS + "1.c", "door");
 if (door)
  door->set_init_status(0);
}

void reset()
{
  if (!board)
  {
    board = clone_object("/lib/obj/board");
    board->set_datafile("player-info");
    board->move(this_object());
  }   
  ::reset();
}

void dest_me()
{
  if (board)
    board->dest_me();
  ::dest_me();
}
