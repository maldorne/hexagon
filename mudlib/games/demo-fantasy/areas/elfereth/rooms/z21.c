// Paris. Abr'98

#include "../path.h"
#include <language.h>

inherit "/lib/ventures/shop.c";

void setup()
{
  set_short(_LANG_FOREST_TOWN_SHOP_SHORT);
  set_long(_LANG_FOREST_TOWN_SHOP_LONG);

  set_light(60);
  add_property("no_undead", 1);
  add_exit(DIR_SOUTH, ROOMS + "z20.c", "door");

  // will be restocked when there are any one in the inventory
  add_permanent_goods(BASEOBS + "armours/shirt.c", 2);
  add_permanent_goods(BASEOBS + "armours/cape.c", 1);
  add_permanent_goods(BASEOBS + "armours/trousers.c", 1);
  add_permanent_goods(BASEOBS + "misc/rope.c", 1);
  add_permanent_goods(BASEOBS + "misc/torch.c", 2);
  add_permanent_goods(BASEOBS + "weapons/shovel.c", 1);
  add_permanent_goods(BASEOBS + "weapons/dagger.c", 2);
  add_permanent_goods(BASEOBS + "weapons/short_sword.c", 1);
  add_permanent_goods(BASEOBS + "weapons/long_sword.c", 1);

  // add_clone(NPCS + "some_npc.c", 1);
  // set_attender("some_npc");
}
