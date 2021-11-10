
#include "path.h"

inherit "/lib/room.c";

object board;

void reset() 
{
  if(!board) 
  {
    board = clone_object("/lib/obj/board");
    board->set_datafile("coders-development");
    board->move(this_object());
  }
}

void setup() 
{
  set_light(80);
  set_short("Mud Development Office");
  set_long(
    "You are in the Development Office of the mud. Maps and charts of the "+
    "world adorn the panelled walls. Forms sit on a table near the door "+
    "for registration of new projects.\n");
  add_exit("norte", ADMIN + "admin1.c","standard");
}

void dest_me() 
{
  if (board)
    board->dest_me();
  ::dest_me();
}
