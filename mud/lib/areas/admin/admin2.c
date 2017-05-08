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
	set_long("This is a black marble hallway. Power flows through"+
	     " the very structure"+
		" that serves as the walls of this hallway. You are afraid that"+
		" the power"+
		" will someday burst out of the crystal and destroy this"+
		" wonderous place."+
		" A door lies to your north, and to your south. A golden"+
		" plaque on the "+
		"northern door reads: Mail Control. A identical plaque"+
		"on the southern door reads: Quest regulation.\n");
	set_short("Admin Hallway");
	add_exit("norte",ADMIN+"mail_room","standard");
	add_exit("sur",ADMIN+"quest_room","standard");
	add_exit("este",ROOM+"admin3","road");
	add_exit("oeste",ROOM+"admin1","road");
	add_item("carpet","There is none.");
	add_item("plaque","It is made of gold and magically sealed"+
		" to the door.\n");
	set_light(100);
}      

