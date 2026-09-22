// Created by Lummen and Antiron 26-7-97

inherit "/lib/ventures/pub.c";

#include <living/food.h>
#include "../path.h"
#include <language.h>

object board;

void setup()
{

  set_short(_LANG_NADUK_PUB_SHORT);
  set_long(_LANG_NADUK_PUB_LONG);

  // name.type.cost.heal.volume.intox
  add_menu_item(_LANG_NADUK_PUB_MENU_BEER, ALCOHOL, 20, 2, 10, 10);
  add_menu_item(_LANG_NADUK_PUB_MENU_BLACK_BEER, ALCOHOL, 30, 3, 10, 15);
  add_menu_item(_LANG_NADUK_PUB_MENU_WHISKEY, ALCOHOL, 100, 6, 100, 100);
  add_menu_item(_LANG_NADUK_PUB_MENU_SCHNAPPS, ALCOHOL, 120, 15, 200, 200);
  add_menu_item(_LANG_NADUK_PUB_MENU_MILK, DRINK, 10, 1, 5, 0);
  add_menu_item(_LANG_NADUK_PUB_MENU_OMELETTE, FOOD, 80, 15, 100, 0);
    
  add_exit(DIR_NORTH, ROOMS + "a4.c", "door");
  set_light(60);
  
  add_property("no_undead", 1);
  
  add_clone(NPCS + "barman.c", 1);

  set_attender("barman");
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
