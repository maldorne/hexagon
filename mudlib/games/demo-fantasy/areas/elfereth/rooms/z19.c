// Created by Lummen and Antiron 26-7-97 

inherit "/lib/ventures/pub.c";

#include <living/food.h>
#include "../path.h"
#include <language.h>

object board;

void setup()
{
  set_short(_LANG_FOREST_TOWN_PUB_SHORT);
  set_long(_LANG_FOREST_TOWN_PUB_LONG);

  // name.type.cost.heal.volume.intox
  add_menu_item(_LANG_FOREST_TOWN_PUB_MENU_MEAD, ALCOHOL, 150, 15, 200, 200);
  add_menu_item(_LANG_FOREST_TOWN_PUB_MENU_BERRIES, FOOD, 50, 10, 40, 0);
  add_menu_item(_LANG_FOREST_TOWN_PUB_MENU_ROOTS, FOOD, 30, 5, 40, 0);

  add_exit(DIR_WEST, ROOMS + "z18", "door");
  set_light(40);

  add_property("no_undead", 1);

  add_clone(NPCS + "barman.c", 1);

  set_open_condition("barman");
}
 
void reset()
{
  if (!board)
  {
    board=clone_object("/lib/obj/board.c");
    board->set_datafile("player-info");
    board->move(this_object());
  }
  ::reset();
}

void dest_me()
{
  if (board) board->dest_me();
  ::dest_me();
}
