// Paris. Apr'98

#include <language.h>

void create_descriptions(int num)
{
  add_property("no_undead", 1);

  switch (num)
  {
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
      set_short(_LANG_PATH_SHORT);
      set_long(_LANG_PATH_LONG);
      set_light(30);
    break;
    case 11:
    case 13:
    case 9:
    case 7:
    case 8:
    case 10:
    case 12:
    case 6:
      set_short(_LANG_FOREST_SHORT);
      set_long(_LANG_FOREST_LONG);
      set_light(30);
      add_room_zone("forest");
    break;
    case 17:
      set_short(_LANG_FOREST_CENTER_SHORT);
      set_long(_LANG_FOREST_CENTER_LONG);
      set_light(30);
    break;
    case 16:
    case 20:
    case 18:
    case 14:
      set_short(_LANG_FOREST_TOWN_SHORT);
      set_long(_LANG_FOREST_TOWN_LONG);
      set_light(30);
    break;
    case 15:
      set_short(_LANG_FOREST_TOWN_HOME_SHORT);
      set_long(_LANG_FOREST_TOWN_HOME_LONG);
      set_light(30);
    break;
  }
}
           
void create_exits(int num)
{
  switch (num)
  {
    case 1:
      add_exit(DIR_WEST, ROOMS + "z2.c", "path");
      add_exit(DIR_EAST, ROAD + "rooms/7.c", "path");
    break;
    case 2:
      add_exit(DIR_EAST, ROOMS + "z1.c", "path");
      add_exit(DIR_WEST, ROOMS + "z3.c", "path");
      add_exit(DIR_NORTHWEST, ROOMS + "z4.c", "path");
    break;
    case 3:
      add_exit(DIR_NORTH, ROOMS + "z4.c", "path");
      add_exit(DIR_EAST, ROOMS + "z2.c", "path");
    break;
    case 4:
      add_exit(DIR_WEST, ROOMS + "z5.c", "path");
      add_exit(DIR_SOUTH, ROOMS + "z3.c", "path");
      add_exit(DIR_SOUTHWEST, ROOMS + "z6.c", "path");
      add_exit(DIR_SOUTHEAST, ROOMS + "z2.c", "path");
    break;
    case 5:
      add_exit(DIR_EAST, ROOMS + "z4.c", "path");
      add_exit(DIR_WEST, ROOMS + "z10.c", "path");
      add_exit(DIR_UP, ROOMS + "z14.c", "path");
      add_exit(DIR_SOUTH, ROOMS + "z6.c", "path");
      add_exit(DIR_SOUTHWEST, ROOMS + "z9.c", "path");
    break;
    case 6:
      add_exit(DIR_WEST, ROOMS + "z9.c", "path");
      add_exit(DIR_NORTHEAST, ROOMS + "z4.c", "path");
      add_exit(DIR_SOUTH, ROOMS + "z7.c", "path");
      add_exit(DIR_NORTH, ROOMS + "z5.c", "path");
    break;
    case 7:
      add_exit(DIR_NORTH, ROOMS + "z6.c", "path");
      add_exit(DIR_WEST, ROOMS + "z8.c", "path");
    break;
    case 8:
      add_exit(DIR_EAST, ROOMS + "z7.c", "path");
      add_exit(DIR_NORTH, ROOMS + "z9.c", "path");
      add_exit(DIR_NORTHWEST, ROOMS + "z13.c", "path");
    break;
    case 9:
      add_exit(DIR_EAST, ROOMS + "z6.c", "path");
      add_exit(DIR_WEST, ROOMS + "z13.c", "path");
      add_exit(DIR_NORTHEAST, ROOMS + "z5.c", "path");
      add_exit(DIR_SOUTH, ROOMS + "z8.c", "path");
    break;
    case 10:
      add_exit(DIR_EAST, ROOMS + "z5.c", "path");
      add_exit(DIR_WEST, ROOMS + "z12.c", "path");
      add_exit(DIR_NORTH, ROOMS + "z11.c", "path");
    break;
    case 11:
      add_exit(DIR_SOUTH, ROOMS + "z10.c", "path");
      add_exit(DIR_SOUTHWEST, ROOMS + "z12.c", "path");
    break;
    case 12:
      add_exit(DIR_NORTHEAST, ROOMS + "z11.c", "path");
      add_exit(DIR_SOUTH, ROOMS + "z13.c", "path");
      add_exit(DIR_EAST, ROOMS + "z10.c", "path");
    break;
    case 13:
      add_exit(DIR_EAST, ROOMS + "z9.c", "path");
      add_exit(DIR_SOUTHEAST, ROOMS + "z8.c", "path");
      add_exit(DIR_NORTH, ROOMS + "z12.c", "path");
    break;
    case 14:
      add_exit(DIR_DOWN, ROOMS + "z5.c", "stairs");
      add_exit(DIR_NORTH, ROOMS + "z17.c", "path");
      add_exit(DIR_NORTHWEST, ROOMS + "z16.c", "path");
      add_exit(DIR_NORTHEAST, ROOMS + "z18.c", "path");
    break;
    case 15:
      add_exit(DIR_EAST, ROOMS + "z16.c", "path");
    break;
    case 16:
      add_exit(DIR_EAST, ROOMS + "z17.c", "path");
      add_exit(DIR_WEST, ROOMS + "z15.c", "path");
      add_exit(DIR_NORTHEAST, ROOMS + "z20.c", "path");
      add_exit(DIR_SOUTHEAST, ROOMS + "z14.c", "path");
    break;
    case 17:
      add_exit(DIR_NORTH, ROOMS + "z20.c", "path");
      add_exit(DIR_SOUTH, ROOMS + "z14.c", "path");
      add_exit(DIR_EAST, ROOMS + "z18.c", "path");
      add_exit(DIR_WEST, ROOMS + "z16.c", "path");
    break;
    case 18:
      add_exit(DIR_EAST, ROOMS + "z19.c", "door");
      add_exit(DIR_NORTHWEST, ROOMS + "z20.c", "path");
      add_exit(DIR_SOUTHWEST, ROOMS + "z14.c", "path");
      add_exit(DIR_WEST, ROOMS + "z17.c", "path");
    break;
    case 20:
      add_exit(DIR_SOUTH, ROOMS + "z17.c", "path");
      add_exit(DIR_SOUTHWEST, ROOMS + "z16.c", "path");
      add_exit(DIR_SOUTHEAST, ROOMS + "z18.c", "path");
      add_exit(DIR_NORTH, ROOMS + "z21.c", "door");
    break;
  }
}

void create_monsters(int num)
{
  switch (num)
  {
    case 3:
    case 5:
      add_clone(ROAD + "npcs/pilgrim.c", 1);
    break;
    case 11:
      add_clone(NPCS + "black_bear.c", 1);
    break;
    case 7:
    break;
    case 13:
      add_clone(NPCS + "wolf.c", 2);
    break;
    case 10:
      add_clone(NPCS + "spider.c", 1);
    break;
    case 14:
    case 18:
    case 16:
    case 20:
      add_clone(NPCS + "elf.c", 1);
    break;
  }
}
