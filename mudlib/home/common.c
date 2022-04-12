/*
 * Common coder room
 */

#define ADMIN "/games/hexagon/areas/admin/"

#include <translations/exits.h>

inherit "/lib/room";

object board;

void reset() 
{
  int i,n;
  object *all;
  
  if (!board) 
  {
    board = clone_object("/lib/obj/board");
    board->set_datafile("coders-common");
    board->move(this_object());
  }
  
  /* Try to clean some memory */
  // n = reclaim_objects();
  
  // for(i = 0; i < sizeof(all = children("/global/god.c")); i++)
  // {
  //   if(all[i])
  //     tell_object(all[i],"[ Objetos liberados (reclaim_object): "+ n +" ]\n");
  // }

  ::reset();
}

void setup() 
{
  set_light(60);
  set_short("Coder's common room");
  set_long("Here the coders of "+mud_name()+ " " +
        "meet to talk about anything related to the game. " +
        "Use the board to leave a note and write yout thoughts about any current issue, " +
        "it is your mud too... \n");

  // add_exit("cc", "/d/ciudad_capital/common", "standard");
  add_exit(DIR_SOUTH, ADMIN + "admin1", "standard");
  add_exit(DIR_NORTH, ADMIN + "meeting.c", "standard");
}

void dest_me() 
{
  if (board)
    board->dest_me();
  ::dest_me();
}
