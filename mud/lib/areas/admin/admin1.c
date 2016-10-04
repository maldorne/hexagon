#define IMM1 " "
#define IMM2 " "

#include "path.h"
inherit "/std/room.c";

void init() {
::init();
}

void setup() {
      set_long("This is a black marble hallway.  Power flows"+
	" through the very structure"+
	" that serves as the walls of this hallway.  You are afraid"+
	" that the power"+
	" will someday burst out of the crystal and destroy this"+
	" wonderous place."+
	"  A door lies to your north, and to your south.  A golden "+
	"plaque on the "+
	"northern door reads: Meeting Chambers of Immortals.  "+
	"A identical plaque  "+
	"on the southern door reads: Influence/ Territorial claims room.\n");

	set_short("Admin Hallway");
	add_exit("norte","/home/common","standard");
	add_exit("sur",ADMIN+"development","standard");
	add_exit("este",ROOM+"admin2","road");
	// add_exit("oeste",ROOM+"palace2","standard");
	
	add_item("carpet","There is none.");
	add_item("plaque","It is made of gold and magically sealed"+
		" to the door.\n");
	set_light(100);
}      

