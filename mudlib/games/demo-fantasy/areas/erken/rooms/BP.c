inherit "/lib/ventures/pub.c";

#include <living/food.h>
#include "../path.h"
#include <language.h>

object board;

void setup()
{
  set_short(_LANG_ERKEN_PUB_SHORT);
  set_long(_LANG_ERKEN_PUB_LONG);

  // name.type.cost.heal.volume.intox
  add_menu_item(_LANG_ERKEN_PUB_MENU_LIQUOR, ALCOHOL, 20, 2, 300, 20);
  add_menu_item(_LANG_ERKEN_PUB_MENU_BLACK_BEER, ALCOHOL, 30, 3, 50, 50);
  add_menu_item(_LANG_ERKEN_PUB_MENU_WHISKEY, ALCOHOL, 100, 6, 100, 100);
  add_menu_item(_LANG_ERKEN_PUB_MENU_SCHNAPPS, ALCOHOL, 150, 15, 200, 200);
  add_menu_item(_LANG_ERKEN_PUB_MENU_RIBS, FOOD, 30, 5, 40, 0);

  add_exit(DIR_NORTH, ROOMS + "B5", "door");
  set_light(40);
  add_room_zone("erken");

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
