// Created by Lummen 24-7-97

#include "../path.h"
#include <language.h>

inherit "/lib/ventures/shop.c";

void setup()
{
  set_short(_LANG_ERKEN_SHOP_SHORT);
  set_long(_LANG_ERKEN_SHOP_LONG);

  set_light(60);
  set_zone("erken");
  add_property("no_undead", 1);
  add_exit(DIR_NORTH, ROOMS + "B6.c", "door");

  // will be restocked when there are any one in the inventory
  add_permanent_goods(BASEOBS + "armours/shirt.c", 2);

  // add_clone(NPCS + "some_npc.c", 1);
  // set_attender("some_npc");
}
