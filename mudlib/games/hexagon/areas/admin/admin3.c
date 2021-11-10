#define IMM1 " "
#define IMM2 " "

#include "path.h"

inherit "/lib/room.c";

void init() {
  /*
  shield_it(SHIELD,"/d/ss/daggerford/ladyluck");
  */
  ::init();
}

void setup() {
  set_long("This is a black marble hallway. Power flows"+
    " through the very structure"+
    " that serves as the walls of this halway. You are afraid"+
    " that the power"+
    " will someday burst out of the crystal and destroy this"+
    " wonderous place."+
    " A door lies to your north, and to your south. A golden "+
    "plaque on the "+
    "northern door reads: Domain control. A identical plaque"+
    "on the southern door reads:  Site security. A door to the"+
    " east has yet"+
    " another plaque on it that reads: File Access security.\n");
  set_short("Admin Hallway");
  
  add_exit(DIR_NORTH,ADMIN+"domain_control","standard");
  add_exit(DIR_SOUTH,ADMIN+"site_control","standard");
  add_exit(DIR_EAST,ADMIN+"access_control","standard");
  add_exit(DIR_WEST,ROOM+"admin2","road");
  add_exit(DIR_SOUTHEAST,ADMIN+"patrician.c","standard");
  add_exit(DIR_NORTHEAST,ADMIN+"weather.c","standard");

  add_item("carpet","There is none.");
  add_item("plaque","It is made of gold and magically sealed "+
    "to the door.\n");
  set_light(100);
}      

